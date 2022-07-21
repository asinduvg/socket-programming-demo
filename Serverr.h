#ifndef SOCKET_SIMPLE_SERVERR_H
#define SOCKET_SIMPLE_SERVERR_H

#include <iostream>
#include <netinet/in.h>
#include <vector>

using namespace std;

class Serverr {
private:
    struct sockaddr_in address{};
    struct timeval timeout;
    int addrlen;
    fd_set master_set, working_set; //set of socket descriptors
    fd_set writefds;
    int server_fd;
    int max_sd, desc_ready;
    int on = 1;
    bool end_server = false;
    vector<int> client_sockets;
    char buffer[1500] = {0};

    void handleNewClient();
    void handleExistingClient(int clientSD);

public:
    Serverr(int port);
    bool createSocket();
    void waitForClient();
};


#endif //SOCKET_SIMPLE_SERVERR_H
