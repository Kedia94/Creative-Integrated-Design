#include "Client.h"

Client::Client(){
	_bandwidth = 0;
}

void Client::makeConnection(const char *server_ip, const char *server_port){
	int client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_sock < 0){
		perror("ERROR opening client socket");
		return;
	}

	struct sockaddr_in server_addr;
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( atoi(server_port) );
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("ERROR connecting client socket to server socket");
		return;
	}
	printf("socket connected\n");

	_client_sock = client_sock;
}

void Client::reportBandwidth(){
	printf("current bandwidth : %d\n",_bandwidth);

	char buffer[PACKET_SIZE];
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", _bandwidth);

	int send_n = send(_client_sock, buffer, sizeof(buffer), 0);
	if ( send_n < 0 ){
		perror("ERROR sending to server socker");
		return;
	}
}

void Client::setBandwidth(int bd){
	_bandwidth = bd;
}

void Client::addBandwidth(int delta){
	_bandwidth += delta;
}
