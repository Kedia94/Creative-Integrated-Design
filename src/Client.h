#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PACKET_SIZE 10

class Client{
	public:
		Client();
		void makeConnection(const char *,const char *);
		void reportBandwidth();
		void setBandwidth(int);
		void addBandwidth(int);

	private:
		int _client_sock;
		int _bandwidth;
};

