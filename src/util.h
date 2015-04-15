#ifndef UTIL_H
#define UTIL_H

#include "types.h"

#include <stdbool.h>

#define PORT_LENGTH 5
#define IP_MAXSTRLEN INET6_ADDRSTRLEN + 1 + PORT_LENGTH + 1

/*
 * Prints `message` along with an error message for the global variable
 * errno using strerror. If `exit_code` is non-0, it will exit with exit code
 * `exit_code`.
 *
 * Format: "`message`: strerror(errno)"
 */
void error(char* message, int exit_code);

/*
 * Store the human-readable format of `addr` in `string`.
 * The port will be included if `include_port` is true.
 */
void address_to_string(IPAddress address, char* string, bool include_port);

#endif /* UTIL_H */
