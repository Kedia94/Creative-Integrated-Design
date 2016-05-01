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
#include <pthread.h>

#include <map>

#include "RTSPParser.h"

struct soc {
  int connfd;
  sockaddr_in sa_cli;
  void *server;
};

class RTSPServer {
  public:
    RTSPServer(struct sockaddr_in, int, int);
    ~RTSPServer(void);
    RTSPServer* Create(int);
    void Accept(void);
    static void *Loop(void *);
    char* Geturl(void);
    static void* SendRTP(void *);
    std::map<std::string, RTSPParser*> Getparser(void);
    void Addparser(char *, RTSPParser *);

  private:
    struct sockaddr_in _server_addr;
    int _listenfd, _port;
    char *_url;
    std::map <std::string, RTSPParser*> _parser;

    void Createurl(void);

};
