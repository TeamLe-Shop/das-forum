#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "types.h"

struct Client {
    IPAddress address;
    Socket socket;
    FILE* file; // We're using a FILE* here so we can just send ncurses
                // window data down the socket with ease.
};

typedef struct Client Client;

struct ClientList {
    Client* clients;
    size_t size;
    size_t max_clients;
};

typedef struct ClientList ClientList;

/*
 * Create and allocate memory for a new ClientList.
 * The maximum amount of clients it can contain is
 * specified by `max_clients`.
 */
ClientList* ClientList_New(size_t max_clients);

/*
 * Free `list` and any memory used by the ClientList.
 */
void ClientList_Destroy(ClientList* list);

/*
 * Add `client` to `list`.
 * If the new size of the ClientList exceeds the maximum
 * number of clients, this function returns false
 * and the client is not added.
 */
bool ClientList_Add(ClientList* list, Client client);

/*
 * Remove `client` from `list`, and shrink it down to appropriate size.
 * Connections associated with that client are closed.
 */
void ClientList_Disconnect(ClientList* list, Client client);

/*
 * Get the index of `client` in `list`.
 */
ssize_t ClientList_GetIndex(ClientList* list, Client client);

/*
 * Sends a formatted message to `client` and flushes the stream.
 */
void Client_Send(Client client, char* format, ...);

#endif /* CLIENT_H */
