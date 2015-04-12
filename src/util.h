#ifndef UTIL_H
#define UTIL_H

#include "types.h"

/*
 * Prints `message` along with an error message for the global variable
 * errno, and then exits with exit code 1.
 *
 * Format: "`message`: strerror(errno)"
 */
void error(char* message);

/*
 * Return a string containing an internet address in human readable format.
 */
char* ipaddr(Address addr);

#endif /* UTIL_H */
