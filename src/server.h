#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>

#include <stdbool.h>
#include <stdarg.h>

#include "types.h"
#include "client.h"
#include "config.h"

struct Server {
    Socket socket;
    Domain domain;
    ClientList* clients;
    Config* config;
    char* motd;
};

typedef struct Server Server;

/*
 * Creates a new Server object, and uses configuration variables from the file
 * `config_file`.
 */
Server* Server_Open(char* config_file);

/*
 * Closes the socket file descriptor owned by the `server`  object, and frees
 * any memory used by it.
 */
void Server_Close(Server* server);

/*
 * Runs a cycle of the server loop.
 * This accepts incoming connections and receives data from connected clients.
 * Returns whether the server has shut down or not.
 */
bool Server_Cycle(Server* server);

/*
 * Adds a new client to the client list using the connection information passed
 * in.
 */
void Server_HandleConnection(Server* server, Socket socket, IPAddress address,
                             socklen_t length);

void Server_HandleReceivedMessage(Server* server, char* buffer,
                                  size_t buffer_length, Client* client);

/*
 * Broadcast a formatted message to all connected clients.
 */
void Server_Broadcast(Server* server, char* format, ...);

#endif /* SERVER_H */
