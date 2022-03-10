#include "err.h"
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
int TCP_connect(const char *servIP, int servPort)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errExit("socket");
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(servPort);
    if (inet_pton(AF_INET, servIP, &servaddr.sin_addr) != 1)
        errExit("inet_pton");
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
        errExit("connect");
    return sockfd;
}
void TCP_send(int sockfd, const char *buf, size_t nbytes)
{
    ssize_t count = write(sockfd, buf, nbytes);
    if (count != nbytes)
        errExit("write");
}
ssize_t TCP_recv(int sockfd, char *buf, size_t nbytes)
{
    ssize_t sum, count;
    for (sum = 0;;) {
        count = read(sockfd, buf + sum, nbytes);
        if (count == -1)
            errExit("read");
        sum += count;
        if (buf[sum - 1] == '}')
            break;
        else
            sleep(1);
    }
    return sum;
}
/*
ssize_t TCP_recv(int sockfd, char *buf, size_t nbytes)
{
    ssize_t count = read(sockfd, buf, nbytes);
    if (count == -1)
        errExit("read");
    return count;
}*/