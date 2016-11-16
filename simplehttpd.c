/********************************
CODED BY JOÃO OLIVEIRA          *
AND RUI"KOALA"GUSTAVO           *
                                *
 * -- simplehttpd.c --          *
 * A (very) simple HTTP server  *
 *                              *
 * Sistemas Operativos 2016/2017*
 ********************************/
#include "header.h"
#include <unistd.h>

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

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
  //reader_pipe(); //talvez precise de ser chamada noutro sitio IMPORTANTE VER
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

  /*NOT SURE ABOUT THIS WHILE 1*/
  //while(1){
    if(n_threads != teste->n_threads){
      numthreads=teste->n_threads;
      free(child_threads);
      child_threads = malloc((int)teste->n_threads*sizeof(pthread_t));
      for(i =0; i < numthreads; i++){
        if(pthread_create(&child_threads[i], NULL, temp_func,(void *)i) != 0){
          perror("Error at creating thread\n");
        }
      }
    }
    /*SCHEDULE DA QUEUE*/
   /* if(queue_aux > 0){ //quer dizer que existem elementos na queue
      if(teste->schedule_type == 2 && queue_aux > 1){ // PRIORIDADE ESTÁTICA
        organize_static();
      }
      else if(teste->schedule_type == 3 && queue_aux > 1){ //PRIORIDADE DINÂMICOS
        organize_dynamic();
      }
    }*/
  //}
}
void organize_static(){
  int i, n;
  request tmp;
  for (i = 1; i < queue_aux; i++){
    for(n=0; n < queue_aux-1;n++){
      if(queue[n].t_request > queue[n+1].t_request){
        tmp = queue[n];
        queue[n] = queue[n+1];
        queue[n+1] = tmp;
      }
    }
  }
}
void organize_dynamic(){
  int i, n;
  request tmp;
    for(i=1;i<queue_aux;i++){
      for(n=0;n<queue_aux-1;n++){
        if(queue[n].t_request<queue[n+1].t_request){
          tmp = queue[n];
          queue[n] = queue[n+1];
          queue[n+1]=tmp;
        }
      }
    }
}

void init(){
  //alocar espaço de memoria partilhada
  if((shmid = shmget(IPC_PRIVATE, sizeof(configs), IPC_CREAT|0777)) <0 ) {
    perror("Error at shmget\n");
  }

  // mapeia espaço de memoria para espaço de endereçamento do ficheiro de config
  if((teste = (configs*) shmat(shmid, NULL, 0)) < 0 ) {
    perror("Error at shmat\n");
  }

  /*le ficheiro */
  carregarConfig();

  /*criação da pool de threads */
  int *threads_id, i;
  pthread_t scheduler;

  if(pthread_create(&scheduler, NULL, masterthread, NULL) != 0){
    perror("Error at creating master thread\n");
  }

  //printf("NUmeros de thread:%d \n Tipo de coiso:%s \n Server-Porto:%d \n File1: %s \n File2:%s",teste->n_threads,teste->scheduling,teste->server_port,teste->file_list[0],teste->file_list[1]);
  //exit(0);
}

//TODO: aguardar que a  lista de pedidos seja tratada
void catch_ctrlc(int sig){
	printf("Server terminating\n");
  //testing cleanup
	int i;

/*
	for(i=0;i<teste->n_threads;i++){
    	pthread_cancel(child_threads[i]);
    	printf("A fechar a thread %d \n", i);
  	}
*/
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
void *temp_func(void *t){
	long my_id = (long) t;
  //while(1){
    printf("Hello, i'm a thread %ld\n",my_id);
    //reader_pipe();
    sleep(5);
  //}
  pthread_exit(NULL);
}

/*leitura do namedpipe*/
void reader_pipe(){
	char *search = " + \n";
    char aux[1024];
	char buf[MAX_BUF];
    int fd;
	char previous[50];
	char *token1;
    char *token2;
	while(1){
		
    	char * myfifo = "/tmp/myfifo2";
		fd = open(myfifo, O_RDONLY);
    	read(fd, buf, MAX_BUF);
    	//printf("sou eu o buffer:%s\n",buf);
    	strcpy(aux,buf);
    	//printf("Received: %s\n", aux);
    	if(strcmp(previous,buf)==0){
    	}
    	else{
    		token1 = strtok(aux,search);
    		//printf("sou o manel:%s",token1);
    		if(strcmp(token1,"1")==0){
    				token2 = strtok(NULL,search);
    				printf("sou o patricio:%s\n",token2);
			    	if(strcmp(token2,"1")==0){
			    		printf("Scheduling Normal\n");
			    		//teste->schedule_type = 1;
			    		strcpy(previous,buf);
			    	}
			    	else if(strcmp(token2,"2")==0){
			    		printf("Scheduling com prioridade aos pedidos estaticos\n");
			    		//teste->schedule_type = 2;
			    		strcpy(previous,buf);
			    	}
			    	else if(strcmp(token2,"3")==0){
			    		printf("Scheduling com prioridade aos pedidos dinamicos\n");
			    		//teste->schedule_type = 3;
			    		strcpy(previous,buf);
			    	}
		    }
		    else if(strcmp(token1,"2")==0){
    				token2 = strtok(NULL,search);
    				printf("sou o patricio:%s\n",token2);
			    	printf("Numero de threads novo:%s\n",token2);
			    	//teste->n_threads = atoi(token2);//SUBSTITUI O NUMERO THREADS ANTIGA
			    	strcpy(previous,buf);
		    }
		    else  if(strcmp(token1,"3")==0){
    				token2 = strtok(NULL,search);
    				printf("sou o patricio:%s\n",token2);
			    	printf("Novo ficheiro permitido: %s\n",token2);
			    	//strcpy(teste->file_list[j], token2); //LIMPA A LISTA DE FICHEIROS ANTIGA? ADICIONA À ANTIGA? SE SIM PRECISO DE SABER ONDE (PRECISO DE SABER O VALOR DE J)
			   		strcpy(previous,buf);
		    }
	    }
    	close(fd);
    }
    	
    return;
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
          if(fgets(buffer, sizeof buffer, fp) == NULL)
          	printf("EOF or IO error getting port\n");
            if (i == 0){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                teste -> server_port = atoi(token);
                //printf("%s",token);
            }
            else if (i == 1){
                token = strtok(buffer, search);
                token = strtok(NULL, search);
                teste -> schedule_type = atoi(token);
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
			printf("Posiçao do pedido na queue:%d",i);
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
