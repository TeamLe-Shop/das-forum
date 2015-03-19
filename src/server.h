#ifndef SERVER_H
#define SERVER_H

#include "types.h"

struct Server {
    Socket socket;
};

typedef struct Server Server;

/*
 * Creates a new Server object, and opens a new socket
 * with domain `domain`, and on port `port`.
 * The file descriptor this is performed on will be
 * owned by the Server object.
 */
Server* Server_Open(Domain domain, int port);

/*
 * Closes the socket file descriptor owned by the `server`
 * object.
 */
void Server_Close(Server* server);

#endif /* SERVER_H */
