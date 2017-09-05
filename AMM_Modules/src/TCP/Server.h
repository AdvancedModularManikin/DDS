#ifndef _server_h_
#define _server_h_

#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>

#include "MyThread.h"
#include "Client.h"

#define PORT 9015

using namespace std;

class Server {

  private:
    static vector<Client> clients;

    //Socket stuff
    int serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    char buff[256];

  public:
    Server();
    void AcceptAndDispatch();
    static void * HandleClient(void *args);
    static void * SmandleClient(void *args);
    static void SendToAll(const std::string &message);
    static void SendToClient(Client *c, const std::string &message);

  private:
    static void ListClients();
    static void SendToAll(char *message);
    static int FindClientIndex(Client *c); 
};

#endif
