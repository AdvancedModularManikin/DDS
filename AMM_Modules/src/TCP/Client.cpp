#include "Client.h"

using namespace std;

void Client::SetName(std::string &name) {
  if (name.size() > MAX_NAME_LENGTH) {
    name.resize(MAX_NAME_LENGTH);
  }
  this->name = name;
}

void Client::SetId(int id) {
  this->id = id;
}
