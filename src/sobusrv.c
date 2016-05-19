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
    char path[BUFF];
    char operacao[BUFF];
    char nome[BUFF];
    char sha1sum[BUFF];
    char pipe_server_cli[BUFF];
    char pipe_cli_server[BUFF];
}*Fich;

int main() {
	int p,n=0,rd,wr,/*pipeToCliente,*/pipeFromCliente,file;
    char destino_pipe[BUFF];
    char buffer[BUFF];
    strcpy(destino_pipe,getDestPipe());
    mkfifo("/home/wani/.Backup/so_pipe", 0600);
	Fich f =malloc (sizeof (struct ficheiro));

    int x=fork();

    if(!x){
        while(1){
            p = open("/home/wani/.Backup/so_pipe", O_RDONLY);
            n=read(p,f,sizeof (struct ficheiro));
            if(n){
                if(!fork()){
                    mkfifo(f->pipe_cli_server,0666);
                    printf("%s\n",f->pipe_cli_server );
                    mkfifo(f->pipe_server_cli,0666);
                    printf("%s\n", f->pipe_server_cli);
                    kill(f->pid, SIGUSR1);
                    /*pipeToCliente=open(f->pipe_server_cli,O_WRONLY);*/
                    pipeFromCliente=open(f->pipe_cli_server,O_RDONLY);

                    rd=1;wr=1;
                    /*Backup*/
                    file=open(f->path, O_CREAT |O_WRONLY );
                    while(rd && wr){
                        rd=read(pipeFromCliente,buffer,BUFF);
                        wr=write(file,buffer,rd);
                    }
                    close(file);
                    kill(f->pid,SIGUSR1);

                   unlink(f->pipe_server_cli);
                   unlink(f->pipe_cli_server);
                   _exit(0);
                }
            }

            close(p);
        }
        _exit(0);
    }

    return 0;	
}
