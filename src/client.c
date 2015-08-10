#include "client.h"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <curses.h>

#include <arpa/inet.h>

#include "util.h"

ClientList* ClientList_New(size_t max_clients)
{
    ClientList* list = malloc(sizeof(ClientList));
    list->clients = ListInit(list->clients, NULL, NULL);
    list->max_clients = max_clients;

    return list;
}

void ClientList_Destroy(ClientList* list)
{
    size_t i;
    // Close all open sockets
    for (i = 0; i < list->clients->size; i++) {
        ClientList_Disconnect(list, list->clients->list[i]);
    }
    List_Destroy(list->clients);
    free(list);
}

bool ClientList_Add(ClientList* list, Client client)
{
    if (list->clients->size >= list->max_clients) {
        return false;
    }

    List_Append(list->clients, client);

    return true;
}

void ClientList_Disconnect(ClientList* list, Client client)
{
    char addr[IP_MAXSTRLEN];
    address_to_string(client.address, addr, false);
    printf("Client disconnected [%s]\n", addr);

    delscreen(client.screen);
    fclose(client.file);
    close(client.socket);

    ssize_t index;
    if ((index = ClientList_GetIndex(list, client)) != -1) {
        List_Erase(list->clients,  ClientList_GetIndex(list, client));
    } else {
        fprintf(stderr, "Failed to disconnect client\n");
    }

}

ssize_t ClientList_GetIndex(ClientList* list, Client client)
{
    size_t i;
    for (i = 0; i < list->clients->size; i++) {
        // Here we just search by socket; we could probably use other
        // unique variables. Memory addresses wouldn't work as conveniently.
        // See server.c: `Server_HandleConnection`
        if (list->clients->list[i].socket == client.socket) {
            return i;
        }
    }
    fprintf(stderr, "ClientList_GetIndex: `client` is not in `list`!\n");
    return -1;
}

Client Client_New(Socket socket, IPAddress address) {
    Client c = {address, socket, fdopen(socket, "r+"), NULL, false};
    return c;
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

void Client_Send_VA(Client client, char* format, va_list argument_list)
{
    va_list arglist;
    va_copy(arglist, argument_list);
    if (vfprintf(client.file, format, arglist) == -1) {
        error("Failed to send message to client", 0);
    }
    fflush(client.file);
    va_end(arglist);
}
