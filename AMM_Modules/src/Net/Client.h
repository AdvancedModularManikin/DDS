#ifndef _client_h_
#define _client_h_

#include "stdafx.h"

#define MAX_NAME_LENGTH 20

using namespace std;

class Client {
  public:
    string name;
    unsigned long id{};
    bool keepHistory = false;

    //Socket stuff
    int sock{};


    Client() {};
    void SetName(std::string &name);
    void SetId(unsigned long id);
    void SetKeepHistory(bool historyflag);
};

#endif
