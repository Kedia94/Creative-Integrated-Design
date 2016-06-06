#include "Server.h"

Server::Server(){}

void Server::makeNConnection(int startport, int n){
	printf("Start making connections for loadbalancing\n");
	for(int i = 0; i < n; i++){
		make1Connection(startport+i*2);
	}
}

void Server::make1Connection(int port){
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0){
		perror("ERROR opening server socket");
		return;
	}

	struct sockaddr_in server_addr;
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(server_sock, (struct sockaddr *) &server_addr, (socklen_t)sizeof(server_addr)) < 0){
		perror("ERROR binding server socket");
		return;
	}

	listen(server_sock, 10);
	printMyAddress(server_addr);

	struct sockaddr_in client_addr;
	int c = sizeof(client_addr);
	int client_sock;
	client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&c);
	if (client_sock < 0){
		perror("ERROR on accept");
		return;
	}
	fcntl(client_sock, F_SETFL, O_NONBLOCK);
	_client_sock.push_back(client_sock);
	_bandwidth.push_back(0);
}

void Server::updateBandwidth(){
	bool changed = false;
	for(int i = 0; i < _client_sock.size(); i++){
		int bd = readAll(_client_sock[i]);
		if (bd != -1){
			_bandwidth[i] = bd;
			changed = true;
		}
	}
	if (changed){
		printBandwidth();
	}
}

void Server::printBandwidth(){
	for(int i = 0; i < _bandwidth.size(); i++){
		printf("%d ",_bandwidth[i]);
	}
	printf("\n");
}

int Server::readAll(int client_sock){
	int ret = -1;
	int read_n = readOne(client_sock);
	while( read_n != -1 ){
		ret = read_n;
		read_n = readOne(client_sock);
	}
	return ret;
}

int Server::readOne(int client_sock){
	char buffer[PACKET_SIZE];
	int read_n = recv(client_sock, buffer, PACKET_SIZE, 0);
	if (read_n < 0)
		return -1;
	while(read_n != PACKET_SIZE){
		int delta = recv(client_sock, buffer+read_n, PACKET_SIZE - read_n, 0);
		if (delta == -1)
			continue;
		read_n += delta;
	}
	return atoi(buffer);
}

void Server::printMyAddress(struct sockaddr_in addr){
	printEth0Ip();
	printf("port : %d\n",ntohs(addr.sin_port));
}

void Server::printEth0Ip(){
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
	  printf("ip : %s\n",inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

int Server::selectServer(){
	int min = 1000000000, min_idx;
	for(int i = 0; i < _bandwidth.size(); i++){
		if ( min > _bandwidth[i] ){
			min = _bandwidth[i];
			min_idx = i;
		}
	}
	return min_idx;
}
