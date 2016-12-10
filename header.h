
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
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/msg.h>

// Produce debug information
#define DEBUG     1
#define PIPE_NAME "np_client_server"
#define FILEPATH "server.log"

// Header of HTTP reply to client
#define SERVER_STRING   "Server: simpleserver/0.1.0\r\n"
#define HEADER_1  "HTTP/1.0 200 OK\r\n"
#define HEADER_2  "Content-Type: text/html\r\n\r\n"

#define GET_EXPR  "GET /"
#define CGI_EXPR  "cgi-bin/"
#define SIZE_BUF  1024

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
void appendEstatisticas(char linha[]);
void catch_sighup_estat(int sig); 
void *workerThread(int n_pool);
void init();
void *reader_pipe();
void cleanup();
void *temp_func(void *t);
void carregarConfig();
void *masterthread();
void destroy_thread();
void create_threads();

void retira_paragrafo(char *linha);
void organize_static();
void organize_dynamic();
void organize_fifo();



char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int port,socket_conn,new_conn;
time_t rawtime;
struct tm * timeinfo;
time_t timeServer;
struct tm * timeServInfo;
sigset_t block_ctrlc;
char *ficheiro_mapeado;
int shmid;
pthread_t *child_threads;



/*semaforos*/
sem_t mutex, cond;
int queue_aux =0; //número de elementos no buffer
int flag=0;
int kill_pipe=0;
int kill_master=0;
int len_file_list = 0;


//estrutura para a pool de threads
typedef struct pool{
  int threads_id;
}thread;

// estrutura de config
typedef struct {
  int n_threads;
  int server_port;
  int schedule_type; //tipo de schedule - 1 default (FIFO) - 2 para estatico - 3 para dinamico
  char file_list[50][50]; //lista de ficheiros
}configs;
configs *teste;
int search_queue(configs *teste,char file[]);

//estrutura das estatisticas
typedef struct{
  char request_type[20];
  char file_name[50];
  int thread_ans; //thread que responde
  char t_reception[50]; //hora de recepção
  char t_sent[20]; // hora de envio
  int pedidosRecusados;
  int pedidosAceites;
  int n_pedidos_estaticos;
  int n_pedidos_dinamicos;
}statistic;
statistic *memShared;

//estrutura dos pedidos
typedef struct{
  short t_request; // tipo de pedido 1 ou 2
  char requested_file[SIZE_BUF];
  int socket;
  int id;
  statistic stat;
  
}request;

request *queue;

/*TODO: VER ISTO -> ESTRUTURA DO FREITAS*/
typedef struct{
    long    mtype;
    char    mtext[SIZE_BUF];
}msgbuf;

