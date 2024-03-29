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
#include <mutex>

#include <map>

#include "RTSPParser.h"
#include "Server.h"
#include "Client.h"

struct soc {
  int connfd;
  sockaddr_in sa_cli;
  void *server;
  RTSPParser *rtsppar;
};

class RTSPServer {
  public:
    RTSPServer(struct sockaddr_in, int, int);
    ~RTSPServer(void);
    RTSPServer* Create(int);
	void Loadbalance(int, char*[MAX_SERVER],char*[MAX_SERVER]);
    void Accept(void);
    static void *Loop(void *);
    char* Geturl(void);
    static void* SendRTP(void *);
    std::map<std::string, RTSPParser*> Getparser(void);
    void Addparser(char *, RTSPParser *);
	void Makeclient(char *,char *);
	Client *Getclient();

  private:
	Client *_client;
    struct sockaddr_in _server_addr;
    int _listenfd, _port;
    char *_url;
    std::map <std::string, RTSPParser*> _parser;

    void Createurl(void);
};
