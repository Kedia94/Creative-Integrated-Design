#include "testServer.h"
#include <unistd.h>

int main(){
	printf(" port? : ");
	int port;
	scanf("%d",&port);
	printf(" how many balancee? : ");
	int n;
	scanf("%d",&n);
	Server server;
	server.makeNConnection(port, n);
	printf("Connection established!\n");
	while(true){
		printf("Doing some other important things...\n");
		sleep(10);
		server.updateBandwidth();
		sleep(1);
	}
}
