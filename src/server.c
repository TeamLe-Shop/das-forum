#include "server.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>
#include <stdarg.h>
#include <stdbool.h>

#include <arpa/inet.h>

#include <sys/socket.h>

#include "types.h"
#include "util.h"

Server* Server_Open(char* configfile)
{
    Server* new_server = malloc(sizeof(Server));

    // Configuration variables
    int port;
    Domain domain = PF_INET6;
    bool reuse_address;
    size_t max_clients;
    int backlog;

    new_server->config = Config_New(configfile);

    if (new_server->config) {
        port = Config_GetInteger(new_server->config, "port", 7778);
        reuse_address = Config_GetInteger(new_server->config, "reuse_address",
                                          true);
        max_clients = Config_GetInteger(new_server->config, "max_clients", 10);
        backlog = Config_GetInteger(new_server->config, "backlog", SOMAXCONN);
        new_server->motd = Config_GetString(new_server->config, "motd",
                                            "Welcome!");
    }

    Socket sock = socket(domain, SOCK_STREAM, 0);

    if (sock == -1) {
        error("Failed to open server socket", 1);
    }

    new_server->socket = sock;
    new_server->domain = domain;

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

    listen(new_server->socket, backlog);

    new_server->clients = ClientList_New(max_clients);

    return new_server;
}

void Server_Close(Server* server)
{
    if (server->config) {
        Config_Destroy(server->config);
    }
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

    for (i = 0; i < server->clients->clients->size; i++) {
        Client* current_client = &server->clients->clients->list[i];
        int bytes = recv(current_client->socket, buffer, buffer_length - 1, 0);
        if (bytes == 0) {
            delscreen(current_client->screen);
            ClientList_Disconnect(server->clients, *current_client);
            continue;
        } else if (bytes == -1) {
            if (errno != EAGAIN) {
                error("Failed to receive message from client", 0);
            }
        } else {
            Server_HandleReceivedMessage(server, buffer, buffer_length,
                                         current_client);

        }
    }

    return true;
}

void Server_HandleReceivedMessage(Server* server, char* buffer,
                                  size_t buffer_length, Client* client)
{
    char address[IP_MAXSTRLEN];

    if (!client->terminal_given) {
        client->screen = newterm(buffer, client->file, client->file);
        //set_term(client->screen);
        //clear();
        //refresh();
        //fflush(client->file);
        Client_Send(*client, "Thank you!\n");
        client->terminal_given = true;
        return;
    }

    address_to_string(client->address, address, false);
    printf("Received message from [%s]: %s", address, buffer);
    Server_Broadcast(server, "[%s]: %s", address, buffer);
}

void Server_HandleConnection(Server* server, Socket socket, IPAddress address,
                             socklen_t length)
{
    char addr[IP_MAXSTRLEN];
    address_to_string(address, addr, false);
    printf("Client connected [%s]\n", addr);

    Client c = Client_New(socket, address);

    ClientList_Add(server->clients, c);

    Client_Send(c, "Enter your terminal type (e.g. xterm-256color): ");
}

void Server_Broadcast(Server* server, char* format, ...)
{
    va_list argument_list;
    va_start(argument_list, format);

    size_t i;

    for (i = 0; i < server->clients->clients->size; i++) {
        Client client = server->clients->clients->list[i];
        Client_Send_VA(client, format, argument_list);
    }

    va_end(argument_list);
}
