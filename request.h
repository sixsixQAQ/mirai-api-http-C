#ifndef REQUEST_H
#define REQUEST_H

#include <stddef.h>

#define szIP          sizeof("255.255.255.255")
#define szPort        sizeof("65535")
#define szMethod      sizeof("POST")
#define szPath        256
#define szArgs        256
#define szURI         (szResource + szArgs)
#define szHost        sizeof("255.255.255.255: 65535")
#define szConnection  sizeof("keep-alive")
#define szContentType (2*sizeof("application/vnd.android.package-archive"))
#define szData        4096
#define szSpace       200

enum echoChoice { echoNone,
                  echoSend,
                  echoRecv,
                  echoBoth };
typedef struct RequestBasis {
    char method[szMethod];
    char host[szHost];
    char connection[szConnection];
    char contentType[szContentType];
} RequestBasis;

typedef struct URI {
    char path[szPath];
    char args[szArgs];
} URI;

typedef struct Get {
    RequestBasis *basis;
    URI *uri;
    size_t contentLength;
} Get;

typedef struct Post {
    RequestBasis *basis;
    URI *uri;
    char *data;
    size_t contentLength;
} Post;


RequestBasis *
makeBasis(const char *method, const char *host,
          const char *connection, const char *contentType);
URI *makeURI(const char *path, const char *args);
Post *makePost(const RequestBasis *basis, const URI *uri, const char *data);
Get *makeGet(const RequestBasis *basis, const URI *uri);
void freePost(Post *post);
void freeGet(Get *get);

extern char recvBuffer[];
extern char sendBuffer[];
//static void sendPost(int sockfd, const Post *post);
//static void sendGet(int sockfd, const Get *get);
void HTTP_post(int sockfd, const Post *post, int echo);
void HTTP_get(int sockfd, const Get *get, int echo);
#endif