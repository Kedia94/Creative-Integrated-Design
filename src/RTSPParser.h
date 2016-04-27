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

class RTSPParser {
  public:
    RTSPParser(void);
    ~RTSPParser(void);
    void Create(char *);
    char *Renew(char *);
    char *Option(char *);
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
    char *Getcseq(void);

  
  private:
    char *_ret;
    char *_factor, *_fileurl, *_version;
    char *_cseq, *_code;
    unsigned short _session, _rtpport, _rtcpport;

};

