#include "RTSPServer.h"

std::mutex mutx;

RTSPServer::RTSPServer(struct sockaddr_in server_addr, int listenfd,  int port){
  _server_addr = server_addr;
  _listenfd = listenfd;
  _port = port;
  _client = NULL;
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

void RTSPServer::Loadbalance(int servernum, char *server_ip[MAX_SERVER], char *server_port[MAX_SERVER]){
  sockaddr_in sa_cli;
  socklen_t cli_addr_len = sizeof(sa_cli);
  
  int connfd;

  Server server;
  server.makeNConnection(9000, servernum);

  while(true){
    struct soc sock;
    connfd = accept(_listenfd, (sockaddr*)&sa_cli, &cli_addr_len);

    char clientIP[16];
    char buf[32];

    inet_ntop(AF_INET, &sa_cli.sin_addr.s_addr, clientIP, sizeof(clientIP));
    snprintf(buf, sizeof(buf), "%s:%d", clientIP, ntohs(sa_cli.sin_port));

    RTSPParser *rtsppar = new RTSPParser(clientIP);

    printf("New client %s\n", buf);

    mutx.lock();
    _parser[buf] = rtsppar;
    mutx.unlock();

    if (connfd < 0) {
      printf("accept() error\n");
      usleep(1000*1000);
      continue;
    }

	char read_buf[2048], write_buf[2048];
	read(connfd, read_buf, sizeof(read_buf));
	server.updateBandwidth();
	int selected = server.selectServer();
	const char * serverIp = server_ip[selected];
	const char * serverPort = server_port[selected];
	printf("%s %s\n",serverIp,serverPort);
	snprintf(write_buf, sizeof(write_buf), "%s",rtsppar->Redirect(read_buf,serverIp, serverPort));
	printf("buf: %s\n", write_buf);

	write(connfd, write_buf, strlen(write_buf));
	memset(read_buf, 0, sizeof(read_buf));
	memset(write_buf, 0, sizeof(write_buf));
  }
}

void RTSPServer::Accept(){
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

    char clientIP[16];
    char buf[32];

    inet_ntop(AF_INET, &sa_cli.sin_addr.s_addr, clientIP, sizeof(clientIP));
    snprintf(buf, sizeof(buf), "%s:%d", clientIP, ntohs(sa_cli.sin_port));

    RTSPParser *rtsppar = new RTSPParser(clientIP);

    printf("New client %s\n", buf);

    mutx.lock();
    _parser[buf] = rtsppar;
    mutx.unlock();


    if (connfd < 0) {
      printf("accept() error\n");
      usleep(1000*1000);
      continue;
    }

    sock.connfd = connfd;
    sock.sa_cli = sa_cli;
    sock.server = this;
    sock.rtsppar = rtsppar;

    if (pthread_create(&thread, NULL, Loop, (void *)&sock)){
      printf("Loop pthread_create error");
    }
    mutx.lock();
    for (auto i = _parser.begin(); i != _parser.end();){
      if (i->second->Getteardown()){
        _parser.erase(i++);
        continue;
      }
      i++;
    }
    mutx.unlock();
    printf("mtx unlock\n");
  }
}

void *RTSPServer::Loop(void *newsock){
  pthread_detach(pthread_self());

  int connfd = ((struct soc *)newsock)->connfd;
  char clientIP[16];
  sockaddr_in sa_cli = ((struct soc*)newsock)->sa_cli;
  inet_ntop(AF_INET, &sa_cli.sin_addr.s_addr, clientIP, sizeof(clientIP));
//  printf("New Client: %s:%d\n", clientIP, ntohs(sa_cli.sin_port));


  RTSPServer *serv = (RTSPServer *)(((struct soc *)newsock)->server);
  char read_buf[2048], write_buf[2048];
  int n;
  RTSPParser *rtsppar = ((struct soc *)newsock)->rtsppar;

//  snprintf(read_buf, sizeof(read_buf), "%s:%d", clientIP, ntohs(sa_cli.sin_port));

//  std::map<std::string, RTSPParser*> pars = serv->Getparser();
//  serv->Addparser(read_buf, rtsppar);

//  memset(read_buf, 0, sizeof(read_buf));

  pthread_t thread;
  bool wasPlaying = false;
  while ((n = read(connfd, read_buf, sizeof(read_buf))) > 0){

    snprintf(write_buf, sizeof(write_buf), "%s", rtsppar->Renew(read_buf));
    
    if (strncmp(write_buf, rtsppar->Getnofile(), 29) == 0){
      printf("no such file: %s\n", rtsppar->Getfiledir());
      write(connfd, write_buf, strlen(write_buf));
      rtsppar->Setteardown();
      rtsppar->Setcomplete();
      return NULL;
    }
    

    write(connfd, write_buf, strlen(write_buf));
    memset(read_buf, 0, sizeof(read_buf));
    memset(write_buf, 0, sizeof(write_buf));

	if ((serv->_client != NULL) && (rtsppar->GetRTPS() != NULL)){
		bool isPlaying = rtsppar->GetRTPS()->Getplay();
		char * filedir = rtsppar->GetRTPS()->_filedir;
		int bitrate = rtsppar->_bitrate[filedir];
		if (isPlaying^wasPlaying){
			if(wasPlaying){
				serv->_client->addBandwidth(-bitrate);
			}
			else
				serv->_client->addBandwidth(bitrate);
			wasPlaying = isPlaying;
			serv->_client->reportBandwidth();
		}
	}

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
    //    printf("%x %x\n", pars.begin(), pars.end());
    mutx.lock();
    for (auto i = pars.begin(); i != pars.end();i++){
      //      printf("%x teardown: %d, %d\n", i, i->second->Getteardown(), i->second->Getcomplete());
      if (i->second->Getteardown() || i->second->GetRTPS() == NULL){
        continue;
      }
      else {
        i->second->GetRTPS()->Play();
      }
    }
    mutx.unlock();
  }
}

std::map<std::string, RTSPParser*> RTSPServer::Getparser(void){
  return _parser;
}

void RTSPServer::Addparser(char *key, RTSPParser *value){
  _parser[key] = value;
}

void RTSPServer::Makeclient(char * balancer_ip, char * balancer_port){
	_client = new Client();
	_client->makeConnection((const char *)balancer_ip, (const char *)balancer_port);
}

Client *RTSPServer::Getclient(){
	return _client;
}
