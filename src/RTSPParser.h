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
#define MAX_SERVER 100

#include <map>

using namespace std;
struct cmp_str
{
   bool operator()(char const *a, char const *b)
   {
       return strcmp(a, b) < 0;
   }
};

class RTSPParser {
  public:
    RTSPParser(char *);
    ~RTSPParser(void);
    void Createip();
    void Create(char *);
	char *Redirect(char *, const char *, const char*);
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
	map <char *,int,cmp_str> _bitrate;

  
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
};

