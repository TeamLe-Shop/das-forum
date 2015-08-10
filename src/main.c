#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include "util.h"
#include "types.h"
#include "server.h"

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
    char* config = "config.json";

    for (size_t i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-config") == 0) {
            if (argc - 1 < i + 1) {
                error("No argument provided for option 'config'\n", 1);
            }
            config = argv[i + 1];
            i++;
        }
    }


    server = Server_Open(config);

    // Set our handlers for various termination signals.
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);

    while (Server_Cycle(server)) {}

    Server_Close(server);
    return 0;
}
