#include "request.h"
#include "err.h"
#include "std.h"
#include "tcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char recvBuffer[sizeof(RequestBasis) + sizeof(URI) + sizeof(size_t) + szData];
char sendBuffer[sizeof(recvBuffer)];

/*allocate a new memory and copy the data.*/
RequestBasis *
makeBasis(const char *method, const char *host,
          const char *connection, const char *contentType)
{
    RequestBasis *basis = (RequestBasis *)malloc2(sizeof(RequestBasis));
    strcpy(basis->method, method);
    strcpy(basis->host, host);
    strcpy(basis->connection, connection);
    strcpy(basis->contentType, contentType);
    return basis;
}
/*Allocate a new memory and copy the data.*/
URI *makeURI(const char *path, const char *args)
{
    URI *uri = (URI *)malloc2(sizeof(URI));
    if (path != NULL)
        strcpy(uri->path, path);
    if (args != NULL)
        strcpy(uri->args, args);
    return uri;
}
/*
post wills ucceed the old memory of basis, uri,allocate a new memory and copy data.
So we only need to free post, which can also free the memory succeeded
from uir and basis.
*/
Post *makePost(const RequestBasis *basis, const URI *uri, const char *data)
{
    Post *post = (Post *)malloc2(sizeof(Post));
    post->basis = basis;
    post->uri = uri;
    post->data = strdup(data);
    post->contentLength = strlen(data);
    return post;
}
Get *makeGet(const RequestBasis *basis, const URI *uri)
{
    Get *get = (Get *)malloc2(sizeof(Get));
    get->basis = basis;
    get->uri = uri;
    get->contentLength = 0; // It is different with what I think.
    // get->contentLength = strlen(get->uri->args);
    return get;
}
/*Free all the memory succeeeded*/
void freePost(Post *post)
{
    free2(post->basis);
    free2(post->uri);
    free2(post->data);
    free2(post);
}
void freeGet(Get *get)
{
    free2(get->basis);
    free2(get->uri);
    free(get);
}
static void
sendPost(int sockfd, const Post *post)
{
    snprintf(sendBuffer, sizeof(sendBuffer),
             "%s /%s%s HTTP/1.1\r\n"
             "Connection:%s\r\n"
             "Content-Type:%s\r\n"
             "Content-Length:%ld\r\n"
             "\r\n"
             "%s\r\n\r\n",
             post->basis->method, post->uri->path, post->uri->args,
             post->basis->connection, post->basis->contentType,
             post->contentLength, post->data);
    TCP_send(sockfd, sendBuffer, sendMax(sendBuffer));
}
static void
sendGet(int sockfd, const Get *get)
{
    snprintf(sendBuffer, sizeof(sendBuffer),
             "%s /%s%s HTTP/1.1\r\n"
             "Connection:%s\r\n"
             "Content-Type:%s\r\n"
             "Content-Length:%ld\r\n"
             "\r\n\r\n",
             get->basis->method, get->uri->path, get->uri->args,
             get->basis->connection, get->basis->contentType,
             get->contentLength);
    TCP_send(sockfd, sendBuffer, sendMax(sendBuffer));
}
void HTTP_post(int sockfd, const Post *post, int echo)
{
    sendPost(sockfd, post);
    if (echo == echoSend || echo == echoBoth)
        print("%s", sendBuffer);
    ssize_t nbytes = TCP_recv(sockfd, recvBuffer, recvMax(recvBuffer) - 1);
#ifdef DEBUG_TCP
    print("\n[ET recvBytes]%d\n", nbytes);
#endif
    recvBuffer[nbytes] = '\0';
    if (echo == echoRecv || echo == echoBoth)
        print("%s", recvBuffer);
}
void HTTP_get(int sockfd, const Get *get, int echo)
{
    sendGet(sockfd, get);
    if (echo == echoSend || echo == echoBoth)
        print("%s", sendBuffer);
    ssize_t nbytes = TCP_recv(sockfd, recvBuffer, recvMax(recvBuffer) - 1);
#ifdef DEBUG_TCP
    print("\n[ET recvBytes]%d\n", nbytes);
#endif
    recvBuffer[nbytes] = '\0';
    if (echo == echoRecv || echo == echoBoth)
        print("%s", recvBuffer);
}