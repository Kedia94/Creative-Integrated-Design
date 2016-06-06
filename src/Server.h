#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#define PACKET_SIZE 10
using namespace std;

class Server{
	public:
		Server();
		void makeNConnection(int startport, int n);
		void make1Connection(int port);
		void updateBandwidth();
		void printBandwidth();
		int selectServer();

	private:
		int readAll(int);
		int readOne(int);
		void printMyAddress(struct sockaddr_in);
		void printEth0Ip();
		vector<int> _client_sock;
		vector<int> _bandwidth;
};
