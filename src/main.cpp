#include "main.h"

int main(int argc, char **argv){
  

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
  server_addr.sin_port = 8554;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
    printf("bind() error\n");
    return -1;
  }
  if (listen(server_sock, 5) == -1){
    printf("listen() error\n");
    return -1;
  }
  printf("listening\n");

  
 manual(554);
 manual(8554);
  addr_size = sizeof(clnt_addr);
  clnt_sock = accept(server_sock, (struct sockaddr *)&clnt_addr, &addr_size);
  if (clnt_sock == -1){
    printf("accept() error\n");
    return -1;
  }
  printf("Connection established");







  return 0;
}

void manual(int port) {
  printf("H.265 Streaming Server\n");
  printf("    rtsp://%s:%i/<filename>\n", getmyip(), port);
}

char* getmyip(void){
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);

  /* display result */
  return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);

}
