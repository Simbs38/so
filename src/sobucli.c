#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    int ret=0;
    if(argc==0){
        printf("Por favor introduza uma operação\n");
        ret=-1;
    }
    else if(argc==1){
        printf("Por favor introduza um ficheiro\n");
        ret=-1;
    }
    else {
        char buf[10];
        int r =10;
        int i=2;
        int file;

        if(argv[1][0]== 'r'){

            while(i!=argc){    
                file= open(argv[i], O_RDONLY, 0600);
                while(r==10){
                    r= read(file,buf,10);

                    /*mandar o ret po pipe....*/
                    /*enviar um sinal po servidor com o numero do meu processo*/

                    /*se for backup*/
                        /*enviar o nome po servidor*/
                        /*espero por sinal*/
                        /*envio o resto do ficheiro*/
                        /*espero pelo sinal*/
                        /*imprimo cenas*/

                    /*se for restore*/
                        /*enviar nome*/
                        /*espera pelo nome e sinal*/
                        /*cria um ficheiro na diretoria onde estava, se nao for possivel cria na diretoria local*/
                        /*manda um sinal ao servidor*/
                        /*espera pelo resto do ficheiro*/
                        /*escreve o ficheiro todo*/
                        /*espera pelo sinal*/
                        /*termina*/


                }
            }
        }

    
        else if(argv[1][0]=='b'){
            /*mandar cenas po server e esperar que ele faça cenas para dar revive ao ficheiro
            */
        }
        else{
            printf("Comando invalido\n");
            ret=-1;
        }
    }
    return ret;
}