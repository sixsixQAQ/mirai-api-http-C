#include "mirai.h"
#include "err.h"
#include "queue.h"
#include "request.h"
#include "std.h"
#include "tcp.h"
#include <cjson/cJSON.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*variables defined  in "request.c"*/
extern char recvBuffer[];
extern char sendBuffer[];
char *const M_recvBufer = recvBuffer;
char *const M_sendBuffer = sendBuffer;
/*Default variables, which is suitable for Mirai*/
const char Default_servIP[szIP] = "127.0.0.1";
const int Default_servPort = 8080;
const char Default_contentType[] = "application/json; charset=UTF-8";
const char Default_host[] = "127.0.0.1:8080";
const char Default_connection[] = "keep-alive";

/*Global variables used by Mirai function*/
int Save_sockfd;
int Save_servPort;
char Save_servIP[szIP];
char Save_contentType[szContentType];
char Save_host[szHost];
char Save_connection[szConnection];
char Save_data[szData];
char Save_sessionKey[szSessionKey];
char Save_verifyKey[szVerifyKey];
char Save_QQ[szQQ];

int *const M_sockfd = &Save_sockfd;
int *const M_servPort = &Save_servPort;
char *const M_servIP = Save_servIP;
char *const M_contentType = Save_contentType;
char *const M_host = Save_host;
char *const M_connection = Save_connection;
char *const M_data = Save_data;
char *const M_sessionKey = Save_sessionKey;
char *const M_verifyKey = Save_verifyKey;
char *const M_QQ = Save_QQ;

Queue M_msgSendQueue;
Queue M_msgRecvQueue;

static char *getBeg(const char *str);
static void M_checkResponseCode();
static void M_saveSessionKey();
static void M_msgSendQueueInit();
static void M_msgRecvQueueInit();
static Msg M_createMsg(const char *key, const char *value);
static void
M_sendMessage(const char *uri, const char *target, int echo);

/*call HTTP_post() and check the response-code automtically*/
void M_HTTP_post(int sockfd, const Post *post, int echo)
{
    HTTP_post(sockfd, post, echo);
    M_checkResponseCode();
}
void M_HTTP_get(int sockfd, const Get *get, int echo)
{
    HTTP_get(sockfd, get, echo);
    M_checkResponseCode();
}
/*M_defaultInit() will connect to server by calling TCP_connect()  automatically*/
void M_defaultInit(const char *verifyKey, const char *QQ)
{
    int sockfd = TCP_connect(Default_servIP, Default_servPort);
    M_customInit(sockfd, verifyKey, QQ, Default_host, Default_connection, Default_contentType);
    M_msgSendQueueInit();
    M_msgRecvQueueInit();
}
/*M_customInit() need to connect to server  on yourself by calling TCP_connect() */
void M_customInit(const int sockfd, const char *verifyKey, const char *QQ,
                  const char *host, const char *connection, const char *contentType)
{
    Save_sockfd = sockfd;
    strcpy(Save_verifyKey, verifyKey);
    strcpy(Save_QQ, QQ);
    strcpy(Save_host, host);
    strcpy(Save_connection, strcasecmp(connection, "default") ? connection : Default_connection);
    strcpy(Save_contentType, strcasecmp(contentType, "default") ? contentType : Default_contentType);
    M_msgSendQueueInit();
    M_msgRecvQueueInit();
}
void M_checkVerifyKey(int echo)
{
    RequestBasis *basis = makeBasis("POST", Save_host, Save_connection, Save_contentType);
    URI *uri = makeURI("verify", NULL);
    snprintf(Save_data, sizeof(Save_data), "{verifyKey:%s}", Save_verifyKey);
    Post *post = makePost(basis, uri, Save_data);
    M_HTTP_post(Save_sockfd, post, echo);
    M_saveSessionKey();
    freePost(post);
}
void M_bindSessionKey(int echo)
{
    RequestBasis *basis = makeBasis("POST", Save_host, Save_connection, Save_contentType);
    URI *uri = makeURI("bind", NULL);
    snprintf(Save_data, sizeof(Save_data), "{sessionKey:%s,qq:%s}",
             Save_sessionKey, Save_QQ);
    Post *post = makePost(basis, uri, Save_data);
    M_HTTP_post(Save_sockfd, post, echo);
    free(post);
}
void M_msgAddFace(int faceNum, ... /*const char**/)
{
    size_t i;
    va_list argptr;
    va_start(argptr, faceNum);
    for (i = 0; i < faceNum; ++i) {
        if (isFull(M_msgSendQueue))
            errExit("isFull");
        Msg msg = M_createMsg("Face", va_arg(argptr, const char *));
        push(M_msgSendQueue, &msg);
    }
    va_end(argptr);
}
void M_msgAddText(int textNum, ... /*const char**/)
{
    size_t i;
    va_list argptr;
    va_start(argptr, textNum);
    for (i = 0; i < textNum; ++i) {
        if (isFull(M_msgSendQueue))
            errExit("isFull");
        Msg msg = M_createMsg("Plain", va_arg(argptr, const char *));
        push(M_msgSendQueue, &msg);
    }
    va_end(argptr);
}
void M_msgAddImg(int imgNum, ... /*const char**/)
{
    size_t i;
    va_list argptr;
    va_start(argptr, imgNum);
    for (i = 0; i < imgNum; ++i) {
        if (isFull(M_msgSendQueue))
            errExit("isFull");
        Msg msg = M_createMsg("Image", va_arg(argptr, const char *));
        push(M_msgSendQueue, &msg);
    }
    va_end(argptr);
}
void M_sendGroupMessage(const char *target, int echo)
{
    M_sendMessage("sendGroupMessage", target, echo);
}
void M_sendFriendMessage(const char *target, int echo)
{
    M_sendMessage("sendFriendMessage", target, echo);
}
size_t M_getUnreadMsgCount(int echo)
{
    RequestBasis *basis = makeBasis("GET", Save_host, Save_connection, Save_contentType);
    char args[szArgs];
    snprintf(args, sizeof(args), "?sessionKey=%s", Save_sessionKey);
    URI *uri = makeURI("countMessage", args);
    Get *get = makeGet(basis, uri);
    M_HTTP_get(Save_sockfd, get, echo);
    freeGet(get);
    cJSON *root = cJSON_Parse(getBeg(recvBuffer));
    cJSON *item = cJSON_GetObjectItem(root, "data");
    char *value = cJSON_Print(item);
    size_t msgCount = atoi(value);
    free(value);
    cJSON_Delete(root);
    return msgCount;
}
void M_getUnreadMsg(size_t conut, int echo)
{
    RequestBasis *basis = makeBasis("GET", Save_host, Save_connection, Save_contentType);
    char args[szArgs];
    snprintf(args, sizeof(args), "?sessionKey=%s&count=%ld", Save_sessionKey, (long)conut);
    URI *uri = makeURI("fetchMessage", args);
    Get *get = makeGet(basis, uri);
    M_HTTP_get(Save_sockfd, get, echo);
    freeGet(get);
    M_checkResponseCode();
}
void M_parseGroupMessage()
{
    Message message;
    cJSON *J_root, *J_data, *J_item, *J_sender, *J_group, *J_msgs, *J_msg;
    size_t size, i, msgNum, j;
    J_root = cJSON_Parse(getBeg(recvBuffer));
    J_data = cJSON_GetObjectItem(J_root, "data");
    size = cJSON_GetArraySize(J_data);
    for (i = 0; i < size; ++i) {

        J_item = cJSON_GetArrayItem(J_data, i);
        /*get message type*/
        strcpy(message.type, cJSON_GetStringValue(cJSON_GetObjectItem(J_item, "type")));
        if (strcmp(message.type, "GroupMessage") != 0) {
            cJSON_Delete(J_root);
            return;
        }
        /*get msgs*/
        J_msgs = cJSON_GetObjectItem(J_item, "messageChain");
        msgNum = cJSON_GetArraySize(J_msgs);
        message.msgNum = msgNum - 1;
        for (j = 1; j < msgNum; ++j) {
            J_msg = cJSON_GetArrayItem(J_msgs, j);
            strcpy(message.msgs[j - 1].key, cJSON_GetStringValue(cJSON_GetObjectItem(J_msg, "type")));
            if (strcmp(message.msgs[j - 1].key, "Plain") == 0)
                strcpy(message.msgs[j - 1].value, cJSON_GetStringValue(cJSON_GetObjectItem(J_msg, "text")));
            else if (strcmp(message.msgs[j - 1].key, "Image") == 0)
                strcpy(message.msgs[j - 1].value, cJSON_GetStringValue(cJSON_GetObjectItem(J_msg, "url")));
            else if (strcmp(message.msgs[j - 1].key, "Face") == 0)
                snprintf(message.msgs[j - 1].value, sizeof(message.msgs[j - 1].value), "%.0lf",
                         cJSON_GetNumberValue(cJSON_GetObjectItem(J_msg, "faceId")));
        }
        /*get sender*/
        J_sender = cJSON_GetObjectItem(J_item, "sender");
        snprintf(message.sender.senderID, sizeof(message.sender.senderID), "%.0lf",
                 cJSON_GetNumberValue(cJSON_GetObjectItem(J_sender, "id")));
        strcpy(message.sender.senderName, cJSON_GetStringValue(cJSON_GetObjectItem(J_sender, "memberName")));
        /*get group*/
        J_group = cJSON_GetObjectItem(J_sender, "group");
        snprintf(message.group.groupID, sizeof(message.group.groupID), "%.0lf",
                 cJSON_GetNumberValue(cJSON_GetObjectItem(J_group, "id")));
        strcpy(message.group.groupName, cJSON_GetStringValue(cJSON_GetObjectItem(J_group, "name")));
        push(M_msgRecvQueue, &message);
    }
    cJSON_Delete(J_root);

#ifdef DEBUG_RECVMESSAGE
    print("%s\n", message.type);
    print("%s %s\n", message.msg.value, message.msg.key);
    print("%s %s\n", message.group.groupID, message.group.groupName);
    print("%s %s\n", message.sender.senderID, message.sender.senderName);
#endif
}
/*RecvMsg refers to messages in M_msgRecvQueue*/
size_t M_getRecvMsgNum()
{
    return getElemNum(M_msgRecvQueue);
}
size_t M_getSendMsgNum()
{
    return getElemNum(M_msgSendQueue);
}
static void
M_sendMessage(const char *_uri_, const char *target, int echo)
{
    RequestBasis *basis = makeBasis("POST", Save_host, Save_connection, Save_contentType);
    URI *uri = makeURI(_uri_, NULL);
    snprintf(Save_data, sizeof(Save_data),
             "{sessionKey:%s,target:%s,messageChain:[",
             Save_sessionKey, target);
    char tmp[szData];
    for (; !isEmpty(M_msgSendQueue);) {
        Msg msg;
        pop(M_msgSendQueue, &msg);
        if (strcmp(msg.key, "Plain") == 0)
            snprintf(tmp, sizeof(tmp), "{type:Plain,text:\"%s\"}", msg.value);
        else if (strcmp(msg.key, "Image") == 0)
            snprintf(tmp, sizeof(tmp), "{type:Image,url:\"%s\"}", msg.value);
        else if (strcmp(msg.key, "Face") == 0)
            snprintf(tmp, sizeof(tmp), "{type:Face,faceId:\"%s\"}", msg.value);
        strcat(Save_data, tmp);
        if (isEmpty(M_msgSendQueue))
            strcat(Save_data, "]}");
        else
            strcat(Save_data, ",");
    }
    Post *post = makePost(basis, uri, Save_data);
    M_HTTP_post(Save_sockfd, post, echo);
    freePost(post);
}

/*Used by cJSON function to get the begining position of '{' */
static char *
getBeg(const char *str)
{
    char *beg = strchr(str, '{');
    if (beg == NULL)
        errExit("strchr");
    return beg;
}
static void
M_msgSendQueueInit()
{
    M_msgSendQueue = createQueue(sizeof(Msg), szMaxMsgNum);
}
static void
M_msgRecvQueueInit()
{
    M_msgRecvQueue = createQueue(sizeof(Message), szMaxMsgNum);
}
static void
M_checkResponseCode()
{
    cJSON *root = cJSON_Parse(getBeg(recvBuffer));
    cJSON *item = cJSON_GetObjectItem(root, "code");
    char *value = cJSON_Print(item);
    int code = atoi(value);
    free(value);
    cJSON_Delete(root);
    if (code != 0)
        print("\e[31mcode:%d\n", code);
}
static void
M_saveSessionKey()
{
    cJSON *root = cJSON_Parse(getBeg(M_recvBufer));
    cJSON *item = cJSON_GetObjectItem(root, "session");
    char *sessionKey = cJSON_GetStringValue(item);
    strcpy(Save_sessionKey, sessionKey);
    cJSON_Delete(root);
}
static Msg
M_createMsg(const char *key, const char *value)
{
    Msg msg;
    strncpy(msg.key, key, sizeof(msg.key));
    strncpy(msg.value, value, sizeof(msg.value));
    return msg;
}