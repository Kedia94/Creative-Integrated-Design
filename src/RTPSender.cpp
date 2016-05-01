#include "RTPSender.h"

RTPSender::RTPSender(void){
  _fd = -1;
  _xfd = -1;
  _play = false;
  _seq=0;
  Createid();
}

RTPSender::~RTPSender(void){
  Close();
}

void RTPSender::Open(char *name){
  _fd = open(name, O_RDONLY);
  
  char xname[512];

  snprintf(xname, sizeof(xname), "%sx", name);

  _xfd = open(xname, O_RDONLY);

  gettimeofday(&_starttime, NULL);
  _playtime.tv_sec = _starttime.tv_sec;
  _playtime.tv_usec = _starttime.tv_usec;

}

void RTPSender::Close(void){
  if (_fd > 0){
    close(_fd);
    _fd = -1;
  }
  if (_xfd > 0){
    close(_xfd);
    _xfd = -1;
  }
}

bool RTPSender::Create(int port){
  _rtpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (_rtpfd < 0){
    printf("rtp socket() error\n");
    return false;
  }
  _rtcpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (_rtcpfd < 0){
    printf("rtcp socket() error\n");
    return false;
  }

  _rtp_addr.sin_family = AF_INET;
  _rtp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  _rtcp_addr.sin_family = AF_INET;
  _rtcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  while (true){
    _rtp_addr.sin_port = htons((unsigned short)port);
    if (bind(_rtpfd, (struct sockaddr*)&_rtp_addr, sizeof(_rtp_addr)) < 0){
      port += 2;
      continue;
    }

    port++;

    _rtcp_addr.sin_port = htons((unsigned short)port);
    if (bind(_rtcpfd, (struct sockaddr*)&_rtcp_addr, sizeof(_rtcp_addr)) <0){
      port += 2;
      continue;
    }

    break;
  }
  _rtpport = port-1;
  _rtcpport = port;
  return true;
}
  
void RTPSender::Createid(void){
  int tmp;
  for (int i=0; i<KEYLEN; i++){
    tmp = rand()%36;
    if (tmp <10){
      _id[i] = '0'+tmp;
    } else {
      _id[i] = 'A'+tmp-10;
    }
  }
  _id[KEYLEN] = '\0';

  _ssrc = rand();
}
char *RTPSender::Getid(void){
  return _id;
}

bool RTPSender::Hasfile(void){
  return (_fd > 0);
}

bool RTPSender::Hasindex(void){
  return (_xfd > 0);
}

char *RTPSender::Getplaytime(void){
  if (_xfd >0){
    return strdup("123.456");
  }

  return strdup("");
}

void RTPSender::Play(void){
  if (!_play){
    return;
  }

  int i;
  unsigned char buf[PACKETSIZE];

  buf[0] = 1<<7; // 0x80
  buf[1] = 33;   // 0x21

  *((unsigned short *)(buf+2)) = htons(_seq++);
  *((unsigned int *)(buf+4)) = htonl(_timestamp);
  *((unsigned int *)(buf+8)) = htonl(_ssrc);

  _timestamp += 236;

  for (i=0; i<7; i++){
    Readn(_fd, buf+12+188*i, 188); 

  }

  if (sendto(_rtpfd, buf, PACKETSIZE, 0, (struct sockaddr *)&_client_addr, sizeof(_client_addr)) == -1)
    return;

}

void RTPSender::SetClient(char *ip, int port){
  _client_addr.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &_client_addr.sin_addr);
  _client_addr.sin_port = htons(port);
}

void RTPSender::SetPlay(char *time){
  _play = true;

  //TODO: Set playtime to *time
}

void RTPSender::SetPause(void){
  _play = false;
}

int RTPSender::Getport(void){
  return _rtpport;
}

int RTPSender::Getseq(void){
  return _seq;
}

int RTPSender::Gettimestamp(void){
  return _timestamp;
}

bool RTPSender::Getplay(void){
  return _play;
}
int RTPSender::Readts(unsigned char *buf){
//TODO
  double npt;
return 1;


}

ssize_t RTPSender::Readn(int fd, void *usrbuf, size_t n){
  size_t nleft = n;
  ssize_t nread;
  char *bufp = (char *)usrbuf;

  while (nleft > 0){
    if ((nread = read(fd, bufp, nleft)) < 0){
      if (errno = EINTR){
        nread = 0;
      }
      else{
        return -1;
      }
    }
    else if (nread == 0){
      break;
    }
    nleft -= nread;
    bufp += nread;
  }

  return (n-nleft);
}

