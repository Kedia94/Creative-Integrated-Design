#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/time.h>

#include "RTPSender.h"
#define SERVER_NUM 2

class RTSPParser {
  public:
    RTSPParser(char *);
    ~RTSPParser(void);
    void Createip();
    void Create(char *);
	char *Redirect(char *,int);
    char *Renew(char *);
    char *Options(char *);
    char *Describe(char *);
    char *Setup(char *);
    char *Teardown(char *);
    char *Play(char *);
    char *Pause(char *);
    char *Get_parameter(char *);
    char *Set_parameter(char *);
    char *Getdate(void);
    char *Getfactor(void);
    char *Getfileurl(void);
    char *Getversion(void);
    char *Getfiledir(void);
    char *Getcseq(void);
    char *Getnofile(void);
    char *GetSDP(void);
    char *Createsessionid(void);
    void Setteardown(void);
    bool Getteardown(void);
    RTPSender *GetRTPS(void);
    void Setcomplete(void);
    bool Getcomplete(void);

  
  private:
    RTPSender *_rtps;
    char *_ret;
    char *_factor, *_fileurl, *_version;
    char *_cseq, *_code, *_filedir, *_ip;
    unsigned short _session, _rtpport, _rtcpport;
    int _nextrtpport;
    char *_clientIP;
    bool _teardown;
    bool _complete;
	const char * server_ip[SERVER_NUM] = { "192.168.153.129", "192.168.153.129" };
	const int server_port[SERVER_NUM] = { 8552, 8550 };
};

