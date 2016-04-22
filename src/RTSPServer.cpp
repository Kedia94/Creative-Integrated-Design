#include "RTSPServer.h"

RTSPServer::RTSPServer(struct sockaddr_in _server_addr, int _server_sock,  int _port){
  server_addr = _server_addr;
  server_sock = _server_sock;
  port = _port;
  Createurl();
}

RTSPServer* RTSPServer::Create(int _port){

  int server_sock, clnt_sock;
  socklen_t addr_size;
  struct sockaddr_in server_addr;
  struct sockaddr_in clnt_addr;

  server_sock = socket(PF_INET, SOCK_STREAM, 0); 
  if (server_sock == -1){
    printf("socket() error\n");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = _port;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
    return NULL; 
  }
  if (listen(server_sock, 5) == -1){
    printf("listen() error\n");
    return NULL; 
  }

  return new RTSPServer(server_addr, server_sock, _port);
}

char* RTSPServer::GetRTSPurl(void){
  return url;
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
  sprintf(newurl, "rtsp://%s:%d/", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), port);
  
  url = strdup(newurl);
}
