#include "db.h"
#include "err.h"
#include "mirai.h"
#include "queue.h"
#include "request.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

extern Queue M_msgRecvQueue;
const char dbName[] = "db/philosophy.db";
const char tableName[] = "FIRST";

int insertCallback(void *data, int argc, char **argv, char **azColName);
void makePhilosophyText(char *retText, size_t size, sqlite3 *db, const char *tableName, size_t id);
void sendARandomPhilosophy(Message message);
int main(void)
{
    const char servIP[] = "127.0.0.1";
    const int servPort = 8080;
    const char verifyKey[] = "YQ666";
    const char QQ[][szQQ] = {"1748879992", "2942575858"};
    const char mode[] = "default";
    int sockfd = TCP_connect(servIP, servPort);

    M_customInit(sockfd, verifyKey, QQ[0], "127.0.0.1:8080", mode, mode); // be eauql to M_defaultInit(verifyKey, QQ);
    M_checkVerifyKey(echoNone);
    M_bindSessionKey(echoNone);

    size_t num;
    Message message;
    for (;;) {
        num = M_getUnreadMsgCount(echoNone);
        if (num != 0) {
            M_getUnreadMsg(num, echoRecv);
            M_parseGroupMessage();
            for (; !isEmpty(M_msgRecvQueue);) {
                pop(M_msgRecvQueue, &message);
                if (strcmp(message.type, "GroupMessage") != 0)
                    continue;
                if (strcmp(message.msgs[0].key, "Plain") != 0)
                    continue;
                if (strcasecmp(message.msgs[0].value, "/philosophy") != 0)
                    continue;
                sendARandomPhilosophy(message);
            }
        }
    }

    return 0;
}
/*repeatGroupMemberMessage should be called after binding sessionKey*/
void repeatGroupMemberMessage()
{
    size_t i, k, num;
    Message message;
    for (i = 0;; ++i) {
        num = M_getUnreadMsgCount(echoNone);
        if (num != 0) {
            M_getUnreadMsg(num, echoNone);
            M_parseGroupMessage();
            for (; !isEmpty(M_msgRecvQueue);) {
                pop(M_msgRecvQueue, &message);
                if (strcmp(message.type, "GroupMessage") != 0)
                    continue;
                print("[msgNum]:%ld\n\n", message.msgNum);
                for (k = 0; k < message.msgNum; ++k) {

                    if (strcmp(message.msgs[k].key, "Plain") == 0)
                        M_msgAddText(1, message.msgs[k].value);
                    else if (strcmp(message.msgs[k].key, "Image") == 0)
                        M_msgAddImg(1, message.msgs[k].value);
                    else if (strcmp(message.msgs[k].key, "Face") == 0)
                        M_msgAddFace(1, message.msgs[k].value);
                }
                M_sendGroupMessage(message.group.groupID, echoSend);
                print("%ld", getElemNum(M_msgRecvQueue));
            }
        }
    }
}
void sendARandomPhilosophy(Message message)
{
    char text[sizeof(Philosophy)];
    sqlite3 *db = DB_open(dbName);
    makePhilosophyText(text, sizeof(text), db, tableName, -1);
    DB_close(db);
    M_msgAddText(1, text);
    M_sendGroupMessage(message.group.groupID, echoSend);
}
void makePhilosophyText(char *retText, size_t size, sqlite3 *db, const char *tableName, size_t id)
{
    size_t maxID = DB_getMaxID(db, tableName);
    if (id < 0 || id > maxID) {
        srand(time(NULL));
        id = rand() % (maxID + 1);
    }
    db = DB_open(dbName);
    Philosophy phi;
    DB_getPhilosophy(id, db, tableName, &phi);
    DB_close(db);
    snprintf(retText, size, "        %s\n\n"
                            "                          "
                            "——%s",
             phi.text, phi.author);
}
int insertCallback(void *data, int argc, char **argv, char **azColName)
{
    size_t i;
    for (i = 0; i < argc; ++i) {
        print("%s = %s\n", argv[i], azColName[i]);
    }
    return 0;
}