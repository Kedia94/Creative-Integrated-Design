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

class RTSPParser {
  public:
    RTSPParser(char *);
    ~RTSPParser(void);
    void Createip();
    void Create(char *);
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
	void sendRTP(void);
	RTPSender *_rtps;


  
  private:
    char *_ret;
    char *_factor, *_fileurl, *_version;
    char *_cseq, *_code, *_filedir, *_ip;
    unsigned short _session;
    char *_clientIP;
};

