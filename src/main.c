#include <stdio.h>
#include <stdbool.h>

#include <sys/socket.h>

#include "util.h"
#include "types.h"
#include "server.h"

#define DEFAULT_PORT 7778

int main(int argc, char* argv[])
{
    // These variables should be loaded from configuration.
    int port = DEFAULT_PORT;
    Domain domain = PF_INET6;
    bool reuse_address = true;

    Server* server = Server_Open(domain, port, reuse_address);
    while (Server_Cycle(server)) {}
    Server_Close(server);
    return 0;
}
