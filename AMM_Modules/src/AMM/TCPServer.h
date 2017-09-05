#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

#define MAXPACKETSIZE 4096

class TCPServer
{
	public:
	int sockfd, newsockfd, n, pid;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
	pthread_t serverThread;
	char msg[ MAXPACKETSIZE ];
	static string Message;

	void setup(int port);
	string receive();
	string getMessage();
	void Send(string msg);
	void detach();
	void clean();

	private:
	static void * Task(void * argv);
};
