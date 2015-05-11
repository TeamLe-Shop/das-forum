#include "util.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(char* message, int error)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    if (error) {
        exit(error);
    }
}

void address_to_string(IPAddress address, char* string, bool include_port)
{
    char port_str[PORT_LENGTH + 2];
    if (address.ss_family == AF_INET) {
        IP4Address ip4addr = *(IP4Address*)&address;
        inet_ntop(AF_INET, &ip4addr.sin_addr, string, INET_ADDRSTRLEN);
        if (include_port) {
            sprintf(port_str, ":%d", ip4addr.sin_port);
            strlcat(string, port_str, IP_MAXSTRLEN);
        }
    } else if (address.ss_family == AF_INET6) {
        IP6Address ip6addr = *(IP6Address*)&address;
        inet_ntop(AF_INET6, &ip6addr.sin6_addr, string, INET6_ADDRSTRLEN);
        if (include_port) {
            sprintf(port_str, ":%d", ip6addr.sin6_port);
            strlcat(string, port_str, IP_MAXSTRLEN);
        }
    }
}
