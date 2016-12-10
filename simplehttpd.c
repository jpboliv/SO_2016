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

    int main(int argc, char ** argv){
      struct sockaddr_in client_name;
      socklen_t client_name_len = sizeof(client_name);
      int port;
      init();
      queue = malloc(sizeof(request)*2*teste->n_threads);
      signal(SIGINT,catch_ctrlc);


      /*SEMÁFOROS*/

      sem_unlink("BUFFER_MUTEX");
      buffer_mutex = sem_open("BUFFER_MUTEX", O_CREAT|O_EXCL, 0700, 1);

      sem_unlink("EMPTY");
      empty = sem_open("EMPTY", O_CREAT|O_EXCL, 0700, teste->n_threads);

      sem_unlink("FULL");
      full = sem_open("FULL", O_CREAT|O_EXCL, 0700, 0);

      if(fork()==0){
        printf("Criar processos de gestor de configurações: \n");
        //gestorConfig();
        exit(0);
      }

      if(fork()==0){

              int msgflg = IPC_CREAT | 0666;
              int msqid;
              key_t key;
              msgbuf tmp;
              strcpy(tmp.mtext,"");
              key = 1234;
              if ((msqid = msgget(key, msgflg)) < 0){

              } 
              printf("%s-%d\n", "Criar processos de gestor de estatísticas: " ,getpid());

              signal(SIGUSR1, catch_sigusr1);
              signal(SIGUSR2, catch_sigusr2);

              while(1){
               //Receive an answer of message type 1.
                  if (msgrcv(msqid, &tmp, SIZE_BUF, 1, 0) < 0)
                  {

                  }

                  if(strcmp(tmp.mtext,"")==0)
                  {

                  }
                  else
                  {
                    appendEstatisticas(tmp.mtext);
                    strcpy(tmp.mtext,"");
                  }
              }
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
      int optval = 1;
      if(setsockopt(socket_conn, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
       exit(1); 
   }
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

        /*
        // Verify if request is for a page or script
        if(!strncmp(req_buf,CGI_EXPR,strlen(CGI_EXPR)))
          execute_script(new_conn);
        else
          // Search file with html page and send to client
          //send_page(new_conn);
      */
        // Terminate connection with client
        //close(new_conn);
        sem_post(full);        
      }
    }

    void init(){
      //alocar espaço de memoria partilhada
      if((shmid = shmget(IPC_PRIVATE, sizeof(statistic*), IPC_CREAT|0777)) < 0 ) {
        perror("Error at shmget\n");
      }

      // mapeia espaço de memoria para espaço de endereçamento do ficheiro de config
      if((memShared = (statistic*) shmat(shmid, NULL, 0)) < 0 ) {
        perror("Error at shmat\n");
      }

      memShared->pedidosAceites=0;
<<<<<<< HEAD
    memShared->pedidosRecusados=0;
      /*le ficheiro */
      //sem_wait(mutex);
      carregarConfig();
      //sem_post(mutex);
=======
      memShared->pedidosRecusados=0;
      /*le ficheiro */
      sem_wait(mutex);
      carregarConfig();
      sem_post(mutex);
>>>>>>> origin/master
      /*criação da pool de threads */
      pthread_t scheduler;
      pthread_t pipe;
      //sem_init(&mutex, 0, 1);
      //sem_init(&cond, 0, 0);

      sem_unlink("MUTEX");
      mutex = sem_open("MUTEX", O_CREAT|O_EXCL, 0700, 1);
      
      if(pthread_create(&scheduler, NULL, masterthread, NULL) != 0){
        perror("Error at creating master thread\n");
      }
      if(pthread_create(&pipe, NULL, reader_pipe, NULL) != 0){
        perror("Error at creating master thread\n");
      }
      //printf("NUmeros de thread:%d \n Tipo de coiso:%s \n Server-Porto:%d \n File1: %s \n File2:%s",teste->n_threads,teste->scheduling,teste->server_port,teste->file_list[0],teste->file_list[1]);
      //exit(0);
    }

    void destroy_thread(){
      int i;
      flag=1;
      kill_master=1;
      for( i =0; i < teste->n_threads; i++){
        pthread_join(child_threads[i], NULL);
      }
           
    }
void create_threads(){
flag = 0;
kill_master=0;
      pthread_t scheduler;
     // pthread_cancel(masterthread);
   if(pthread_create(&scheduler, NULL, masterthread, NULL) != 0){
            perror("Error at creating master thread\n");
      }
}
    //TODO: aguardar que a  lista de pedidos seja tratada
    void catch_ctrlc(int sig){

      printf("Server terminating\n");
      printf("Pedidos aceites %d" ,memShared->pedidosAceites);
      //testing cleanup
      int i;
      kill_master=1;
      kill_pipe=1;
      flag=1;
      
      close(socket_conn);
<<<<<<< HEAD
      /*for( i =0; i < teste->n_threads; i++){
=======
      /*
      for( i =0; i < teste->n_threads; i++){
>>>>>>> origin/master
        pthread_join(child_threads[i], NULL);
      }*/
      for(i=0;i<teste->n_threads;i++){
          pthread_cancel(child_threads[i]);
          printf("A fechar a thread %d \n", i);
        }
    
      //pthread_cancel(masterthread);  
      //pthread_exit(&masterthread);
      //pthread_exit(&child_threads);
      free(child_threads);
      free(queue);
      
        //limpar estatsticas
      shmdt(memShared);
      if(shmctl(shmid, IPC_RMID, NULL) < 0){
        perror("Error at shmctl\n");
      }
      
      exit(0);
    }

    void *masterthread(void* arg){
      //int n_threads=1;
      //int numthreads;
      int i;
      free(child_threads);
      sem_wait(mutex);
      child_threads = malloc((int)teste->n_threads*sizeof(pthread_t));
      sem_post(mutex);
      for(i =0; i <teste->n_threads; i++){
        if(pthread_create(&child_threads[i], NULL, workerThread, (void *)i )!=0) {
          printf("Error at pthread_create 1\n");
        }
      }
     while(1){
      if(kill_master==1){
        pthread_exit(NULL);
        return(NULL);
      }
        /*if(numthreads != teste->n_threads){
          numthreads=teste->n_threads;
          free(child_threads);
          child_threads = malloc((int)teste->n_threads*sizeof(pthread_t));
          for(i =0; i < numthreads; i++){
            if(pthread_create(&child_threads[i], NULL,workerThread, (void *)i )!= 0){
              perror("Error at creating thread\n");
            }
          }
        }*/


        //SCHEDULE DA queue
        if(queue_aux > 0){ //quer dizer que existem elementos na queue
          if(teste->schedule_type == 2 && queue_aux > 1){ // PRIORIDADE ESTÁTICA
            organize_static();
          }
          else if(teste->schedule_type == 3 && queue_aux > 1){ //PRIORIDADE DINÂMICOS
            organize_dynamic();
          }
        }
        else if(teste->schedule_type == 1 && queue_aux > 1){ 
          organize_fifo();
        }
      }
    }


void *reader_pipe(void* arg){
  
    // Creates the named pipe if it doesn't exist yet
  if ((mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
      perror("Cannot create pipe: ");
      exit(0);
    }
  int fd;
  if ((fd = open(PIPE_NAME, O_RDONLY)) < 0) {
  perror("Cannot open pipe for reading: ");
  exit(0);
  }

//criaçao de variveis aux
    char *search = " + \n";
        
        //fim de variaveis aux

     while(1){
      if(kill_pipe==1){
        pthread_exit(NULL);
        return(NULL);
      }
        //READER FROM PIPE
          // Opens the pipe for reading
  
        char previous[100];
        char *token1;
        char *token2;
        
        char aux[1024];
        char buf[MAX_BUF];
        read(fd, &buf, sizeof(buf));
        //fgets(buf,MAX_BUF, fd);
        strcpy(aux,buf);

          if(strcmp(previous,buf)==0){
          }
          else{
            token1 = strtok(aux,search);

            if(strcmp(token1,"1")==0){
                token2 = strtok(NULL,search);
                printf("sou o patricio:%s\n",token2);
                if(strcmp(token2,"1")==0){
                  printf("Scheduling Normal\n");
                  teste->schedule_type = 1;// TODO - N TEMOS A FUNÇAO PARA ORGANIZAR PEDIDOS DO TIPO 1
                  strcpy(previous,buf);
                }
                else if(strcmp(token2,"2")==0){
                  printf("Scheduling com prioridade aos pedidos estaticos\n");
                  teste->schedule_type = 2;
                  strcpy(previous,buf);
                }
                else if(strcmp(token2,"3")==0){
                  printf("Scheduling com prioridade aos pedidos dinamicos\n");
                  teste->schedule_type = 3;
                  strcpy(previous,buf);
                }
            }
            else if(strcmp(token1,"2")==0){
              if(queue_aux==0){
                token2 = strtok(NULL,search);
                //printf("sou o patricio:%s\n",token2);
                printf("Numero de threads novo:%s\n",token2);

                  int jota = atoi(token2);
                if(teste->n_threads == jota){
                  printf("A pool ja tem %d threads\n",jota );
                  strcpy(previous,buf);
                }
                else{
                  destroy_thread();
                  teste->n_threads = jota;
                  create_threads();//SUBSTITUI O NUMERO THREADS ANTIGA
                  strcpy(previous,buf);
                }
              }else{printf("i'm busy dumbass");}
            }
            else  if(strcmp(token1,"3")==0){
                token2 = strtok(NULL,search);
                //printf("sou o patricio:%s\n",token2);
                printf("Novo ficheiro permitido: %s\n",token2);
                strcpy(teste->file_list[len_file_list], token2); //LIMPA A LISTA DE FICHEIROS ANTIGA? ADICIONA À ANTIGA? SE SIM PRECISO DE SABER ONDE (PRECISO DE SABER O VALOR DE J)
                len_file_list++;
                int k;
                printf("listagem dos files:");
                for(k=0;k<len_file_list;k++){
                  printf("%s\n",teste->file_list[k] );
                }
                strcpy(previous,buf);
            }
          }
          
          //END OF READER OF PIPE

      }
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

    void organize_fifo(){
      int i, n;
      request tmp;
      for(i=1; i<queue_aux;i++){
        for(n=0;n<queue_aux-1;n++){
          if(queue[n].id > queue[n+1].id){
            tmp = queue[n+1];
            queue[n+1] = queue[n];
            queue[n]=tmp;
          }
        }
      }
    }

  int search_queue(configs *teste,char file[]){
    int i;
    for(i = 0; i < queue_aux; i++){
      if(strcmp(teste->file_list[i],file)==0){
        return 1;
      }
      else {
        return 0;
      }
    }
    return -1;
  }
  void *workerThread(int n_pool)
  {
    int msqid;
    key_t key;
    int n;
    msgbuf tmp;
    //char num[10];
    key = 1234;
    printf("estou a ser criada%d\n",n_pool);
    sigset_t block_ctrlc;
    //sigemptyset (&block_ctrlc); 
    //sigaddset (&block_ctrlc, SIGINT); 
    while(1)
    {
      sem_wait(full);
      sem_wait(buffer_mutex);

      if(flag==1){
        pthread_exit(NULL);
        return(NULL);
      }
      if(queue_aux>0){
      sigprocmask (SIG_BLOCK, &block_ctrlc, NULL);
/*
      sem_wait(&cond);
      sem_wait(&mutex);
*/
      n=queue_aux-1;

      if(strstr(queue[n].requested_file,".gz"))
        {
          if(search_queue(teste,queue[n].requested_file)==1)
          {
            execute_script(queue[n].socket);
            memShared->pedidosAceites++;
            printf("Executou o ficheiro comprimido!\n");
            memShared->n_pedidos_dinamicos++;
            end_t = clock();
            total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
            memShared->time_pedidos_dinamicos = memShared->time_pedidos_dinamicos + (double)total_t;
          }

          else
          {
            cannot_execute((int)socket);
            memShared->pedidosRecusados++;
            printf("Não executou\n");
          }
        }
        else
        {
          send_page(queue[n].socket);
          memShared->pedidosAceites++;
          memShared->n_pedidos_estaticos++;
          end_t = clock();
          total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
          memShared->time_pedidos_estaticos = memShared->time_pedidos_estaticos + (double)total_t;
        }
        
        if(queue[n].t_request==1){
          strcpy(queue[n].stat.request_type,"estatico");
          printf("%s\n", queue[n].stat.request_type);
        }
        else if(queue[n].t_request==2){
          strcpy(queue[n].stat.request_type,"dinamico");
          printf("%s\n", queue[n].stat.request_type);
        }
        queue[n].stat.thread_ans=n_pool;
        //strcpy(queue[n].stat.request_type,queue[n].requested_file);
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strcpy(queue[n].stat.t_sent,asctime (timeinfo));

        if ((msqid = msgget(key, 0666 )) < 0)   //Get the message queue ID for the given key

          //Message Type
          strcpy(tmp.mtext,"");
          strcat(tmp.mtext,queue[n].stat.request_type);
          strcat(tmp.mtext,",");
          strcat(tmp.mtext,queue[n].stat.file_name);
          strcat(tmp.mtext,",");
          retira_paragrafo(queue[n].stat.t_reception);
          strcat(tmp.mtext,queue[n].stat.t_reception);
          strcat(tmp.mtext,",");
          retira_paragrafo(queue[n].stat.t_sent);
          strcat(tmp.mtext,queue[n].stat.t_sent);
          strcat(tmp.mtext,";");
          tmp.mtype=1;
          if (msgsnd(msqid, &tmp, SIZE_BUF, IPC_NOWAIT) < 0)
          {
              printf ("Não foi enviado\n");
          }

          else
              printf("Message Sent\n");

          queue[n].socket=0;
          queue[n].t_request=0;
          strcpy(queue[n].requested_file,"");
          queue_aux--;
          
          sem_post(buffer_mutex);
          sem_post(empty);

          close(new_conn);
          //sem_post(&mutex);
          
          sigprocmask (SIG_UNBLOCK, &block_ctrlc, NULL);
      #if DEBUG
        printf("->queue_aux: %d\n",queue_aux);
      #endif
        }
        else{}
          
    }

  }


    /*CARREGAMENTO DO FICHEIRO CONFIG*/
    void carregarConfig(){


      FILE *fp;
        char buffer[1024];
        char *search = " = ; \n";
        char *token;
        
        int j = 0;
      teste = malloc(sizeof (configs));
<<<<<<< HEAD
      //sem_wait(mutex);
=======
      sem_wait(mutex);
>>>>>>> origin/master
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
                      
                      teste->schedule_type = 1;

                    //printf("teste->schedule_type: %d\n",teste->schedule_type);

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
                        len_file_list = j;
                        token = strtok(NULL, search);
                    }
                }
            }

            fclose(fp);
<<<<<<< HEAD
           // sem_post(mutex);
=======
            sem_post(mutex);
>>>>>>> origin/master
        }
    }


    // Processes request from client
    void get_request(int socket)
    {
      int i,j;
      int found_get;
      printf("DEBUG:Sou o socket:%d\n",socket);
      found_get=0;
      queue_aux++;
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
          if(strstr(req_buf,".gz")){
            queue[i].t_request=2;
          }
          else{
            queue[i].t_request = 1;
          }
          strcpy(queue[i].stat.t_reception,asctime (timeinfo));
          strcpy(queue[i].requested_file,req_buf);
          strcpy(queue[i].stat.file_name,req_buf);
          queue[i].socket=socket;
          start_t = clock();
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

void execute_script(int socket) {
  FILE *in;
  char buf[512];
  //char * cmd; cmd = "";
  sprintf(buf_tmp,"zcat htdocs/%s",req_buf);


  if((in = popen(buf_tmp, "r")) == NULL) {
    not_found(socket);
  } else {
    send_header(socket);
    send(socket, "<html><body>", strlen("<html><body>"),0);
    while (fgets(buf, sizeof(buf) - 1, in) != NULL) {
      send(socket, "<p>", strlen("<p>"),0);
      send(socket, buf, strlen(buf), 0);
      send(socket, "</p>", strlen("</p>"),0);
    }
    send(socket, "</body></html>", strlen("</body></html>"),0);
  }
  pclose(in);
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
        while(fgets(buf_tmp,SIZE_BUF,fp)){

          send(socket,buf_tmp,strlen(buf_tmp),0);

        }

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
 
  void appendEstatisticas(char linha[])
  {
    FILE *f;
    struct stat sb;
    int fd;
    char barra_n[2]={'\n','\0'};
    if((f=fopen("server.log","a"))!=NULL)
    {
      fputs(linha,f);
      fputs(barra_n,f);
    }
    fclose(f);

    fd = open ("server.log", O_RDONLY);
      
   if(fd == -1)
      return ;

   if(fstat(fd, &sb) == -1)
      return ;
      
    ficheiro_mapeado = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if(ficheiro_mapeado == MAP_FAILED) {
        perror ("mmap");
        return ;
    }
    if(close(fd)==-1) {
        perror ("close");
        return ;
    }
  }
  
  void catch_sigusr1(int sig){

      if((memShared->n_pedidos_estaticos == 0) && (memShared->n_pedidos_dinamicos == 0)){
        printf("Nao houve pedidos ainda\n");
      }else{
        printf("Numero de pedidos estaticos servidos:%d\n",memShared->n_pedidos_estaticos );
      printf("Numero de pedidos comprimidos servidos:%d\n",memShared->n_pedidos_dinamicos);
      printf("Tempo medio para servir um pedido a conteúdo estático não comprimido:%f segundos \n",(double)(memShared->time_pedidos_estaticos/(double)memShared->n_pedidos_estaticos + 0));
      printf("Tempo médio para servir um pedido a conteúdo estático comprimido:%f segundos \n", (double)(memShared->time_pedidos_dinamicos/(double)memShared->n_pedidos_dinamicos + 0)); 
      }
         
    }

    void catch_sigusr2(int sig){
       memShared->n_pedidos_estaticos = 0;
       memShared->n_pedidos_dinamicos = 0 ;
       memShared->n_pedidos_estaticos = 0;
       memShared->time_pedidos_dinamicos = 0;
       printf("Reset à informação estatística agregada \n");
    }


  //sinal que imprime as estatisticas
  void catch_sighup_estat(int sig)
  {
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    //printf("Tempo Inicial do servidor: %s\n", horaServidor);
    printf("Hora actual do servidor: %s\n", asctime(timeinfo));
    //printf("Pedidos Recusados: %d\n",memShared->pedidosRecusados);
    printf("Pedidos Aceites: %d\n", memShared->pedidosAceites);
  }