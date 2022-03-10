#ifndef ERR_H
#define ERR_H

#define szErrBuf   1024
#define szPrintBuf 4096
void errExit(const char *format, ...);
void errPrint(const char *format, ...);
void print(const char *format, ...);
#endif