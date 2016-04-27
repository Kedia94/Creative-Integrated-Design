#pragma once

#include "RTSPServer.h"

class RTSPSession {
  public:
    RTSPSession(RTSPServer *);
    ~RTSPSession(void);

  private:
    RTSPServer *_server;
};
