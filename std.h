#ifndef STD_H
#define STD_H
#include <stddef.h>

void *malloc2(size_t size);
void *realloc2(void *oldMem, size_t size);
void free2(void *ptr);
#endif