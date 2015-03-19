#include "server.h"

#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>

#include "types.h"
#include "util.h"

Server* Server_Open(Domain domain, int port)
{
    Server* new_server = malloc(sizeof(Server));
    Socket sock = socket(domain, SOCK_STREAM, 0);

    if (sock == -1) {
        error("Failed to open server socket");
        exit(1);
    }

    new_server->socket = sock;

    // TODO: Bind socket to address & listen on socket

    return new_server;
}

void Server_Close(Server* server)
{
    close(server->socket);
    free(server);
}
