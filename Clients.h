#ifndef SOCKET_SIMPLE_CLIENTS_H
#define SOCKET_SIMPLE_CLIENTS_H

#include <iostream>
#include <netinet/in.h>

using namespace std;

class Clients {
private:
    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[1500] = {0};

    void readAndSendMessagesToServer();

public:
    Clients(const char *host, int port);
    int connectToServer();

};


#endif //SOCKET_SIMPLE_CLIENTS_H
