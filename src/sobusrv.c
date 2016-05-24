#include <sys/types.h>
#include <signal.h>
#include "dest.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


void restore(Fich f){
    int  rd,wr,pipeToCliente,file,n;
    char path_sha1sum[BUFF];
    char path_pipe[BUFF];
    char path_link[BUFF];
    char path_fileGz[BUFF];
    char path_file[BUFF];
    
    if(!fork()){
        /*getDestinoMetadata*/
        strcpy(path_link,getDestMeta());
        strcat(path_link,f->nome);

        /*getDestinoSha1sum */
        n=readlink(path_link,path_sha1sum,BUFF);
        path_sha1sum[n]='\0';
        
        /*getPathFileGz*/
        strcpy(path_fileGz,getDestData());
        strcat(path_fileGz,"file.gz");

        /*getPathFile*/
        strcpy(path_file,getDestData());
        strcat(path_file,"file");

        /*getPathPipe*/
        strcpy(path_pipe,f->pipe);
        mkfifo(path_pipe,0666);
        kill(f->pid, SIGUSR1);

        pipeToCliente=open(path_pipe,O_WRONLY);
        
        if (!fork()) {
            
            execlp("cp","cp" ,path_sha1sum,path_fileGz, NULL);
            perror("Erro ao tentar calcular hash");
            _exit(1);
        }   
        wait(NULL);

        if (!fork()) {
            unlink(path_file);
            execlp("gunzip","gunzip" ,path_fileGz, NULL);
            perror("Erro ao tentar calcular hash");
            _exit(1);
        }   
        wait(NULL);

        file=open(path_file,O_RDONLY);

        rd=1;wr=1;
        Lido estrutura=(Lido) malloc (sizeof(struct lido));
        estrutura->fim=1;    
        while(rd && wr){
            estrutura->rd=read(file,estrutura->buffer,BUFF);
            if(!estrutura->rd) {estrutura->fim=0;rd=0;wr=0;}
            wr=write(pipeToCliente,estrutura,(sizeof(struct lido)));
            }
        close(file);
        unlink(path_file);
        unlink(path_pipe);
        kill(f->pid,SIGUSR1);
        _exit(0);
        
    }
    
}

void backup(Fich f){
    int  rd,wr,pipeFromCliente,file; 
    char destino_pipe[BUFF];
    char destino_data[BUFF];
    char destino_metadata[BUFF];
    

    if(!fork()){
        mkfifo(f->pipe,0666);
        kill(f->pid, SIGUSR1);
        pipeFromCliente=open(f->pipe,O_RDONLY);
        strcpy(destino_pipe,getDestData());
        rd=1;wr=1;
        
        /*Backup*/
        file=open(strcat(destino_pipe,f->nome),O_CREAT | O_RDWR , 0600 );
        Lido estrutura=(Lido) malloc (sizeof(struct lido));
        while(rd && wr){
            rd=read(pipeFromCliente,estrutura,(sizeof(struct lido)));
            if(estrutura->fim) wr=write(file,estrutura->buffer,estrutura->rd);
            else{rd=0; wr=0;}
            
        }

        
            if (!fork()) {
                
                execlp("gzip","gzip" , destino_pipe, NULL);
                perror("Erro ao tentar calcular hash");
                _exit(1);
            }   
            wait(NULL);
            strcpy(destino_data,getDestData());
            strcat(destino_data,f->sha1sum);
            if (!fork()) {
                strcat(destino_pipe,".gz");
                execlp("mv", "mv",destino_pipe,destino_data , NULL);
                perror("Erro ao tentar calcular hash");
                _exit(1);
            } 
            wait(NULL);
            
            strcpy(destino_metadata,getDestMeta());
            strcat(destino_metadata,f->nome);
            symlink(destino_data,destino_metadata);                    

        
       
        close(file);
        kill(f->pid,SIGUSR1);

       unlink(f->pipe);
       unlink(f->pipe);
       _exit(0);
    }
}


int main() {
    mkfifo(getDestPipe(), 0600);
	Fich f =malloc (sizeof (struct ficheiro));
    int p,n;
    if(!fork()){
        while(1){
            p = open(getDestPipe(), O_RDONLY);
            n=read(p,f,sizeof (struct ficheiro));
                if(n){
                    if(f->operacao[0]=='b') backup(f);
                    else if(f->operacao[0]=='r')restore(f);
                }
            close(p);
            } 
        unlink(getDestPipe());
        _exit(0);
    }

    return 0;	
}
