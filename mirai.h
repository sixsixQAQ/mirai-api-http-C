#ifndef MIRAI_H
#define MIRAI_H

#include "queue.h"
#include "request.h"

#define szQQ         20
#define szSessionKey 15
#define szVerifyKey  30

extern const char Default_servIP[];
extern const int Default_servPort;
extern const char Default_contentType[];
extern const char Default_host[];
extern const char Default_connection[];

extern int Save_sockfd;
extern char Save_servIP[];
extern int Save_servPort;
extern char Save_contentType[];
extern char Save_host[];
extern char Save_connection[];
extern char Save_data[];
extern char Save_sessionKey[];
extern char Save_verifyKey[];

extern int *const M_sockfd;
extern int *const M_servPort;
extern char *const M_servIP;
extern char *const M_contentType;
extern char *const M_host;
extern char *const M_connection;
extern char *const M_data;
extern char *const M_sessionKey;
extern char *const M_verifyKey;

#define szTarget    szQQ
#define szType      szKey
#define szKey       100
#define szValue     (szData - szSessionKey - szTarget - szSpace)
#define szMaxMsgNum 100

#define szSenderID   20
#define szSenderName 50
#define szGroupID    20
#define szGroupName  50
typedef struct Msg {
    char key[szKey];
    char value[szValue];
}Msg;
typedef struct Group {
    char groupID[szGroupID];
    char groupName[szGroupName];
}Group;
typedef struct Sender {
    char senderID[szSenderID];
    char senderName[szSenderName];
}Sender;
typedef struct Message {
    char type[szType];
    Sender sender;
    Group group;
    Msg msgs[szMaxMsgNum];
    size_t msgNum;
}Message;
enum responseCode { success = 0,
                    verify_key_is_error,
                    bot_is_not_exist,
                    session_is_false,
                    session_is_not_bind,
                    target_not_exist,
                    file_not_exist,
                    bot_has_no_power = 10,
                    bot_is_silenced = 20,
                    message_is_too_long = 30,
                    error = 400 };

void M_HTTP_post(int sockfd, const Post *post, int echo);
void M_HTTP_get(int sockfd, const Get *get, int echo);
void M_defaultInit(const char *verifyKey, const char *QQ);
void M_customInit(const int sockfd, const char *verifyKey, const char *QQ,
                  const char *host, const char *connection, const char *contentType);
void M_checkVerifyKey(int echo);
void M_bindSessionKey(int echo);
void M_msgAddFace(int faceNum, ... /*const char**/);
void M_msgAddText(int textNum, ... /*const char* text*/);
void M_msgAddImg(int imgNum, ... /*const char* url*/);
void M_sendGroupMessage(const char *target, int echo);
void M_sendFriendMessage(const char *target, int echo);
size_t M_getSendMsgNum();
size_t M_getRecvMsgNum();
size_t M_getUnreadMsgCount(int echo);
void M_getUnreadMsg(size_t conut, int echo);
void M_parseGroupMessage();
#endif