#pragma once

#include <WS2tcpip.h>


#pragma comment (lib, "ws2_32.lib")

class TcpListener {

public:

protected:

private:
   const char* m_ipAddress;
   int         m_port;
   int         m_socket;

   /// Master file dscriotior set.
   fd_set      m_master;

public:

   TcpListener (const char* ipAddress, int port) :
      m_ipAddress(ipAddress), m_port(port) {}

   int Init ();
   int Run ();

protected:
   virtual void OnClientConnected (int clientSocket);

   virtual void OnClientDisconnected (int clientSocket);

   virtual void OnMessageReceived (int clientSocket, const char* msg, int length);

   void SendToClient (int clientSocket, const char* msg, int length);

   void BroadcastToClients (int sendingClient, const char* msg, int length);

};
