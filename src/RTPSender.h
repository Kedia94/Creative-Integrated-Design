#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define KEYLEN 8

class RTPSender {
  public:
    RTPSender(void);
    ~RTPSender(void);
    void Open(char *);
    void Close(void);
    bool Create(int port);
    void Createid(void);
    char *Getid(void);
    bool Hasfile(void);
    bool Hasindex(void);
    char *Getplaytime(void);
    void Play(char *);
    void Pause(void);
    int Getport(void);



  private:
    FILE *_fp, *_xfp;
    char _id[KEYLEN];
    bool _play;
    int _rtpfd, _rtcpfd;
    int _rtpport, _rtcpport;
    struct sockaddr_in _rtp_addr, _rtcp_addr;
    int _ssrc;
    int _seq;
};
