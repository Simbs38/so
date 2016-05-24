#include <unistd.h>
#include <fcntl.h>
#include "dest.h"
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>


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

void continua(){}


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




Fich getFicheiroBack(char* argv[],int i){
    Fich f=(Fich) malloc (sizeof(struct ficheiro));
    f->pid=getpid();
    strcpy(f->operacao,argv[1]);
    strcpy(f->sha1sum,getsha1sum(argv[i]));
    strcpy(f->nome,argv[i]);
    sprintf(f->pipe,"%spipe_%d",getDest(),getpid());
    return f;
}

Fich getFicheiroRest(char* argv[],int i){
    Fich f=(Fich) malloc (sizeof(struct ficheiro));
    f->pid=getpid();
    strcpy(f->operacao,argv[1]);
    strcpy(f->nome,argv[i]);
    sprintf(f->pipe,"%spipe_%d",getDest(),getpid());
    return f;
}



void backup(int argc, char *argv[],Fich f,int p){
    int i,pipeToServidor,file,rd,wr;
    char destino_pipe[BUFF];
    strcpy(destino_pipe,getDestPipe());

    for(i=2;i!=argc;i++){
    f=getFicheiroBack(argv,i);
    write(p,f, sizeof(struct ficheiro));
    
    signal(SIGUSR1, continua);
    pause();

    pipeToServidor=open(f->pipe,O_WRONLY);
    file=open(f->nome,O_RDONLY);
    rd=1; wr=1;


    Lido estrutura=(Lido) malloc (sizeof(struct lido));
    estrutura->fim=1;    
    do{
        estrutura->rd=read(file,estrutura->buffer,BUFF);
        if(!estrutura->rd){ estrutura->fim=0;rd=0; wr=0;}
        wr=write(pipeToServidor,estrutura,(sizeof(struct lido)));
    }while(rd && wr);
    
    }
    signal(SIGUSR1,continua);
    pause();
    printf("%s: copiado\n",f->nome );
    close(pipeToServidor);
    close(file);

}

void restore(int argc, char *argv[],Fich f,int p){
    int i,pipeFromServidor,file,rd,wr;
    char destino_pipe[BUFF];
    strcpy(destino_pipe,getDestPipe());
    
    for(i=2;i!=argc;i++){
        f=getFicheiroRest(argv,i);
        write(p,f,sizeof(struct ficheiro));

        signal(SIGUSR1, continua);
        pause();
        pipeFromServidor=open(f->pipe,O_RDONLY);
        file=open(f->nome,O_WRONLY | O_CREAT, 0666);
        rd=1;wr=1;
        Lido estrutura=(Lido) malloc (sizeof(struct lido));
        while(rd && wr){
            rd=read(pipeFromServidor,estrutura,(sizeof(struct lido)));
            if(estrutura->fim) wr=write(file,estrutura->buffer,estrutura->rd);
            else{rd=0; wr=0;}
            
        }
        close(file);
        printf("%s: restaurado\n",f->nome );

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
