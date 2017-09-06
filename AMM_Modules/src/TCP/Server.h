#ifndef _server_h_
#define _server_h_

#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>

#include "ServerThread.h"
#include "Client.h"

using namespace std;

class Server {

  private:
    static vector<Client> clients;
    int serverSock;
    struct sockaddr_in serverAddr, clientAddr;

  public:
    explicit Server(uint16_t port);
    void AcceptAndDispatch();
    static void * HandleClient(void *args);

    static void SendToAll(const std::string &message);
    static void SendToClient(Client *c, const std::string &message);

  private:
    static void ListClients();
    static void SendToAll(char *message);
    static int FindClientIndex(Client *c);

protected:
    bool m_runThread;
};

#endif
