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

#define BUFF 256

char *getDest(){
    char *source=malloc(sizeof BUFF);
    strcpy(source,(getenv("HOME")));
    strcat(source,"/.Backup/");
    return source;

}

char *getDestMeta(){
    char *source=malloc(sizeof BUFF);
    strcpy(source,getDest());
    strcat(source,"metadata/");
    return source;
}

char *getDestData(){
    char *source=malloc(sizeof BUFF);
    strcpy(source,getDest());
    strcat(source,"data/");
    return source;
}

char *getDestPipe(){
    char *source=malloc(sizeof BUFF);
    strcpy(source,getDest());
    strcat(source,"so_pipe");
    return source;
}