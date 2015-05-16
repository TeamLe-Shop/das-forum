#include "client.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "util.h"

ClientList* ClientList_New(size_t max_clients)
{
    ClientList* list = malloc(sizeof(ClientList));
    list->clients = NULL;
    list->size = 0;
    list->max_clients = max_clients;

    return list;
}

void ClientList_Destroy(ClientList* list)
{
    size_t i;
    // Close all open sockets
    for (i = 0; i < list->size; i++) {
        ClientList_Disconnect(list, list->clients[i]);
    }
    free(list->clients);
    free(list);
}

bool ClientList_Add(ClientList* list, Client client)
{
    if (list->size >= list->max_clients) {
        return false;
    }

    list->clients = realloc(list->clients, sizeof(Client) * (list->size + 1));
    list->clients[list->size] = client;
    list->size++;

    return true;
}

void ClientList_Disconnect(ClientList* list, Client client)
{
    char addr[IP_MAXSTRLEN];
    address_to_string(client.address, addr, false);
    printf("Client disconnected [%s]\n", addr);

    ssize_t index;
    if ((index = ClientList_GetIndex(list, client)) != -1) {
        list->size--;
        memmove(&client, &list->clients[index + 1],
                sizeof(Client) * (list->size - index));
        if (list->size > 0) {
            list->clients = realloc(list->clients, sizeof(Client) * list->size);
        } else {
            free(list->clients);
            list->clients = NULL;
        }
    } else {
        fprintf(stderr, "Failed to disconnect client\n");
    }

    fclose(client.file);
    close(client.socket);
}

ssize_t ClientList_GetIndex(ClientList* list, Client client)
{
    size_t i;
    for (i = 0; i < list->size; i++) {
        // Here we just search by socket; we could probably use other
        // unique variables. Memory addresses wouldn't work as conveniently.
        // See server.c: `Server_HandleConnection`
        if (list->clients[i].socket == client.socket) {
            return i;
        }
    }
    fprintf(stderr, "ClientList_GetIndex: `client` is not in `list`!\n");
    return -1;
}

void Client_Send(Client client, char* format, ...)
{
    va_list argument_list;
    va_start(argument_list, format);


    if (vfprintf(client.file, format, argument_list) == -1) {
        error("Failed to send message to client", 0);
    }
    fflush(client.file);

    va_end(argument_list);
}
