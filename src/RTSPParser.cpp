#include "RTSPParser.h"

RTSPParser::RTSPParser(char *clientIP){
  _version = strdup("RTSP/1.0");
  _code = strdup("200 OK");
  Createip();
  _nextrtpport = 6970;
  _clientIP = strdup(clientIP);
  _teardown = false;
  _complete = false;
}

RTSPParser::~RTSPParser(void){

  delete _rtps;
}

void RTSPParser::Createip(void){
  int fd;
  struct ifreq ifr;

  fd = socket(AF_INET, SOCK_DGRAM, 0);

  /* I want to get an IPv4 IP address */
  ifr.ifr_addr.sa_family = AF_INET;

  /* I want IP address attached to "eth0" */
  strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

  ioctl(fd, SIOCGIFADDR, &ifr);

  close(fd);

  char newurl[100];
  /* display result */
  snprintf(newurl, sizeof(newurl), "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  _ip = strdup(newurl);
}

void RTSPParser::Create(char *rtsp){
  char *ptr, *bp_ptr; 
  ptr = strtok_r(strdup(rtsp), "\r\n", &bp_ptr);

  char *line, *bp_line;

  line = strtok_r(ptr, " ", &bp_line);
  _factor = strdup(line);

  line = strtok_r(bp_line, " ", &bp_line);
  _fileurl = strdup(line);
  _filedir = strdup(line);

  line = strtok_r(bp_line, " ", &bp_line);
//  _version = strdup("RTSP/1.0");
  
  ptr = strtok_r(bp_ptr, "\r\n", &bp_ptr);
  _cseq = strdup(ptr);

  line = strtok_r(_filedir, "/", &bp_line);
  line = strtok_r(bp_line, "/", &bp_line);
  _filedir = strdup(bp_line);

//  _code = strdup("200 OK");
}

char *RTSPParser::Renew(char *rtsp){
  Create(rtsp);
printf("%s\n",_factor);
  if (strcmp(_factor, "OPTIONS") == 0){
    return Options(rtsp);
  }
  else if (strcmp(_factor, "DESCRIBE") == 0){
    return Describe(rtsp);
  }
  else if (strcmp(_factor, "SETUP") == 0){
    return Setup(rtsp);
  }
  else if (strcmp(_factor, "TEARDOWN") == 0){
    return Teardown(rtsp);
  }
  else if (strcmp(_factor, "PLAY") == 0){
    return Play(rtsp);
  }
  else if (strcmp(_factor, "PAUSE") == 0){
    return Pause(rtsp);
  }
  else if (strcmp(_factor, "GET_PARAMETER") == 0){
    return Get_parameter(rtsp);
  }
  else if (strcmp(_factor, "SET_PARAMETER") == 0){
    return Set_parameter(rtsp);
  }

  return rtsp;
}

char *RTSPParser::Options(char *rtsp){
  char buf[2048];
  snprintf(buf, sizeof(buf), "%s %s\r\n"
                            "%s\r\n"
                            "%s\r\n"
                            "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, GET_PARAMETER, SET_PARAMETER\r\n\r\n",
                            _version,
                            _code,
                            _cseq,
                            Getdate());

  _ret = strdup(buf);
  return _ret;
}

char *RTSPParser::Describe(char *rtsp){
  char buf[2048];

  printf("file: %s\n", _filedir);

  _rtps = new RTPSender();
  _rtps->Open(_filedir);

  if (!_rtps->Hasfile()) {
    snprintf(buf, sizeof(buf), "%s 404 Stream Not Found\r\n"
                               "%s\r\n"
                               "%s\r\n\r\n",
                               _version,
                               _cseq,
                               Getdate());
    _teardown = true;
    _ret = strdup(buf);
    return _ret;
  }



  char *tmp = GetSDP();
  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n"
                             "Content-Base: %s\r\n"
                             "Content-Type: application/sdp\r\n"
                             "Content-Length: %d\r\n\r\n"
                             "%s\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate(),
                             _fileurl, 
                             (int)strlen(tmp), 
                             tmp); 
  
  _ret = strdup(buf);
  return _ret;

}

char *RTSPParser::Setup(char *rtsp){
  char buf[2048];
  char *transport, *ptr;
  strtok_r(rtsp, "\r\n", &ptr);
  strtok_r( ptr, "\r\n", &ptr);
  strtok_r( ptr, "\r\n", &ptr);
  transport = strtok_r(ptr, "\r\n", &ptr);

  // example
  // transport =  Transport: RTP/AVP;unicast;client_port=8000-8001

  strtok_r(strdup(transport), "=", &ptr);
  int rtpport = atoi(strtok_r(ptr, "-", &ptr));
  
  // rtpport = 8000

  printf("%d\n", rtpport);
  if (!_rtps->Create(_nextrtpport)){
    printf("Error on binding rtp, rtcp\n");
  }
  _nextrtpport += 2;

  _rtps->SetClient(_clientIP, rtpport);

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n"
                             "Transport: RTP/AVP;unicast;destination=%s;source=%s;client_port=%d-%d;server_port=%d-%d\r\n"
                             "Session: %s;timeout=%d\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate(),
                             _clientIP, 
                             _ip,
                             rtpport,
                             (rtpport+1),
                             _rtps->Getport(),
                             (_rtps->Getport()+1),
                             _rtps->Getid(),
                             64); // TODO: timeout
  
  _ret = strdup(buf);
      
  return _ret;
}

char *RTSPParser::Teardown(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate());

  _teardown = true;

  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Play(char *rtsp){
  char buf[2048];

  /*
   * CSeq: current sequence number
   * 
   * Pause / Play
   *
   * PLAY rtsp://192.168.0.190:8554/h265.ts RTSP/1.0\r\n
   * CSeq: 19\r\n
   * User-Agent: ~~\r\n
   * Session: CCE03A08\r\n
   * \r\n
   *
   * Seek / Play
   *
   * PLAY rtsp://192.168.0.190:8554/h265.ts RTSP/1.0\r\n
   * CSeq: 21\r\n
   * User-Agent: ~~\r\n
   * Session: CCE03A08\r\n
   * Range: npt=94.774-\r\n
   * \r\n
   */

  char *npt, *ptr;

  strtok_r(rtsp, "\r\n", &ptr);
  strtok_r( ptr, "\r\n", &ptr);
  strtok_r( ptr, "\r\n", &ptr);
  npt = strtok_r(ptr, "\r\n", &ptr);
  if (npt != NULL){
    strtok_r(npt, "=", &ptr);
    npt = strtok_r(ptr, "-", &ptr);
    printf("npt: %s\n", npt);
  }
   _rtps->SetPlay(npt); 
   printf("Play? %d\n", _rtps->Getplay());

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n"
                             "Session: %s\r\n"
                             "RTP-info: url=%s;seq=%d;rtptime=%d\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate(),
                             _rtps->Getid(), 
                             _clientIP,
                             _rtps->Getseq(),
                             _rtps->Gettimestamp()); // TODO: RTP-info
  
  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Pause(char *rtsp){
  char buf[2048];

  _rtps->SetPause();
  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n"
                             "Session: %s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate(),
                             _rtps->Getid()); 

  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Get_parameter(char *rtsp){
  char buf[2048];
  
  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n"
                             "Session: %s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate(),
                             _rtps->Getid());

  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Set_parameter(char *rtsp){
  char buf[2048];
  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             Getdate());

  _ret = strdup(buf);
  
  return _ret;
}

char *RTSPParser::Getdate(void){
  char buf[128];
  time_t tt = time(NULL);

  strftime(buf, sizeof(buf), "Date: %a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));

  return strdup(buf);
}

char *RTSPParser::Getfactor(void){
  return _factor;
}

char *RTSPParser::Getfileurl(void){
  return _fileurl;
}

char *RTSPParser::Getfiledir(void){
  return _filedir;
}

char *RTSPParser::Getversion(void){
  return _version;
}

char *RTSPParser::Getcseq(void){
  return _cseq;
}

char *RTSPParser::Getnofile(void){

  char buf[256];

  snprintf(buf, sizeof(buf), "%s 404 Stream Not Found\r\n"
                             "%s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _cseq,
                             Getdate());

  return strdup(buf);
}

char *RTSPParser::GetSDP(void){
  char buf[2048];

  snprintf(buf, sizeof(buf), "v=0\r\n"
                             "o=- %s 1 IN IP4 %s\r\n"
                             "s=MPEG Transport Stream, streamed by the HEVC Streaming Server\r\n"
                             "i=%s\r\n"
                             "t=0 0\r\n"
                             "a=tool:HEVC Streaming Server\r\n"
                             "a=type:broadcast\r\n"
                             "a=control:*\r\n"
                             "a=range:npt=0-%s\r\n"
                             "a=x-qt-text-nam:MPEG Treansport Stream by the HEVC Streaming Server\r\n"
                             "a=x-qt-text-inf:%s\r\n"
                             "m=video 0 RTP/AVP 33\r\n" // TS file payload is 33
                             "c=IN IP4 0.0.0.0\r\n"
                             "b=AS:4000\r\n",
                             //"a=control:track1\r\n",
 
                             Createsessionid(), //TODO: Create Session ID
                             _ip,
                             _filedir,
                             _rtps->Getplaytime(), // TODO: Video total playtime
                             _filedir);

  return strdup(buf);
}

char *RTSPParser::Createsessionid(void){
  char buf[256];

  struct timeval tv;
  gettimeofday(&tv, NULL);
  snprintf(buf, sizeof(buf), "%ld%06ld", tv.tv_sec, tv.tv_usec);
  
  return strdup(buf);
}

RTPSender *RTSPParser::GetRTPS(void){
  return _rtps;
}

bool RTSPParser::Getteardown(void){
  return _teardown;
}

void RTSPParser::Setcomplete(void){
  _complete = true;
}

bool RTSPParser::Getcomplete(void){
  return _complete;
}
