#pragma once
#include <sstream>
#include "TcpListener.hxx"

class WebServer : public TcpListener {

   public:

   public:
      WebServer (const char* ipAddr, int port) :
         TcpListener(ipAddr, port) {}

   protected:
      virtual void OnClientConnected (int clientSocket);

      virtual void OnClientDisconnected (int clientSocket);

      virtual void OnMessageReceived (int clientSocket, const char* msg, int length);

};
