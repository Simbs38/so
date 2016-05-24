#define BUFF 1024

typedef struct ficheiro{
    int pid;
    char operacao[BUFF];
    char nome[BUFF];
    char sha1sum[BUFF];
    char pipe[BUFF];
}*Fich;


typedef struct lido{
    int rd;
    char buffer[BUFF];
    int fim;

}*Lido;

char *getDest();

char *getDestMeta();

char *getDestData();

char *getDestPipe();