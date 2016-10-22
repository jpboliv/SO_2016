/********************************
CODED BY JOÃO OLIVEIRA          *
AND RUI"KOALA"GUSTAVO           *
                                *
 * -- simplehttpd.c --          *
 * A (very) simple HTTP server  *
 *                              *
 * Sistemas Operativos 2016/2017*
 ********************************/

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
void *temp_func(int my_id);
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
	char t_reception[20]; //hora de recepção
	char t_sent; // hora de envio
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

int main(int argc){
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);
	int port;
  init();
  queue = malloc(sizeof(request)*2*teste->n_threads);
	signal(SIGINT,catch_ctrlc);

  if(fork()==0){
    printf("Criar processos de gestor de configurações: \n");
    //gestorConfig();
    exit(0);
  }
  if(fork()==0){
    printf("Criar processos de gestor de estatísticas: \n");
    //gestorEstatistica();
    exit(0);
  }
	// Verify number of arguments
	/*if (argc!=2) {
		printf("Usage: %s <port>\n",argv[0]);
		exit(1);
	}*/
	//port=atoi(argv[1]);
  port=teste->server_port;
	printf("Listening for HTTP requests on port %d\n",port);

	// Configure listening port
	if ((socket_conn=fireup(port))==-1)
		exit(1);

	// Server requests
	while (1)
	{
		// Accept connection on socket
		if ( (new_conn = accept(socket_conn,(struct sockaddr *)&client_name,&client_name_len)) == -1 ) {
			printf("Error accepting connection\n");
			exit(1);
		}

		// Identify new client
		identify(new_conn);

		// Process request
		get_request(new_conn);

		// Verify if request is for a page or script
		if(!strncmp(req_buf,CGI_EXPR,strlen(CGI_EXPR)))
			execute_script(new_conn);
		else
			// Search file with html page and send to client
			send_page(new_conn);

		// Terminate connection with client
		close(new_conn);

	}
}

void *masterthread(){
  int n_threads=1;
  int numthreads, i;
  /*
  free(child_threads);
  child_threads = malloc((int)teste->n_threads*sizeof(pthread_t));*/
  /*for(i =0; i < n_threads; i++){
    if(pthread_create(&child_threads[i], NULL, temp_func, (void* )i) !=0) {
      printf("Error at pthread_create 1\n");
    }
  }*/
    if(n_threads != teste->n_threads){
      numthreads=teste->n_threads;
      free(child_threads);
      child_threads = malloc((int)teste->n_threads*sizeof(pthread_t));
      for(i =0; i < numthreads; i++){
        pthread_create(&child_threads[i], NULL, temp_func, (void* )i);
      }
      
    }
}

void init(){
  //alocar espaço de memoria partilhada
  shmid = shmget(IPC_PRIVATE, sizeof(configs), IPC_CREAT|0777);

  // mapeia espaço de memoria para espaço de endereçamento do ficheiro de config
  teste = (configs*) shmat(shmid, NULL, 0);

  /*le ficheiro */
  carregarConfig();

  /*criação da pool de threads */
  int *threads_id, i;
  pthread_t scheduler;

  pthread_create(&scheduler, NULL, masterthread, NULL);

  //printf("NUmeros de thread:%d \n Tipo de coiso:%s \n Server-Porto:%d \n File1: %s \n File2:%s",teste->n_threads,teste->scheduling,teste->server_port,teste->file_list[0],teste->file_list[1]);
  //exit(0);
}

//TODO: aguardar que a  lista de pedidos seja tratada
void catch_ctrlc(int sig){
	printf("Server terminating\n");
  //testing cleanup
	int i;
  

	for(i=0;i<teste->n_threads;i++){
    	pthread_cancel(child_threads[i]);
    	printf("A fechar a thread %d \n", i);
  	}

  for( i =0; i < teste->n_threads; i++){
        pthread_join(child_threads[i], NULL);
      }
  pthread_exit(&masterthread);
  pthread_exit(&child_threads);
  free(child_threads);

  	//limpar estatsticas
	shmdt(memShared);
	if(shmctl(shmid, IPC_RMID, NULL) < 0){
    printf("Error at shmctl\n");
  }
	close(socket_conn);
	exit(0);
}

/*função da worker thread*/
void *temp_func(int my_id){
  while(1){
    printf("Hello, i'm thread %d\n", my_id);
    //reader_pipe();
    sleep(5);
  }
  pthread_exit(NULL);
}
/*leitura do namedpipe*/
void reader_pipe(){
	  int fd;
    char * myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];

    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDONLY);
    read(fd, buf, MAX_BUF);
    printf("Received: %s\n", buf);
    close(fd);
}

/*CARREGAMENTO DO FICHEIRO CONFIG*/
void carregarConfig(){


	FILE *fp;
    char buffer[1024];
    char *search = " = ; \n";
    char *token;
    int j = 0;
 	teste = malloc(sizeof *teste);
    if((fp = fopen("config.txt", "r")) == NULL){
        perror("Erro a ler o ficheiro.\n");
    }
    else{
        int i=0;
        for(i = 0; i < 4; i++) {
            fgets(buffer, sizeof(buffer), fp);
            if (i == 0){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                teste -> server_port = atoi(token);
                //printf("%s",token);
            }
            else if (i == 1){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                strcpy(teste->scheduling, token);
                //printf("%s",token);

            }
            else if (i == 2){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                teste->n_threads = atoi(token);
                //printf("%s",token);
            }
            else if (i == 3){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                //printf("%s",token);
                while(token!=NULL) {
                    strcpy(teste->file_list[j], token);
                    //printf("%s",token);
                    j++;
                    token = strtok(NULL, search);
                }
            }
        }

        fclose(fp);

    }
}


// Processes request from client
void get_request(int socket)
{
	int i,j;
	int found_get;
	printf("DEBUG:Sou o socket:%d\n",socket);
	found_get=0;
	while ( read_line(socket,SIZE_BUF) > 0 ) {
		if(!strncmp(buf,GET_EXPR,strlen(GET_EXPR))) {
			// GET received, extract the requested page/script
			found_get=1;
			i=strlen(GET_EXPR);
			j=0;
			while( (buf[i]!=' ') && (buf[i]!='\0') )
				req_buf[j++]=buf[i++];
			req_buf[j]='\0';
		}
	}

	// Currently only supports GET
	if(!found_get) {
		printf("Request from client without a GET\n");
		exit(1);
	}
	printf("SOU EU A PAGINA:%s\n",req_buf);
	// If no particular page is requested then we consider htdocs/index.html
	if(!strlen(req_buf))
		sprintf(req_buf,"index.html");

	#if DEBUG
	printf("get_request: client requested the following page: %s\n",req_buf);
	#endif

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	for (i=0;i<2*teste->n_threads;i++){
		if(queue[i].t_request==0){
			if(!strncmp(req_buf,CGI_EXPR,strlen(CGI_EXPR))){
				queue[i].t_request=2;
			}
			else{
				queue[i].t_request = 1;
			}
			strcpy(queue[i].stat.t_reception,asctime (timeinfo));
			strcpy(queue[i].requested_file,req_buf);
			queue[i].socket=socket;
			printf("DEBUG:Printing queue:%s\n",queue[i].requested_file);
			printf("DEBUG:Printing file type:%d\n 1- Estatico 2- Dina \n",queue[i].t_request);
			printf("DEBUG:Printing hora de recepção do pedido %s\n",queue[i].stat.t_reception);
			return;
		}
	}
		return;
}


// Send message header (before html page) to client
void send_header(int socket)
{
	#if DEBUG
	printf("send_header: sending HTTP header to client\n");
	#endif
	sprintf(buf,HEADER_1);
	send(socket,buf,strlen(HEADER_1),0);
	sprintf(buf,SERVER_STRING);
	send(socket,buf,strlen(SERVER_STRING),0);
	sprintf(buf,HEADER_2);
	send(socket,buf,strlen(HEADER_2),0);

	return;
}


// Execute script in /cgi-bin
void execute_script(int socket)
{
	// Currently unsupported, return error code to client
	cannot_execute(socket);

	return;
}


// Send html page to client
void send_page(int socket)
{
	FILE * fp;

	// Searchs for page in directory htdocs
	sprintf(buf_tmp,"htdocs/%s",req_buf);

	#if DEBUG
	printf("send_page: searching for %s\n",buf_tmp);
	#endif

	// Verifies if file exists
	if((fp=fopen(buf_tmp,"rt"))==NULL) {
		// Page not found, send error to client
		printf("send_page: page %s not found, alerting client\n",buf_tmp);
		not_found(socket);
	}
	else {
		// Page found, send to client

		// First send HTTP header back to client
		send_header(socket);

		printf("send_page: sending page %s to client\n",buf_tmp);
		while(fgets(buf_tmp,SIZE_BUF,fp))
			send(socket,buf_tmp,strlen(buf_tmp),0);

		// Close file
		fclose(fp);
	}

	return;

}


// Identifies client (address and port) from socket
void identify(int socket)
{
	char ipstr[INET6_ADDRSTRLEN];
	socklen_t len;
	struct sockaddr_in *s;
	int port;
	struct sockaddr_storage addr;

	len = sizeof addr;
	getpeername(socket, (struct sockaddr*)&addr, &len);

	// Assuming only IPv4
	s = (struct sockaddr_in *)&addr;
	port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	printf("identify: received new request from %s port %d\n",ipstr,port);

	return;
}


// Reads a line (of at most 'n' bytes) from socket
int read_line(int socket,int n)
{
	int n_read;
	int not_eol;
	int ret;
	char new_char;

	n_read=0;
	not_eol=1;

	while (n_read<n && not_eol) {
		ret = read(socket,&new_char,sizeof(char));
		if (ret == -1) {
			printf("Error reading from socket (read_line)");
			return -1;
		}
		else if (ret == 0) {
			return 0;
		}
		else if (new_char=='\r') {
			not_eol = 0;
			// consumes next byte on buffer (LF)
			read(socket,&new_char,sizeof(char));
			continue;
		}
		else {
			buf[n_read]=new_char;
			n_read++;
		}
	}

	buf[n_read]='\0';
	#if DEBUG
	printf("read_line: new line read from client socket: %s\n",buf);
	#endif

	return n_read;
}


// Creates, prepares and returns new socket
int fireup(int port)
{
	int new_sock;
	struct sockaddr_in name;

	// Creates socket
	if ((new_sock = socket(PF_INET, SOCK_STREAM, 0))==-1) {
		printf("Error creating socket\n");
		return -1;
	}

	// Binds new socket to listening port
 	name.sin_family = AF_INET;
 	name.sin_port = htons(port);
 	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(new_sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
		printf("Error binding to socket\n");
		return -1;
	}

	// Starts listening on socket
 	if (listen(new_sock, 5) < 0) {
		printf("Error listening to socket\n");
		return -1;
	}

	return(new_sock);
}


// Sends a 404 not found status message to client (page not found)
void not_found(int socket)
{
 	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,SERVER_STRING);
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<HTML><TITLE>Not Found</TITLE>\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<BODY><P>Resource unavailable or nonexistent.\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"</BODY></HTML>\r\n");
	send(socket,buf, strlen(buf), 0);

	return;
}


// Send a 5000 internal server error (script not configured for execution)
void cannot_execute(int socket)
{
	sprintf(buf,"HTTP/1.0 500 Internal Server Error\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"\r\n");
	send(socket,buf, strlen(buf), 0);
	sprintf(buf,"<P>Error prohibited CGI execution.\r\n");
	send(socket,buf, strlen(buf), 0);

	return;
}


void retira_paragrafo(char *linha)
{
	unsigned int len = strlen(linha);
	if(linha[len-1]=='\n')
	{
		linha[len-1]='\0';
	}
}
/*

// Closes socket before closing
void catch_ctrlc(int sig)
{
	printf("Server terminating\n");
	close(socket_conn);
	exit(0);
}
*/
