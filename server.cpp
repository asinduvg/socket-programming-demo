#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/unistd.h>
#include <cstring>
#include <sys/ioctl.h>

#define PORT 8080

using namespace std;

int mains() {

    int server_fd, new_socket, rc, on = 1;
    struct sockaddr_in address{};
    int addrlen = sizeof(address);
    int max_sd, max_clients = 30, sd, activity, valread;
    int client_socket[30];
    char buffer[1500] = {0};

    // set of socket descriptors
    fd_set readfds;

    // initialize all client_socket[] to be 0, so not checked
    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // socket returns a descriptor referencing new socket

    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

//    rc = ioctl(server_fd, FIONBIO, (char *) &on);
//    if (rc < 0) {
//        perror("ioctl() failed");
//        close(server_fd);
//        exit(-1);
//    }

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Awaiting for a client request..." << endl;

    while (true) {

        puts("hi inside while loop");

        // clear the socket set
        FD_ZERO(&readfds);

        // add master socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // add child sockets to set
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            // if valid socket descriptor, then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            // highest file descriptor number, need it for the select function
            if (sd > max_sd) max_sd = sd;
        }

        // wait for an activity on one of the sockets, timeout is NULL,
        // so wait indefinitely
        activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);

        if ((activity < 0) && (errno != EINTR)) printf("select error");

        // If something happened on the master socket,
        // then its an incoming connection

        if (FD_ISSET(server_fd, &readfds)) {
            puts("inside isset");
            new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);

            if (new_socket < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd - %d\n", new_socket);

            // add new socket to array of sockets
            for (int i = 0; i < max_clients; i++) {
                // if position is empty
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }

//            memset(&buffer, 0, sizeof(buffer)); // clear the buffer
//            // read client's message
//            recv(new_socket, (char *) &buffer, sizeof(buffer), 0);
//            printf("Client %d says: %s\n", new_socket, buffer);
//
//            string str(buffer);
//            string response = "Server response to " + str;
//
//            send(new_socket, response.c_str(), strlen(response.c_str()), 0);

        }

        puts("----------");

        // else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            cout << sd << endl;

            if (FD_ISSET(sd, &readfds)) {

                puts("some io operation");

                memset(&buffer, 0, sizeof(buffer)); // clear the buffer
                recv(sd, (char *) &buffer, sizeof(buffer), 0);
                printf("Client %d says: %s\n", sd, buffer);

                string str(buffer);
                string response = "Server response to " + str;

                send(sd, response.c_str(), strlen(response.c_str()), 0);

                break;

                // check if it was for closing, and also read the
                // incoming message
//                if ((valread = read(sd, buffer, 1024)) == 0) {
//                    // somebody disconnected, get his details and print
//                    getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
//                    printf("Host disconnected");
//
//                    // close the socket and mark as 0 in list for reuse
//                    close(sd);
//                    client_socket[i] = 0;
//                }

                // echo back the message that came in
//                else {
                // set the string termination NULL byte on the end
                // of the data read
//                    buffer[valread] = '\0';
//                    send(sd, buffer, strlen(buffer), 0);
//                }
            }
        }

    }
}
