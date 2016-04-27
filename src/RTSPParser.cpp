#include "RTSPParser.h"

RTSPParser::RTSPParser(void){

}
RTSPParser::~RTSPParser(void){

}

void RTSPParser::Create(char *rtsp){
  char *ptr, *bp_ptr; 
  ptr = strtok_r(rtsp, "\r\n", &bp_ptr);

  char *line, *bp_line;

  line = strtok_r(ptr, " ", &bp_line);
  _factor = line;

  line = strtok_r(bp_line, " ", &bp_line);
  _fileurl = line;

  line = strtok_r(bp_line, " ", &bp_line);
  _version = strdup("RTSP/1.0");
  
  ptr = strtok_r(bp_ptr, "\r\n", &bp_ptr);
  _cseq = ptr;

  _code = strdup("200 OK");
}

char *RTSPParser::Renew(char *rtsp){
  Create(rtsp);

  if (strcmp(_factor, "OPTION") == 0){
    return Option(rtsp);
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

char *RTSPParser::Option(char *rtsp){
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

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "Content-Base: %s\r\n"
                             "Content-Type: application/sdp\r\n"
                             "Content-Length: %d\r\n\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             _cseq, // TODO: Content-Base
                             5, //TODO: Content-Length
                             _cseq); // TODO: %s
  
  _ret = strdup(buf);

  return _ret;

}

char *RTSPParser::Setup(char *rtsp){
  return _ret;
}

char *RTSPParser::Teardown(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq);

  // TODO: Teardown this session

  return _ret;
}

char *RTSPParser::Play(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "Session: %s\r\n"
                             "RTP-info: %s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             _cseq, // TODO: Session
                             _cseq); // TODO: RTP-info
  
  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Pause(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "Session: %s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             _cseq); // TODO: Session

  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Get_parameter(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "Content-Length: %s\r\n"
                             "Content-Type: %s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             _cseq, // TODO: Content-Length
                             _cseq, // TODO: Content-Type
                             _cseq); // TODO: %s

  _ret = strdup(buf);

  return _ret;
}

char *RTSPParser::Set_parameter(char *rtsp){
  char buf[2048];

  snprintf(buf, sizeof(buf), "%s %s\r\n"
                             "%s\r\n"
                             "Content-Length: %s\r\n"
                             "Content-Type: %s\r\n"
                             "%s\r\n\r\n",
                             _version,
                             _code,
                             _cseq,
                             _cseq, // TODO: Content-Length
                             _cseq, // TODO: Content-Type
                             _cseq); // TODO: %s

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

char *RTSPParser::Getversion(void){
  return _version;
}

char *RTSPParser::Getcseq(void){
  return _cseq;
}

