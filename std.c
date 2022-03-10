#include "std.h"
#include "err.h"
#include <stdlib.h>

void *malloc2(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL)
        errExit("malloc");
    return ret;
}
void *realloc2(void *oldMem, size_t size)
{
    void *p = realloc(oldMem, size);
    if (p == NULL)
        errExit("realloc");
    return p;
}
void free2(void *ptr)
{
    if (ptr == NULL)
        return;
    free(ptr);
    ptr = NULL;
}