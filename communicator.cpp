#include <cstring>
#include "Serverr.h"
#include "Clients.h"

int main(int argc, char **argv) {

    if (argc < 2) {
        perror("At least one argument needed!");
        return -1;
    }

    if (argc == 2) {
        const char *arg = argv[1];
        if (!strcmp(arg, "server")) {
            Serverr serv = Serverr(8080);
            serv.createSocket();
            serv.waitForClient();
        } else {
            perror("Argument not matched!");
            return -1;
        }
    } else if (argc == 4) {
        const char *arg1 = argv[1];
        const char *host = argv[2];
        const int port = atoi(argv[3]);

        if (!strcmp(arg1, "client")) {
            Clients cl = Clients(host, port);
            cl.connectToServer();
        } else {
            perror("Argument not matched!");
            return -1;
        }

    } else {
        perror("Arguments count not matched!");
        return -1;
    }
}