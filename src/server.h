#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>

#include <stdbool.h>

#include "types.h"

struct Server {
    Socket socket;
    Domain domain;
};

typedef struct Server Server;

/*
 * Creates a new Server object, and opens a new socket
 * with domain `domain`, and on port `port`.
 * It will reuse the address if `reuse_address` is set to true.
 * The file descriptor this is performed on will be
 * owned by the Server object.
 */
Server* Server_Open(Domain domain, int port, bool reuse_address);

/*
 * Closes the socket file descriptor owned by the `server`
 * object, and frees any memory used by it.
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

#endif /* SERVER_H */
