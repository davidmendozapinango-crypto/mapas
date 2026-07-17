#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

static char *xstrdup(const char *s)
{
    if (s == NULL) {
        return NULL;
    }
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p != NULL) {
        memcpy(p, s, n);
    }
    return p;
}

#endif
