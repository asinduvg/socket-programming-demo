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

    // set socket to be non-blocking
    if (ioctl(server_fd, FIONBIO, (char *) &on) < 0) {
        perror("ioctl() failed");
        close(server_fd);
        exit(-1);
    }

    // forcefully attaching socket to the port
    if (bind(this->server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(-1);
    }

    // set the backlog
    if (listen(this->server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(-1);
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
    do  {

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
                    // accept all incoming connections in the queue
                    handleNewClient();
                    // this is not the listening socket, therefore an existing connection must be readable
                } else {
                    // receive all incoming data on this socket before we loop back and call select again
                    handleExistingClient(i);
                }
            }
        }

    } while (!end_server);

    // clean up all the sockets that are open
    for(int i = 0; i <= max_sd; ++i) {
        if(FD_ISSET(i, &master_set)) close(i);
    }
}

void Serverr::handleNewClient() {
    int new_socket;

    do {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);

        if (new_socket < 0) {
            if (errno != EWOULDBLOCK) {
                perror("accept failed");
                 end_server = true;
            }
            break;
        }

        cout << "A new client connected with sd " << new_socket << endl;

        // add new socket to fd set
        FD_SET(new_socket, &master_set);

        if (new_socket > max_sd) max_sd = new_socket;

        // loop and accept another incoming connection
    } while (new_socket != -1);
}

void Serverr::handleExistingClient(int clientSD) {
    bool close_conn = false;

    // clear the buffer
    memset(&buffer, 0, sizeof(buffer));

    // read client's message
    int rc = recv(clientSD, (char *) &buffer, sizeof(buffer), 0);
    if (rc < 0) {
        if (errno != EWOULDBLOCK) {
            perror("recv() failed");
            close_conn = true;
        }
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