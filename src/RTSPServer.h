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
#include <string>

#include "RTSPParser.h"

class RTSPServer {
  public:
    RTSPServer(struct sockaddr_in, int, int);
    ~RTSPServer(void);
    RTSPServer* Create(int);
    void Accept(void);
    char* Geturl(void);

  private:
    struct sockaddr_in _server_addr;
    int _listenfd, _port;
    char *_url;

    void Createurl(void);

};
