#include "TCPServer.h" 

string TCPServer::Message;

void* TCPServer::Task(void *arg)
{
	auto newsockfd = static_cast<int>((long)arg);
	char msg[MAXPACKETSIZE];
	pthread_detach(pthread_self());
	while(true)
	{
		int n;
		n= static_cast<int>(recv(newsockfd, msg, MAXPACKETSIZE, 0));
		if(n==0)
		{
		   close(newsockfd);
		   break;
		}
		msg[n]=0;
		//send(newsockfd,msg,n,0);
		Message = string(msg);
        }
	return nullptr;
}

void TCPServer::setup(int port)
{
	sockfd=socket(AF_INET,SOCK_STREAM,0);
 	memset(&serverAddress,0,sizeof(serverAddress));
	serverAddress.sin_family=AF_INET;
	serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddress.sin_port=htons(static_cast<uint16_t>(port));
	bind(sockfd,(struct sockaddr *)&serverAddress, sizeof(serverAddress));
 	listen(sockfd,30);
}

string TCPServer::receive()
{
	string str;
	while(true)
	{
		socklen_t sosize  = sizeof(clientAddress);
		newsockfd = accept(sockfd,(struct sockaddr*)&clientAddress,&sosize);
		str = inet_ntoa(clientAddress.sin_addr);
		pthread_create(&serverThread, nullptr,&Task,(void *)newsockfd);
	}
	return str;
}

string TCPServer::getMessage()
{
	return Message;
}

void TCPServer::Send(string msg)
{
	send(newsockfd,msg.c_str(),msg.length(),0);
}

void TCPServer::clean()
{
	Message = "";
	memset(msg, 0, MAXPACKETSIZE);
}

void TCPServer::detach()
{
	close(sockfd);
	close(newsockfd);
} 
