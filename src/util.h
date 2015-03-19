#ifndef UTIL_H
#define UTIL_H

#include <string.h>

/*
 * Prints `message` along with an error message for the global variable
 * errno.
 *
 * Format: "`message`: strerror(errno)"
 */
void error(char* message);

#endif /* UTIL_H */
