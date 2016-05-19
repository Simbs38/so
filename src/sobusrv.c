#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define BUFFER_LENGTH 128

typedef struct estrutura{
    int   pipe;
    char *name_pipe;
    char *dest;
    char *dest_meta;
    char *dest_data;
    int   pipe_go;
    char *name_pipe_go;
    int   pipe_come;
    char *name_pipe_come;
    char *operacao;

}*Est;

int existe(char *s){
    return 1;
}

void pipe_done(){
    printf("Pipe Done\n");
}

void something_go_wrong(){
    printf("Erro\n");
}

void send_stuff_pls(){
    printf("Ready for info\n");
}


char *getDest(char *d){
    return strcat(strcat(getenv("HOME"),"/.BackUp"),d);
}

char *join(char *one, char *two){
    return strcat(one,two);
}


void backup(Est so){                        
    char *lines=strtok(so->operacao," ");
    int   data,metadata;
    char *meta_file;
    char *data_file;
    int   msg_size;
    char  buffer[BUFFER_LENGTH];




while(lines!=NULL){
    lines=strtok(NULL," ");
    if(lines!=NULL){
        lines=strtok(NULL,"-");
               
                /*cria o ficheiro lincavel*/
                meta_file=join(so->dest_meta,lines);
                metadata=open(meta_file,O_CREAT | O_WRONLY,0644);
                if(metadata<0) signal(SIGHUP,something_go_wrong);
                
                lines=strtok(NULL," ");
                data_file=join(so->dest_data,lines);
                
                /*ver s o cenas ja existe*/
                if(existe(lines)){   
                    data=open(data_file,O_CREAT | O_WRONLY,0644);        
                    if(data<0) signal(SIGHUP,something_go_wrong);
                    
                    /*manda um sinal po cliente a dizer para enviar o ficheiro*/
                    signal(SIGUSR1,send_stuff_pls);
                    
                    /*getfile*/
                    /*recebe o ficheiro*/
                    /*escreve num ficheiro*/
                    while((msg_size=read(so->pipe_come,buffer,BUFFER_LENGTH))) write(data,buffer,BUFFER_LENGTH);
                    /*comprime*/
                    execlp("gzip","gzip",data_file,NULL);
                    /*associa o ao seu link*/
                }


                if(!fork()){
                    execlp("ln","ln",data_file,meta_file,NULL);
                    perror("error");
                    _exit(1);
                }
            }
        }
    }

       /*se o digest ja existir, apagar o ficheiro e apenas usar o lincavel*/
    /*se o ficheiro ja existe apagar o antigo digest do ficheiro e escrever este de novo*/
/*envia sinal*/



void restore(Est so){
    /*se for restore*/
    /*enviar diretoria ao cliente*/
    /*esperar por sinal de chegada*/
    /*enviar o ficheiro ao cliente*/
    /*envia sinal*/
}






int main(){
    /*comando(1 char)<digest nome_do_ficheiro> */
    if(!fork()){
        
        /*Declaração de variaveis*/
        int msg_size;
        Est so =(Est) malloc(sizeof(struct estrutura));
        char operacao[BUFFER_LENGTH];
        char buffer[BUFFER_LENGTH];

        /*Inicialização da primeira parte da estrutura*/
        so->dest=getDest("");
        so->dest_data=getDest("/data");
        so->dest_meta=getDest("/metadata");
        mkfifo(so->dest, 0666);


        /*A espera de comandos da parte do cliente*/
        while(1){
            /*Le a operação*/
            so->pipe=open(so->dest,O_RDONLY);
            msg_size = read(so->pipe,buffer, BUFFER_LENGTH);
            if(msg_size==0){
                printf("stuff\n");
                if(!fork()){

                    
                    /*Inicialização do resto da estrutura*/
                    so->name_pipe_go=strcat(strcat(so->dest,buffer),"_go");
                    so->name_pipe_come=strcat(strcat(so->dest,buffer),"_come");
                    so->pipe_go=mkfifo(so->name_pipe_go,0666);
                    so->pipe_come=mkfifo(so->name_pipe_come,0666);
                    so->pipe_go=open(so->name_pipe_go,O_WRONLY);
                    so->pipe_come=open(so->name_pipe_come,O_RDONLY);

                    /*Enviar sinal ao cliente para o programa arrancar*/
                    signal(SIGINT,pipe_done);
                    msg_size = read(so->pipe_come,operacao, BUFFER_LENGTH);
                    if(msg_size!=0){

                        /*Chamada das funções para executar os comandos pedidos*/
                        strcpy(so->operacao,operacao+1);
                        if(operacao[0]=='b') backup(so);
                        else if (operacao[0]=='r') restore(so);
                    }
                }
                close(so->pipe);
            }            
        }
    }
    return 0;
}