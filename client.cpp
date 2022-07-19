#include<arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/unistd.h>
#include <iostream>

#define PORT 8080

using namespace std;

int main(int argc, char const* argv[]) {
    int sock = 0, client_fd;
    struct sockaddr_in serv_addr{};
    char buffer[1500] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    int address_converted = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if(address_converted <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // connect to server
    cout << "Trying to connect to server.." << endl;

    int i = 0;
    while(true){
        client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if(client_fd < 0) {
            printf("\nConnection Failed \n");

            if(i == 10) return -1;
            else {
                printf("Retrying ...\n");
                i++;
                continue;
            }
        }
        break;
    }

    cout << "Connection with server succeeded!" << endl;

    while(true) {
        string data;
        cout << "> ";
        getline(cin, data);
        memset(&buffer, 0, sizeof(buffer)); // clear the buffer
        strcpy(buffer, data.c_str());

        send(sock, (char*)&buffer, strlen(buffer), 0);

        cout << "Awaiting server response..." << endl;

        memset(&buffer, 0, sizeof(buffer)); // clear the buffer

        // reading the server message to the buffer
        recv(sock, (char*)&buffer, sizeof(buffer), 0);

        cout << buffer << endl;

    }

    // closing the connected socket
    close(client_fd);
    cout << "Connection closed!" << endl;
    return 0;

}