#include "main.h"

void usage() {
  printf("<Servernum> <Server1-ip> <Server1-port> <Server2-ip> <Server2-port> ...\n");
}

int main(int argc, char **argv){

	if (argc < 4){
		usage();
		return 0;
	}
	int servernum = atoi(argv[1]);
	if (servernum <= 0){
		usage();
		return 0;
	}
	if (servernum >= MAX_SERVER){
		printf("maximum server number exceeded. maximum is : %d\n",MAX_SERVER);
		return 0;
	}
	if (servernum != (argc - 2)/2){
		printf("Are you sure there's %d servers?\n",servernum);
		return 0;
	}
	char *server_ip[MAX_SERVER], *server_port[MAX_SERVER];
	for(int i = 0; i < servernum; i++){
		server_ip[i] = argv[2+i*2];
		server_port[i] = argv[3+i*2];
	}

  RTSPServer* rtspserver = NULL;
  int port = 554;

  rtspserver = rtspserver->Create(port);

  for (port = 8554; (rtspserver == NULL) && (port < 8754); port += 2) {
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


  printf("Accepting... by method Loadbalance\n");
  rtspserver->Loadbalance(servernum, server_ip, server_port);
  return 0;
}

