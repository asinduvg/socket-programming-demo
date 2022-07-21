#include <arpa/inet.h>
#include <cstring>
#include "Clients.h"

Clients::Clients(const char *host, int port) {
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_port = htons(port);

    // convert IPv4 and IPv6 addresses from text to binary
    int address_converted = inet_pton(AF_INET, host, &this->serv_addr.sin_addr);

    if(address_converted <= 0) {
        cout << "Address not supported" << endl;
        return;
    }
}

int Clients::connectToServer() {
    // create socket
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    // connecting to server
    cout << "Trying to connect to server..." << endl;

    int client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof (serv_addr));
    if(client_fd < 0) {
        cout << "connection failed" << endl;
        return -1;
    }

    cout << "Connection with server succeeded!" << endl;

    readAndSendMessagesToServer();
}

void Clients::readAndSendMessagesToServer() {
    while (true) {
        string data;
        cout << "> ";
        getline(cin, data);
        memset(&buffer, 0, sizeof(buffer)); //clear the buffer
        strcpy(buffer, data.c_str());

        send(sock, (char*)&buffer, strlen(buffer), 0);
    }
}
