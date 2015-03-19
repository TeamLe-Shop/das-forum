#include <stdio.h>

#include <sys/socket.h>

#include "util.h"
#include "types.h"
#include "server.h"

#define DEFAULT_PORT 7778

int main(int argc, char* argv[])
{
    // These variables should be loaded from configuration.
    int port = DEFAULT_PORT;
    Domain domain = PF_INET;

    Server* server = Server_Open(domain, port);
    Server_Close(server);
    return 0;
}
