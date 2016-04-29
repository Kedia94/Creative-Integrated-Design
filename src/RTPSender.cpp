#include "RTPSender.h"

RTPSender::RTPSender(void){
  _fp = NULL;
  _xfp = NULL;
  _play = false;
  _seq=0;
  Createid();
}

RTPSender::~RTPSender(void){
  if (_fp != NULL){
    fclose(_fp);
    _fp = NULL;
  }
  if (_xfp != NULL){
    fclose(_xfp);
    _xfp = NULL;
  }
}

void RTPSender::Open(char *name){
  _fp = fopen(name, "r");
  
  char xname[512];

  snprintf(xname, sizeof(xname), "%sx", name);

  _xfp = fopen(xname, "r");

}

void RTPSender::Close(void){
  if (_fp != NULL){
    fclose(_fp);
    _fp = NULL;
  }
  if (_xfp != NULL){
    fclose(_xfp);
    _xfp = NULL;
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
  return (_fp != NULL);
}

bool RTPSender::Hasindex(void){
  return (_xfp != NULL);
}

char *RTPSender::Getplaytime(void){
  if (_xfp == NULL){
    return NULL;
  }

  return strdup("123.456");
}

void RTPSender::Play(char *time){
  _play = true;
}

void RTPSender::Pause(void){
  _play = false;
}

int RTPSender::Getport(void){
  return _rtpport;
}
