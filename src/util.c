#include "util.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void error(char* message, int error)
{
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    if (error) {
        exit(1);
    }
}
