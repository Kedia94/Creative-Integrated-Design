#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define KEYLEN 8
#define TSPACKETNUM 7
#define TSPACKETSIZE 188
#define PACKETSIZE 12 + (TSPACKETSIZE * TSPACKETNUM)

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
    void Play(void);
    void SetClient(char *, int);
    void SetPlay(char *);
    void SetPause(void);
    int Getport(void);             //return RTP port
    int Getseq(void);
    int Gettimestamp(void);
    bool Getplay(void);

    ssize_t Readn(int, void *, size_t);

    int Readts(unsigned char *);


  private:
    int _fd, _xfd;
    char _id[KEYLEN];
    bool _play;
    int _rtpfd, _rtcpfd;
    int _rtpport, _rtcpport;
    struct sockaddr_in _rtp_addr, _rtcp_addr;
    int _ssrc;
    int _seq;
    int _timestamp;
    timeval _starttime, _playtime;

    struct sockaddr_in _client_addr;

};
