#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define PIPE_NAME "np_client_server"
void write_pipe(char *aux);
void menu();
int fd;
int main(){
    char * myfifo = "/tmp/myfifo2";
    /* write "Hi" to the FIFO */

    
    if ((fd = open(PIPE_NAME, O_WRONLY)) < 0) {
    perror("Cannot open pipe for writing: ");
    exit(0);
}
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
    strcat(aux1,"+");
    printf("%s\n",aux1 );
    write(fd, aux1, sizeof(aux1));
    menu();
}
