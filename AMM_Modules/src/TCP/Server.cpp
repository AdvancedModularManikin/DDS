#include "Server.h"

using namespace std;

//Actually allocate clients
vector<Client> Server::clients;

Server::Server() {

  //Initialize static mutex from MyThread
  MyThread::InitMutex();

  //For setsock opt (REUSEADDR)
  int yes = 1;

  //Init serverSock and start listen()'ing
  serverSock = socket(AF_INET, SOCK_STREAM, 0);
  memset(&serverAddr, 0, sizeof(sockaddr_in));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(PORT);

  //Avoid bind error if the socket was not close()'d last time;
  setsockopt(serverSock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

  if(bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(sockaddr_in)) < 0)
    cerr << "Failed to bind";

  listen(serverSock, 5);
}

/*
	AcceptAndDispatch();

	Main loop:
		Blocks at accept(), until a new connection arrives.
		When it happens, create a new thread to handle the new client.
*/
void Server::AcceptAndDispatch() {
  
  Client *c;
  MyThread *t;

  socklen_t cliSize = sizeof(sockaddr_in);

  while(true) {

      c = new Client();
	  t = new MyThread();

	  //Blocks here;
          c->sock = accept(serverSock, (struct sockaddr *) &clientAddr, &cliSize);

	  if(c->sock < 0) {
	    cerr << "Error on accept";
	  }
	  else {
	    t->Create((void *) Server::HandleClient, c);
	  }
  }
}

void Server::SendToAll(const std::string &message) {
  ssize_t n;

  MyThread::LockMutex("'SendToAll()'");

  for (auto &client : clients) {
    n = send(client.sock, message.c_str(), strlen(message.c_str()), 0);
    // cout << n << " bytes sent." << endl;
  }

  //Release the lock
  MyThread::UnlockMutex("'SendToAll()'");
}


void Server::SendToAll(char *message) {
  ssize_t n;

  //Acquire the lock
  MyThread::LockMutex("'SendToAll()'");
 
    for (auto &client : clients) {
      n = send(client.sock, message, strlen(message), 0);
      // cout << n << " bytes sent." << endl;
    }
   
  //Release the lock  
  MyThread::UnlockMutex("'SendToAll()'");
}

void Server::SendToClient(Client *c, const std::string &message) {
  ssize_t n;
  MyThread::LockMutex("'SendToClient()'");
  int id = Server::FindClientIndex(c);
  // cout << " Sending message to [" << c->name << "](" << c->id << "): " << message << endl;
  n = send(Server::clients[id].sock, message.c_str(), strlen(message.c_str()), 0);
  // cout << n << " bytes sent." << endl;
  MyThread::UnlockMutex("'SendToClient()'");
}




void Server::ListClients() {
  for (auto &client : clients) {
    cout << client.name << endl;
  }
}

/*
  Should be called when vector<Client> clients is locked!
*/
int Server::FindClientIndex(Client *c) {
  for(size_t i=0; i<clients.size(); i++) {
    if((Server::clients[i].id) == c->id) return (int) i;
  }
  cerr << "Client id not found." << endl;
  return -1;
}
