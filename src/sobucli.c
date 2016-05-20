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
    char operacao[BUFF];
    char nome[BUFF];
    char sha1sum[BUFF];
    char pipe_server_cli[BUFF];
    char pipe_cli_server[BUFF];
}*Fich;

void help(){
    printf("\n");
    printf(" -----------------------------------------\n");
    printf(" | Bemvindo ao menu de ajuda do programa |\n");
    printf(" -----------------------------------------\n");
    printf(" | Carregar Ficheiros                    |\n");
    printf(" | sobucli backup <nome_ficheiro>        |\n");
    printf(" | ------------------------------------- |\n");
    printf(" | Restaurar Ficheiros                   |\n");
    printf(" | sobucli restore <nome_ficheiro>       |\n");
    printf(" | ------------------------------------- |\n");
    printf("\n");
}

void continua(){
    printf("Pipes criados, podes continuar\n");
}

void backup_done(){
    printf("Backup feito, podes continuar\n");
}


char* getsha1sum(char *file) {
    char sha[BUFF], *key, *r = NULL;
    int p[2], status;

    pipe(p);
    if (!fork()) {
        close(p[0]);
        dup2(p[1], 1);

        execlp("sha1sum", "sha1sum", file, NULL);
        close(p[1]);
        perror("Erro ao tentar calcular hash");
        _exit(1);
    }

    wait(&status);
    status = WEXITSTATUS(status);
    if (status == 0){

    close(p[1]);
    read(p[0], sha, BUFF);
    close(p[0]);

    key = strtok(sha, " ");

    r = malloc(strlen(key) + 1);
    strcpy(r, key);
    }
    return r;
}



Fich getFicheiroRst(char* argv[],int i){
    Fich f=(Fich) malloc (sizeof(struct ficheiro));
    f->pid=getpid();
    strcpy(f->operacao,argv[1]);
    strcpy(f->nome,argv[i]);
    sprintf(f->pipe_server_cli,"%spipe_server_cli_%d",getDest(),getpid());
    return f;
}




Fich getFicheiroBck(char* argv[],int i){
    Fich f=(Fich) malloc (sizeof(struct ficheiro));
    f->pid=getpid();
    strcpy(f->operacao,argv[1]);
    strcpy(f->sha1sum,getsha1sum(argv[i]));
    strcpy(f->nome,argv[i]);
    sprintf(f->pipe_cli_server,"%spipe_cli_server_%d",getDest(),getpid());
    return f;
}


void backup(int argc, char *argv[],Fich f,int p){
    int i,pipeToServidor,file,rd,wr;
    char destino_pipe[BUFF];
    char buffer[BUFF];
    strcpy(destino_pipe,getDestPipe());

    for(i=2;i!=argc;i++){
    f=getFicheiroBck(argv,i);
    write(p,f, sizeof(struct ficheiro));
    
    signal(SIGUSR1, continua);
    pause();

    pipeToServidor=open(f->pipe_cli_server,O_WRONLY);
    file=open(f->nome,O_RDONLY);
    rd=1; wr=1;

    while(rd && wr){
        rd=read(file,buffer,BUFF);
            if(!rd) wr=write(pipeToServidor,"acabou_o_ficheiro_volte_sempre\0",31);
            else{wr=write(pipeToServidor,buffer,rd);}
            }
    }
    signal(SIGUSR1,backup_done);
    pause();
    printf("%s: copiado\n",f->nome );
    close(pipeToServidor);
    close(file);

}

void restore(int argc, char *argv[],Fich f,int p){
    int i,pipeFromServidor,file,rd,wr;
    char destino_pipe[BUFF];
    char buffer[BUFF];
    strcpy(destino_pipe,getDestPipe());
    
    for(i=2;i!=argc;i++){
        f=getFicheiroRst(argv,i);
        write(p,f,sizeof(struct ficheiro));

        signal(SIGUSR1, continua);
        pause();
        printf("Pipe: %s\n",f->pipe_server_cli );
        pipeFromServidor=open(f->pipe_server_cli,O_RDONLY);
        file=open("d.txt",O_WRONLY | O_RDONLY | O_CREAT, 0666);
        rd=1;wr=1;
        while(rd && wr){
            rd=read(pipeFromServidor,buffer,BUFF);
            if(strcmp(buffer,"acabou_o_ficheiro_volte_sempre\0")){rd=0; wr=0;}
            else wr=write(file,buffer,BUFF);
        }

        printf("%s: restaurado\n","d.txt" );

    }

}


int main(int argc, char *argv[]){
    int p;
    Fich f=malloc(sizeof (struct ficheiro));
	p = open(getDestPipe(), O_WRONLY);
    
    if(argc==1){
        printf("Por favor introduza uma opção\n");
    }
    else if(argc==2){
        if(strcmp(argv[1],"-h") || strcmp(argv[1],"-help"))help();
        else{
        printf("Por favor introduza um ficheiro\n");
        }
    }
    else{
        if(argv[1][0]=='b') backup(argc,argv,f,p);
        else if(argv[1][0]=='r') restore(argc,argv,f,p);

    }
        
            
    close(p);
    return 0;
}
