#include "server.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>
#include <stdarg.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "types.h"
#include "util.h"

Server* Server_Open(Domain domain, int port, bool reuse_address,
                    size_t max_clients)
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

    new_server->clients = ClientList_New(max_clients);

    return new_server;
}

void Server_Close(Server* server)
{
    close(server->socket);
    free(server->clients);
    free(server);
}

bool Server_Cycle(Server* server)
{
    IPAddress newaddress;
    socklen_t newaddress_length = sizeof(IPAddress);
    memset(&newaddress, 0, sizeof(IPAddress));
    Socket newsocket = accept(server->socket, &newaddress, &newaddress_length);

    if (newsocket == -1) { // An error occured! Most likely EWOULDBLOCK.
        if (errno != EWOULDBLOCK) { // However if not, report the error.
            error("Failed to accept incoming connection", 0);
        }
    } else {
        Server_HandleConnection(server, newsocket, newaddress,
                                newaddress_length);
    }

    size_t i;
    const size_t buffer_length = 256;
    char buffer[buffer_length];
    memset(buffer, 0, buffer_length);

    char address[IP_MAXSTRLEN];

    for (i = 0; i < server->clients->size; i++) {
        int bytes = recv(server->clients->clients[i].socket, buffer,
                            buffer_length - 1, 0);
        if (bytes == 0) {
            delscreen(server->clients->clients[i].screen);
            ClientList_Disconnect(server->clients, server->clients->clients[i]);
            continue;
        } else if (bytes == -1) {
            if (errno != EAGAIN) {
                error("Failed to receive message from client", 0);
            }
        } else {
            address_to_string(server->clients->clients[i].address, address,
                              true);
            printf("Received message from [%s]: %s", address, buffer);
            Server_Broadcast(server, "[%s]: %s", address, buffer);
        }
    }

    return true;
}

void Server_HandleConnection(Server* server, Socket socket, IPAddress address,
                             socklen_t length)
{
    char addr[IP_MAXSTRLEN];
    address_to_string(address, addr, false);
    printf("Client connected [%s]\n", addr);

    Client c = {address, socket, fdopen(socket, "r+"), NULL};
    SCREEN* scr = newterm(getenv("TERM"), c.file, c.file);
    c.screen = scr;

    ClientList_Add(server->clients, c);

    // This is a test message for now.
    char* message = "Hello! Welcome to DasForum.";

    Client_Send(c, "The message of the day is... \"%s\"\n", message);
    Client_Send(c, "Users online: %d\n", server->clients->size);
}

size_t Server_MemoryUsage(Server* server)
{
    size_t total = 0;
    total += sizeof(Server);
    // Memory used by server->list structure...
    total += sizeof(ClientList);
    total += sizeof(Client) * server->clients->size;

    // Individual client memory usage
    total += sizeof(FILE); // Client's file descriptor

    return total;
}

void Server_Broadcast(Server* server, char* format, ...)
{
    va_list argument_list;
    va_start(argument_list, format);

    size_t i;

    for (i = 0; i < server->clients->size; i++) {
        Client client = server->clients->clients[i];
        if (vfprintf(client.file, format, argument_list) == -1) {
            error("Failed to send message to client", 0);
        }
        fflush(client.file);
    }

    va_end(argument_list);
}
