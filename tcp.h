#ifndef TCP_H
#define TCP_H

#include <sys/types.h>
int TCP_connect(const char *servIP, int servPort);
void TCP_send(int sockfd, const char *buf, size_t nbytes);
ssize_t TCP_recv(int sockfd, char *buf, size_t nbytes);

#define sendMax(The_Str_For_Send) (strlen(The_Str_For_Send))
#define recvMax(The_Buf_Wait_Recv) (sizeof(The_Buf_Wait_Recv))
#endif