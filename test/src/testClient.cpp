#include "testClient.h"

int main(int argc, char ** argv){
	printf(" ./test <ip> <port>\n");
	if (argc < 3)
		return 0;
	Client client;
	client.makeConnection((const char*)argv[1], (const char*)argv[2]);
	printf("connection established\n");
	while(true){
		int bd;
		scanf("\n%d",&bd);
		client.setBandwidth(bd);
		client.reportBandwidth();
		printf("reported\n");
	}
}
