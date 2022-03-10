#include "err.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void errExit(const char *format, ...)
{
    char buf[szErrBuf];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buf, sizeof(buf) - 1, format, argptr);
    va_end(argptr);
    fprintf(stderr, "\e[31m[ERROR]:%s\n", buf);
    exit(EXIT_FAILURE);
}
void errPrint(const char *format, ...)
{
    char buf[szErrBuf];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buf, sizeof(buf) - 1, format, argptr);
    va_end(argptr);
    fprintf(stderr, "\e[31m[ERROR]:%s\n", buf);
}
void print(const char *format, ...)
{
    char buf[szPrintBuf];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buf, sizeof(buf) - 1, format, argptr);
    va_end(argptr);
    fprintf(stderr, "%s", buf);
}
