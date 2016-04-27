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

  listenfd = socket(AF_INET, SOCK_STREAM, 0); 
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
  
  int connfd = accept(_listenfd, (sockaddr*)&sa_cli, &cli_addr_len);
  if (connfd < 0) {
    return;
  }
  
  char clientIP[16];
  inet_ntop(AF_INET, &sa_cli.sin_addr.s_addr, clientIP, sizeof(clientIP));
  printf("%s\n", clientIP);

  char buf[2048];
  int n;
  
  while ((n = read(connfd, buf, 256)) > 0){
    printf("(debug) Got\n%s\n\n", buf);
    
    RTSPParser *rtsppar = new RTSPParser();

    snprintf(buf, sizeof(buf), "%s", rtsppar->Renew(buf));

    write(connfd, buf, strlen(buf));
  }

}

char* RTSPServer::Geturl(void){
  return _url;
}

void RTSPServer::Reply(void){
  RTSPParser rtsppar;
  std::string ResponseBuffer;
  
  char *CurrentCSeq;
  /*
  snprintf((char *)ResponseBuffer, sizeof(ResponseBuffer), 
           "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n", 
           CurrentCSeq, rtsp.Getdate(), Date);
*/
  ResponseBuffer = "RTSP/1.0 200 OK\r\nCSeq: ";
  rtsppar.Getdate();
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
  sprintf(newurl, "rtsp://%s:%d/", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), _port);
  
  _url = strdup(newurl);
}
