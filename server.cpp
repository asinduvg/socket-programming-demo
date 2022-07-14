#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/unistd.h>
#include <cstring>

#define PORT 8080

using namespace std;

int main() {

    int server_fd, new_socket;
    struct sockaddr_in address{};
    int addrlen = sizeof(address);
    char buffer[1500] = {0};

    // creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket returns a descriptor referencing new socket

    if(server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Awaiting for a client request..." << endl;

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

    if(new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cout << "Client connected!" << endl;

    while(true) {
        cout << "Awaiting client response" << endl;
        memset(&buffer, 0, sizeof(buffer)); // clear the buffer

        // read client's message
        recv(new_socket , (char*)&buffer, sizeof(buffer), 0);

        if(!strcmp(buffer, "quit")) {
            cout << "Client has quit the session!" << endl;
            break;
        }

        cout << "Client says: " << buffer << endl;

        string data;
        cout << "> ";
        getline(cin, data);
        strcpy(buffer, data.c_str());

        if(data == "quit") {
            send(new_socket, (char*)&buffer, strlen(data.c_str()), 0);
            break;
        }

        send(new_socket, (char*)&buffer, strlen(data.c_str()), 0);
    }

    // closing the connected socket
    close(new_socket);

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);

    return 0;
}
