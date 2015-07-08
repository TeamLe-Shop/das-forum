#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/socket.h>

#include "util.h"
#include "types.h"
#include "server.h"

#define DEFAULT_PORT 7778

// We're using a global here to simplify the signal handler.
Server* server;

void signal_handler(int signal)
{
    if (server != NULL) {
        Server_Close(server);
        exit(0);
    }
}

int main(int argc, char* argv[])
{
    // These variables should be loaded from configuration.
    int port = DEFAULT_PORT;
    Domain domain = PF_INET6;
    bool reuse_address = true;

    server = Server_Open(domain, port, reuse_address, 8);

    // Set our handlers for various termination signals.
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    while (Server_Cycle(server)) {}

    Server_Close(server);
    return 0;
}
