
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/fcntl.h>
#include <semaphore.h>

// Produce debug information
#define DEBUG	  	1

// Header of HTTP reply to client
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024

#define MAX_BUF 1024

int  fireup(int port);
void identify(int socket);
void get_request(int socket);
int  read_line(int socket, int n);
void send_header(int socket);
void send_page(int socket);
void execute_script(int socket);
void not_found(int socket);
void catch_ctrlc(int);
void cannot_execute(int socket);
void retira_paragrafo(char *linha);

void init();
void reader_pipe();
void cleanup();
void *temp_func(void *t);
void carregarConfig();
void *masterthread();

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int port,socket_conn,new_conn;
time_t rawtime;
struct tm * timeinfo;
time_t timeServer;
struct tm * timeServInfo;

int shmid;
pthread_t *child_threads;

//estrutura para a pool de threads
typedef struct pool{
  int threads_id;
}thread;

// estrutura de config
typedef struct {
  int n_threads;
  int server_port;
  char scheduling[20]; //tipo de schedule - estatico ou comprimido
  char file_list[50][50]; //lista de ficheiros
}configs;
configs *teste;

//estrutura das estatisticas
typedef struct{
	char request_type[20];
	char file_name[50];
	int thread_ans; //thread que responde
	char t_reception[50]; //hora de recepção
	char t_sent[20]; // hora de envio
}statistic;
statistic *memShared;

//estrutura dos pedidos
typedef struct{
	short t_request; // tipo de pedido 1 ou 2
	char requested_file[SIZE_BUF];
	int socket;
	statistic stat;
}request;

request *queue;
