#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>


class RTSPServer {
  public:
    RTSPServer(struct sockaddr_in, int, int);
    RTSPServer* Create(int);
    char* GetRTSPurl(void);

  private:
    struct sockaddr_in server_addr;
    int server_sock, port;
    char* url;

    void Createurl(void);

};
