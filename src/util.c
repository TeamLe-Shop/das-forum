#include "util.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

void error(char* message)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
}
