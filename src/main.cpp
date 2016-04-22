#include "main.h"

int main(int argc, char **argv){

  printf("\n");
  printf("Initializing the server...\n");
  printf("\n");
  RTSPServer* rtspserver = NULL;
  int port = 554;

  rtspserver = rtspserver->Create(port);

  if (rtspserver == NULL) {
    port = 8554;
    rtspserver = rtspserver->Create(port);
  }

  if (rtspserver == NULL) {
    printf("Cannot create RTSP server\n");
    return -1;
  }
  
  printf("Welcome to H.265 Streaming Server\n");
  printf("You can use following url to stream a video\n");
  printf("   %s<filename>\n", rtspserver->GetRTSPurl());
  
  return 0;
}

