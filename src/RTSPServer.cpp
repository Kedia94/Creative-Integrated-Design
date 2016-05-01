#include "RTSPServer.h"

RTSPServer::RTSPServer(struct sockaddr_in server_addr, int listenfd,  int port){
  _server_addr = server_addr;
  _listenfd = listenfd;
  _port = port;
  Createurl();
}

RTSPServer::~RTSPServer(void){
  close(_listenfd);
}

RTSPServer* RTSPServer::Create(int port){
  int listenfd, clnt_sock;
  socklen_t addr_size;
  struct sockaddr_in server_addr;
  struct sockaddr_in clnt_addr;

  listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
  if (listenfd < 0){
    printf("socket() error\n");
    return NULL;
  }

  int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
             (const void *)&optval , sizeof(int));

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons((unsigned short)port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    printf("bind() error\n");
    return NULL; 
  }
  if (listen(listenfd, 5) == -1){
    printf("listen() error\n");
    return NULL; 
  }
  
  return new RTSPServer(server_addr, listenfd, port);
}

void RTSPServer::Accept(void){
  sockaddr_in sa_cli;
  socklen_t cli_addr_len = sizeof(sa_cli);
  
  int connfd;
  pthread_t thread;

  if (pthread_create(&thread, NULL, SendRTP, this)){
    printf("SendRTP Error\n");
  }

  while(true){
    struct soc sock;
    connfd = accept(_listenfd, (sockaddr*)&sa_cli, &cli_addr_len);

    if (connfd < 0) {
      printf("accept() error\n");
      usleep(1000*1000);
      continue;
    }

    sock.connfd = connfd;
    sock.sa_cli = sa_cli;
    sock.server = this;

    if (pthread_create(&thread, NULL, Loop, (void *)&sock)){
      printf("Loop pthread_create error");
    }
  }
}

void *RTSPServer::Loop(void *newsock){
  pthread_detach(pthread_self());

  int connfd = ((struct soc *)newsock)->connfd;
  char clientIP[16];
  sockaddr_in sa_cli = ((struct soc*)newsock)->sa_cli;
  inet_ntop(AF_INET, &sa_cli.sin_addr.s_addr, clientIP, sizeof(clientIP));
  printf("New Client: %s:%d\n", clientIP, ntohs(sa_cli.sin_port);


  RTSPServer *serv = (RTSPServer *)(((struct soc *)newsock)->server);
  char read_buf[2048], write_buf[2048];
  int n;
  RTSPParser *rtsppar = new RTSPParser(clientIP);

  snprintf(read_buf, sizeof(read_buf), "%s:%d", clientIP, ntohs(sa_cli.sin_port));

  std::map<std::string, RTSPParser*> pars = serv->Getparser();
  serv->Addparser(read_buf, rtsppar);

  memset(read_buf, 0, sizeof(read_buf));

  pthread_t thread;
  while ((n = read(connfd, read_buf, sizeof(read_buf))) > 0){

    snprintf(write_buf, sizeof(write_buf), "%s", rtsppar->Renew(read_buf));
    
    if (strncmp(write_buf, rtsppar->Getnofile(), 29) == 0){
      printf("no such file: %s\n", rtsppar->Getfiledir());
      write(connfd, write_buf, strlen(write_buf));
      return NULL;
    }
    
    printf("buf: %s\n", write_buf);


    write(connfd, write_buf, strlen(write_buf));
    memset(read_buf, 0, sizeof(read_buf));
    memset(write_buf, 0, sizeof(write_buf));

    if (rtsppar->Getteardown()){
      rtsppar->Setcomplete();
      return NULL;
    }
  }

}

char* RTSPServer::Geturl(void){
  return _url;
}

void RTSPServer::Createurl(void){
  int fd; 
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0); 

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);
  
  char newurl[100];
  /* display result */
  snprintf(newurl, sizeof(newurl), "rtsp://%s:%d/", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), _port);
  
  _url = strdup(newurl);
}

void* RTSPServer::SendRTP(void *arg){
  pthread_detach(pthread_self());
int k = 1;
  printf("SendRTP Created\n");
  RTSPServer *server = (RTSPServer *)arg;
  std::map<std::string, RTSPParser*> pars = server->Getparser();
  while (true){
    usleep(1000);
    pars = server->Getparser();
  //  printf("%x %x\n", pars.begin(), pars.end());
    for (auto i = pars.begin(); i != pars.end();){
      if (i->second->Getteardown() && i->second->Getcomplete()){
        //delete i->second; // TODO: Double free in here
        pars.erase(i++);
        continue;
      }
      else if (!i->second->Getteardown()){
        if (i->second->GetRTPS() == NULL){
          i++;
          continue;
        }
        i->second->GetRTPS()->Play();
        i++;
      }
    }
  }
}

std::map<std::string, RTSPParser*> RTSPServer::Getparser(void){
  return _parser;
}

void RTSPServer::Addparser(char *key, RTSPParser *value){
  _parser[key] = value;
}
