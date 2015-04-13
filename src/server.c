#include "server.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "types.h"
#include "util.h"

Server* Server_Open(Domain domain, int port, bool reuse_address)
{
    Server* new_server = malloc(sizeof(Server));
    Socket sock = socket(domain, SOCK_STREAM, 0);

    if (sock == -1) {
        error("Failed to open server socket", 1);
    }

    new_server->socket = sock;
    new_server->domain = domain;


    // NOTE: Whether the address is reusable or not should be specified in
    //       configuration.

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_address, sizeof(int))
        == -1) {
        error("Failed to set socket option SO_REUSEADDR on socket", 1);
    }

    // Set up address
    int result;

    if (new_server->domain == PF_INET) { // IPv4
        IP4Address ip4addr;
        memset(&ip4addr, 0, sizeof(IP4Address));

        ip4addr.sin_family = AF_INET;
        ip4addr.sin_port = htons(port);
        ip4addr.sin_addr.s_addr = htonl(INADDR_ANY);

        result = bind(new_server->socket, (Address*)&ip4addr,
                      sizeof(IP4Address));
    } else if (new_server->domain == PF_INET6) { // IPv6
        IP6Address ip6addr;
        memset(&ip6addr, 0, sizeof(IP6Address));

        ip6addr.sin6_family = AF_INET6;
        ip6addr.sin6_port = htons(port);
        ip6addr.sin6_addr = in6addr_any;

        result = bind(new_server->socket, (Address*)&ip6addr,
                      sizeof(IP6Address));
    }

    if (result == -1) {
        error("Failed to bind server socket to requested address", 1);
    }

    fcntl(new_server->socket, F_SETFL, O_NONBLOCK);

    // NOTE: Backlog should be specified in configuration.

    listen(new_server->socket, SOMAXCONN);

    return new_server;
}

void Server_Close(Server* server)
{
    close(server->socket);
    free(server);
}

bool Server_Exec(Server* server)
{
    IPAddress newaddress;
    socklen_t newaddress_length;
    Socket newsocket = accept(server->socket, &newaddress, &newaddress_length);

    if (newsocket == -1) { // An error occured! Most likely EWOULDBLOCK.
        if (errno != EWOULDBLOCK) { // However if not, report the error.
            error("Failed to accept incoming connection", 0);
        }
    } else {
        Server_HandleConnection(server, newsocket, newaddress,
                                newaddress_length);
    }
    return true;
}

void Server_HandleConnection(Server* server, Socket socket, IPAddress addr,
                             socklen_t length)
{
    printf("Client connected! ");
    if (addr.ss_family == AF_INET) {
        printf("(IPv4)");
    } else if (addr.ss_family == AF_INET6) {
        printf("(IPv6)");
    } else {
        printf("(Unknown address family!)");
    }
    printf("\n");
    // This is a test message for now.
    char* message = "Hello! Welcome to DasForum.\n";
    if (send(socket, message, strlen(message), 0) == -1) {
        error("Failed to send message to client", 0);
    }
    close(socket);
}
