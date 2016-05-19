#include <unistd.h>
#include <fcntl.h>
#include "dest.h"
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>


#define BUFF 1024

typedef struct ficheiro{
    int pid;
    char path[BUFF];
    char operacao[BUFF];
    char nome[BUFF];
    char sha1sum[BUFF];
    char pipe_server_cli[BUFF];
    char pipe_cli_server[BUFF];
}*Fich;

void continua(){
    printf("Pipes criados, podes continuar\n");
}

void backup_done(){
    printf("Backup feito, podes continuar\n");
}


char *getsha1sum(char *file){
    int pfd[2];
    char  *ret=malloc (sizeof BUFF);
    pipe(pfd);
    if(!fork()){
        close(pfd[0]);
        dup2(pfd[1],1);
        execlp("sha1sum","sha1sum",file,NULL);
        perror("erro");
        _exit(1);
    }
    close(pfd[1]);
    dup2(pfd[0],0);
    read(pfd[0],ret,BUFF);
    return ret;


}

char* generate_hash(char *file_path) {
    char sha1sum[BUFF], *hash, *ret = NULL;
    int pp[2], status;

    pipe(pp);
    if (!fork()) {
        close(pp[0]);
        dup2(pp[1], 1);

        execlp("sha1sum", "sha1sum", file_path, NULL);
        close(pp[1]);
        perror("Erro ao tentar calcular hash");
        _exit(1);
    }

    wait(&status);
    status = WEXITSTATUS(status);
    if (status != 0) return ret;

    close(pp[1]);
    read(pp[0], sha1sum, BUFF);
    close(pp[0]);

    hash = strtok(sha1sum, " ");

    ret = malloc(strlen(hash) + 1);
    strcpy(ret, hash);
    return ret;
}


char *change_name(char *file){
    char buffer[BUFF];

    char *parte=strtok(file,"\\");
    strcpy(buffer,parte);

}




Fich getFicheiro(char* argv[],int i){
    char buf[BUFF];
    char *operacao =malloc (sizeof BUFF);
    char *nome =malloc (sizeof BUFF);
    Fich f=(Fich) malloc (sizeof(struct ficheiro));
    f->pid=getpid();
    strcpy(operacao,argv[1]);
    strcpy(nome,argv[i]);
    strcpy(f->operacao,operacao);
    strcpy(f->sha1sum,generate_hash(argv[i]));
    strcpy(f->nome,nome);
    sprintf(f->pipe_server_cli,"%spipe_server_cli_%d","/home/wani/.Backup/",getpid());
    sprintf(f->pipe_cli_server,"%spipe_cli_server_%d","/home/wani/.Backup/",getpid());
    strcpy(f->path,change_name(realpath(f->nome, buf)));
    return f;

}


int main(int argc, char *argv[]){
	int p,i,pipeToServidor,pipeFromServidor,file,rd,wr;
    char destino_pipe[BUFF];
    char buffer[BUFF];
    strcpy(destino_pipe,getDestPipe());
    Fich f=malloc(sizeof (struct ficheiro));
	p = open("/home/wani/.Backup/so_pipe", O_WRONLY);
    
    if(argc==1){
        printf("Por favor introduza uma opção\n");
    }
    else if(argc==2){
        printf("Por favor introduza um ficheiro\n");
    }
    else{
        if(argv[1][0]=='b'){
            for(i=2;i!=argc;i++){
            f=getFicheiro(argv,i);
            write(p,f, sizeof(struct ficheiro));
            

            signal(SIGUSR1, continua);
            pause();

            pipeToServidor=open(f->pipe_cli_server,O_WRONLY);
            pipeFromServidor=open(f->pipe_server_cli,O_RDONLY);
            file=open(f->nome,O_RDONLY);


            while(rd && wr){
                rd=read(file,buffer,BUFF);
                wr=write(pipeToServidor,buffer,rd);
            }

            signal(SIGUSR1,backup_done);
            pause();
            printf("Ficheiro: %s copiado!\n",f->nome );
            close(pipeToServidor);
            close(pipeFromServidor);
            close(file);

            }



        }
    }
        
            
    close(p);
    return 0;
}
