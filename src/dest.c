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



char *getDest(){
    char *source=getenv("HOME");
    strcat(source,"/.Backup");

    return source;

}

char *getDestMeta(){
    char *source=getDest();
    strcat(source,"/metadata");
    return source;
}

char *getDestData(){
    char *source=getDest();
    strcat(source,"/data");
    return source;
}

char *getDestPipe(){
    char*source=getDest();
    strcat(source,"/so_pipe");
    return source;
}