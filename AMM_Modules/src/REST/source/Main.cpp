
#include "REST/headers/WebServer.hxx"
#include <iostream>

void main () {

   std::cout << "hi" << std::endl;

   WebServer webServer("0.0.0.0", 8080);
   if (webServer.Init() != 0) { return; }

   webServer.Run();


   system("pause");
}
