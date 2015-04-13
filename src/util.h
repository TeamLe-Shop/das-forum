#ifndef UTIL_H
#define UTIL_H

#include "types.h"

/*
 * Prints `message` along with an error message for the global variable
 * errno using strerror. If `exit_code` is non-0, it will exit with exit code
 * `exit_code`.
 *
 * Format: "`message`: strerror(errno)"
 */
void error(char* message, int exit_code);

/*
 * Return a string containing an internet address in human readable format.
 */
char* ipaddr(Address addr);

#endif /* UTIL_H */
