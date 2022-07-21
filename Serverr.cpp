#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include "Serverr.h"

Serverr::Serverr(int port) {
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(port);

    this->timeout.tv_sec = 3 * 60;
    this->timeout.tv_usec = 0;
}

bool Serverr::createSocket() {
    // socket returns a descriptor referencing new socket
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->server_fd == 0) {
        perror("socket failed");
//        exit(EXIT_FAILURE);
        return false;
    }

    // set socket to be nonblocking
    if (ioctl(server_fd, FIONBIO, (char *) &on) < 0) {
        perror("ioctl() failed");
        close(server_fd);
        exit(-1);
    }

    // forcefully attaching socket to the port
    if (bind(this->server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
//        exit(EXIT_FAILURE);
        return false;
    }

    // set the backlog
    if (listen(this->server_fd, 3) < 0) {
        perror("listen");
        return false;
    }

    cout << "Awaiting for a client request..." << endl;

    return true;

}

void Serverr::waitForClient() {

    // clear the socket set
    FD_ZERO(&master_set);

    // add master socket to set
    FD_SET(server_fd, &master_set);
    max_sd = server_fd;

    // infinitely listening for activities
    while (true) {

        // copy the master fd_set to the working fd_set
        memcpy(&working_set, &master_set, sizeof(master_set));

        int select_return = select(max_sd + 1, &working_set, nullptr, nullptr, &timeout);

        if (select_return < 0) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        // check to see if the select call failed
        if (select_return == 0) {
            cout << "select() timed out. End program" << endl;
            break;
        }

        // one or more descriptors are readable, need to determine which ones they are
        desc_ready = select_return;

        for (int i = 0; i <= max_sd && desc_ready > 0; ++i) {
            if (FD_ISSET(i, &working_set)) {
                desc_ready = -1;

                if (i == server_fd) {
                    cout << "Server is readable" << endl;
                    // accept all incoming connections in the queue
                    handleNewClient();
                    // this is not the listening socket, therefore an existing connection must be readable
                } else {
                    // receive all incoming data on this socket before we loop back and call select again
                    handleExistingClient(i);
                }

            }
        }

//        if(FD_ISSET(4, &master_set)) {
//            puts("first client ok");
//        }

        // checking whether server socket is set and if so, create a new socket
        // for new incoming client
        // If something
//        if (FD_ISSET(this->server_fd, &this->master_set)) this->handleNewClient();
//        // else looking for existing client connection
//        this->handleExistingClient();

    }
}

void Serverr::handleNewClient() {
    int new_socket;

    do {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);

        if (new_socket < 0) {
            if (errno != EWOULDBLOCK) {
                perror("accept failed");
                // end server = true
            }
//            exit(EXIT_FAILURE);
            break;
        }

        cout << "A new client connected with sd " << new_socket << endl;

        // add new socket to vector of sockets
//        client_sockets.push_back(new_socket);

        // add new socket to fd set
        FD_SET(new_socket, &master_set);

        if (new_socket > max_sd) max_sd = new_socket;

        // loop and accept another incoming connection
        cout << "inside new client " << endl;
    } while (new_socket != -1);


//    readAndSendMessagesClient(new_socket);

}

void Serverr::handleExistingClient(int clientSD) {
    bool close_conn = false;

    // clear the buffer
    memset(&this->buffer, 0, sizeof(this->buffer));

    // read client's message
    int rc = recv(clientSD, (char *) &buffer, sizeof(this->buffer), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("recv() failed");
        }
        return;
    }

    // check whether the client closed the connection
    if(rc == 0) {
        cout << "Client " << clientSD << " disconnected!" << endl;
        close_conn = true;
    }

    if(close_conn) {
        close(clientSD);
        FD_CLR(clientSD, &master_set);
        if(clientSD == max_sd) {
            while (FD_ISSET(max_sd, &master_set) == false) {
                max_sd -= 1;
            }
        }
    } else
        printf("Client %d says: %s\n", clientSD, buffer);
}