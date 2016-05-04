#include "RTPSender.h"

RTPSender::RTPSender(void){
  _fd = -1;
  _xfd = -1;
  _play = false;
  _seq=0;
  _tsr = NULL;
  Createid();
  _curtpn = 0;
}

RTPSender::~RTPSender(void){
  Close();
}

void RTPSender::Open(char *name){
  _fd = open(name, O_RDONLY);
  
  char xname[512];

  snprintf(xname, sizeof(xname), "%sx", name);

  _xfd = open(xname, O_RDONLY);

  if (_xfd > 0){
    Readtsx();
  }

  _tsr = new Reader(_fd);


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
    char buf[20];
    snprintf(buf, sizeof(buf), "%f", _dur);
    return strdup(buf);
  }

  return strdup("");
}

void RTPSender::Play(void){
  if (!_play){
    return;
  }

  int i;
  unsigned char buf[PACKETSIZE];

  buf[0] = 0x80; // 0x80
  buf[1] = 0x21;   // 0x21

  *((unsigned short *)(buf+2)) = htons(_seq);
  *((unsigned int *)(buf+4)) = htonl(_timestamp);
  *((unsigned int *)(buf+8)) = htonl(_ssrc);


  for (i=0; i<7; i++){
    int res = Getframe(buf+12+188*i);
    if (res == -1){
      _play = false;
      break;
    } else if (res == -2){
      break;
    } else {
      _timestamp += res;
    }
  }

  if (i>0){
    if (sendto(_rtpfd, buf, 12+188*i, 0, (struct sockaddr *)&_client_addr, sizeof(_client_addr)) == -1)
      return;
    _seq++;
  }

}

void RTPSender::SetClient(char *ip, int port){
  _client_addr.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &_client_addr.sin_addr);
  _client_addr.sin_port = htons(port);
}

void RTPSender::SetPlay(char *time){
  _play = true;

  double npt;
  //TODO: Set playtime to *time
  if (!Hasindex())
    return;
  if (time != NULL){
    npt = atof(time);
    printf("%f\n", npt);
    Seeknpt(npt);
    printf("%f\n", Getnpt());
  }

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

double RTPSender::Getnpt(void){
  return pcrs.size() > _curtpn ? pcrs[_curtpn]: 0.0;
}


bool RTPSender::Seeknpt(double npt){
  if (npt < 0){
    if (_curtpn >= pcrs.size())
      return false;
    npt = pcrs[_curtpn];
  }
  else {
    auto it = iframe.lower_bound(npt);
    if (it != iframe.begin())
      it--;
    npt = it->first;
    _curtpn = it->second;
    printf("npt: %f, %f\n", npt, _curtpn);
    int res = _tsr->seek(188 * _curtpn, SEEK_SET);
  }

  gettimeofday(&_starttime, NULL);
  _starttime.tv_sec -= (int)npt;
  _starttime.tv_usec -= (npt - (int)npt) * 1000000;

  if (_starttime.tv_usec <0){
    _starttime.tv_usec += 1000000;
    _starttime.tv_sec -= 1;
  }
  return true;
}

int RTPSender::Getframe(unsigned char *buf){
  if (pcrs.size() <= _curtpn){
    _curtpn = 0;
    return -1;
  }
  double npt = pcrs[_curtpn];
  timeval playtime;
  playtime.tv_sec = _starttime.tv_sec + (int) npt;
  playtime.tv_usec = _starttime.tv_usec + (npt - (int)npt)*1000000;

  if (playtime.tv_usec >= 1000000){
    playtime.tv_usec -= 1000000;
    playtime.tv_sec += 1;
  }

  timeval curttime;
  gettimeofday(&curttime, NULL);
  if (curttime.tv_sec < playtime.tv_sec || (curttime.tv_sec == playtime.tv_sec && curttime.tv_usec < playtime.tv_usec)){
    return -2;
  }

  _curtpn++;
  int res = _tsr->readn((char *)buf, 188);
  if (res != 188){
    printf("wrong");
  }

  int ret;
  if (pcrs.size() <= _curtpn){
    ret = 0;
  } else {
    ret = (pcrs[_curtpn] - pcrs[_curtpn - 1])*100000;
  }
  return ret;
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


enum RecordType {
  RECORD_UNPARSED = 0,
  RECORD_VSH = 1, // a MPEG Video Sequence Header
  RECORD_GOP = 2,
  RECORD_PIC_NON_IFRAME = 3, // includes slices
  RECORD_PIC_IFRAME = 4, // includes slices
  RECORD_NAL_H264_SPS = 5, // H.264
  RECORD_NAL_H264_PPS = 6, // H.264
  RECORD_NAL_H264_SEI = 7, // H.264
  RECORD_NAL_H264_NON_IFRAME = 8, // H.264
  RECORD_NAL_H264_IFRAME = 9, // H.264
  RECORD_NAL_H264_OTHER = 10, // H.264
  RECORD_NAL_H265_VPS = 11, // H.265
  RECORD_NAL_H265_SPS = 12, // H.265
  RECORD_NAL_H265_PPS = 13, // H.265
  RECORD_NAL_H265_NON_IFRAME = 14, // H.265
  RECORD_NAL_H265_IFRAME = 15, // H.265
  RECORD_NAL_H265_OTHER = 16, // H.265
  RECORD_JUNK
};



void RTPSender::Readtsx(void){
  /*
     fTo[0] = (u_int8_t)(head->recordType());
     fTo[1] = head->startOffset();
     fTo[2] = head->size();
         // Deliver the PCR, as 24 bits (integer part; little endian) + 8 bits (fractional part)
     float pcr = head->pcr();
     unsigned pcr_int = (unsigned)pcr;
     u_int8_t pcr_frac = (u_int8_t)(256*(pcr-pcr_int));
     fTo[3] = (unsigned char)(pcr_int);
     fTo[4] = (unsigned char)(pcr_int>>8);
     fTo[5] = (unsigned char)(pcr_int>>16);
     fTo[6] = (unsigned char)(pcr_frac);
         // Deliver the transport packet number (in little-endian order):
     unsigned long tpn = head->transportPacketNumber();
     fTo[7] = (unsigned char)(tpn);
     fTo[8] = (unsigned char)(tpn>>8);
     fTo[9] = (unsigned char)(tpn>>16);
     fTo[10] = (unsigned char)(tpn>>24);
     fFrameSize = 11;
     */
  printf("Start read\n");
  if (!Hasindex())
    return;

  unsigned char buf[11];
  double pcr;

  pcrs.clear();
  while (true){
    if (Readn(_xfd, (char*)buf, 11) == 0)
      break;

    unsigned char recordtype = buf[0];
    unsigned pcr_int = (unsigned)buf[3] + ((unsigned)buf[4]<<8) + ((unsigned)buf[5]<<16);
    unsigned pcr_frac = buf[6];
    pcr = pcr_int + pcr_frac / 256.0;

    unsigned tpn = *(unsigned*)(&buf[7]);
    if (recordtype == RECORD_NAL_H264_IFRAME || recordtype == RECORD_NAL_H265_IFRAME)
      iframe[pcr] = tpn;

    while (pcrs.size() <= tpn)
      pcrs.push_back(pcr);
  }
  iframe[0.0] = 0;
  _dur = pcr;
  printf("(debug) Duration %f\n", pcr);
}
