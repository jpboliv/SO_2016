#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
void write_pipe(char *aux);
void menu();
int main(){
    menu();
    //strcpy(aux,"swag");
    //printf("%s\n",aux );
    return 0;
}

void menu(){
    char aux1[1024];
    char aux2[50];
    printf("\n\nConsola de Configuraçao Online\n");
    printf("0 - Sair\n");
    printf("1 - Alterar o tipo de scheduling\n");
    printf("2 - Alterar numero de thread\n");
    printf("3 - Alterar os ficheiros permitidos\n");
    printf("Opçao:");
    scanf("%s", aux1);
      if(strcmp(aux1,"1") == 0){
        printf("Indique o tipo de scheduling que deseja\n");
        printf("1 - Normal(Fifo) \t 2 - Prioridade aos Estaticos \t 3 - Prioridade dos dinamicos\n");
        printf("Opcao:");
        scanf("%s", aux2);
     }
     else if(strcmp(aux1,"2") == 0){
        printf("Indique o numero de threads da nova config:");
        scanf("%s",aux2);
     }
     else if(strcmp(aux1,"3") == 0){
        printf("Indique ficheiro o nome do ficheiro comprimid que pretende permitir:");
        scanf("%s",aux2);
     }
     else if (strcmp(aux1,"0") == 0){
        exit(0);
     }
    strcat(aux1, "+");
    strcat(aux1,aux2);
    write_pipe(aux1);
    menu();
}

void write_pipe(char *aux){
    int fd;
    char * myfifo = "/tmp/myfifo2";
    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);
    /* write "Hi" to the FIFO */
    fd = open(myfifo, O_WRONLY);
    write(fd, aux, sizeof(&aux));
    close(fd);
    /* remove the FIFO */
    unlink(myfifo);
}