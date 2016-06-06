#include "main.h"

int main(int argc, char **argv){

  RTSPServer* rtspserver = NULL;
  int port = 554;

  rtspserver = rtspserver->Create(port);

  for (port = 8554; (rtspserver == NULL) && (port <= 8754); port += 2) {
    rtspserver = rtspserver->Create(port);
  }

  if (rtspserver == NULL) {
    printf("Cannot create RTSP server\n");
    return -1;
  }
  
  printf("Welcome to H.265 Streaming Server\n");
  printf("You can use following url to stream a video\n");
  printf("    %s<filename>\n", rtspserver->Geturl());
  printf("        We support .ts file with AVC, HEVC codec.\n");
  printf("		  No Loadbalancing : ./server\n");
  printf("		  Yes Loadbalancing : ./server <loadbalancer-ip> <loadbalancer-port>\n");
	
  if ( argc == 3)
	  rtspserver->Makeclient(argv[1],argv[2]);

  printf("Accepting...\n");
  rtspserver->Accept();
  return 0;
}

