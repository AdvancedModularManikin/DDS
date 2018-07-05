#ifndef _client_h_
#define _client_h_

#include "stdafx.h"

#define MAX_NAME_LENGTH 20

class Client {
public:
    unsigned long id{};
    std::string name;
    std::string uuid;
    bool keepHistory = false;

    //Socket stuff
    int sock{};

    Client() {};

    void SetId(unsigned long id);

    void SetName(std::string &name);

    void SetUUID(std::string &uuid);

    void SetKeepHistory(bool historyflag);
};

#endif
