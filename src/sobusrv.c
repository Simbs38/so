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


#define BUFF 1024

typedef struct ficheiro{
    int pid;
    char operacao[BUFF];
    char nome[BUFF];
    char sha1sum[BUFF];
    char pipe_server_cli[BUFF];
    char pipe_cli_server[BUFF];
}*Fich;



void restore(Fich f){
    int  rd,wr,pipeToCliente,file;
    char destino_pipe[BUFF];
    char buffer[BUFF];
    char destino_metadata[BUFF];
    
    if(!fork()){
        mkfifo(f->pipe_cli_server,0666);
        pipeToCliente=open(f->pipe_server_cli,O_RDONLY);
        strcpy(destino_pipe,getDest());
        strcat(destino_pipe,"file.gz");
        strcpy(destino_metadata,getDestMeta());
        strcat(destino_metadata,f->nome);


        if (!fork()) {
            
            execlp("cp","cp" ,destino_metadata,destino_pipe, NULL);
            perror("Erro ao tentar calcular hash");
            _exit(1);
        }   
        wait(NULL);

        if (!fork()) {
            
            execlp("gunzip","gunzip" , destino_pipe, NULL);
            perror("Erro ao tentar calcular hash");
            _exit(1);
        }   
        wait(NULL);

        strcpy(destino_pipe,getDest());
        strcpy(destino_pipe,"file");

        file=open(destino_pipe,O_RDONLY);

        kill(f->pid, SIGUSR1);
        rd=1;wr=1;
        while(rd && wr){
            rd=read(file,buffer,BUFF);
            if(!rd) {wr=write(pipeToCliente,"acabou_o_ficheiro_volte_sempre\0",31); rd=0; wr=0;}
            else wr=write(pipeToCliente,buffer,rd);
        }
        strcpy(destino_pipe,getDest());
        strcat(destino_pipe,"file");
        unlink(destino_pipe);
        unlink(f->pipe_server_cli);
        close(file);
        kill(f->pid,SIGUSR1);
        _exit(0);
        
    }
    
}

void backup(Fich f){
    int  rd,wr,pipeFromCliente,file;
    char destino_pipe[BUFF];
    char destino_data[BUFF];
    char buffer[BUFF];
    char destino_metadata[BUFF];
    

    if(!fork()){
        mkfifo(f->pipe_cli_server,0666);
        kill(f->pid, SIGUSR1);
        pipeFromCliente=open(f->pipe_cli_server,O_RDONLY);
        strcpy(destino_pipe,getDest());
        rd=1;wr=1;
        

        /*Backup*/
        file=open(strcat(destino_pipe,f->nome), O_CREAT | O_WRONLY | O_RDONLY, 0666 );
        while(rd && wr){
            rd=read(pipeFromCliente,buffer,BUFF);
            if(strcmp(buffer,"acabou_o_ficheiro_volte_sempre\0")){rd=0; wr=0;}
            else wr=write(file,buffer,rd);
            
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


        unlink(destino_pipe);

        close(file);
        kill(f->pid,SIGUSR1);

       unlink(f->pipe_server_cli);
       unlink(f->pipe_cli_server);
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
