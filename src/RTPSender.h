#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cassert>
#include <unistd.h>

#define KEYLEN 8
#define TSPACKETNUM 7
#define TSPACKETSIZE 188
#define PACKETSIZE 8*12 + TSPACKETSIZE*TSPACKETNUM
#define MPEG2TS 33

class RTPSender {
  public:
    RTPSender(void);
    ~RTPSender(void);
    void Open(char *);
    void Close(void);
    bool Create(int clientport);
    void Createid(void);
    char *Getid(void);
    bool Hasfile(void);
    bool Hasindex(void);
    char *Getplaytime(void);
    void Play(struct sockaddr_in);
    void Pause(void);
    int Getclientport(void);
	int Getserverport(void);
	void setPlay(bool);
	bool isPlaying(void);



  private:
    FILE *_fp, *_xfp;
    char _id[KEYLEN];
    bool _play;
    int _rtpfd, _rtcpfd;
    int _clientrtpport, _serverrtpport, _serverrtcpport, _clientrtcpport;
    struct sockaddr_in _rtp_addr, _rtcp_addr;
    int _ssrc;
    int _seq;
};
