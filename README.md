[toc]
# mirai-api-http-C
## 说明
- 本项目是*Linux*环境（Ubuntu-20.04 LTS）下对[mirai-api-http](https://github.com/project-mirai/mirai-api-http)的简易封装。
- 需要安装额外库[cJSON](https://github.com/DaveGamble/cJSON)和[sqlite3](https://www.baidu.com/link?url=jQiQ_bB03KuYyH4AH0uNbw_LafjINaduQc4gHLWE-H7&wd=&eqid=893a6f0e00056dc50000000662297b94)。
- 较为简陋，**仅适合初学者学习使用**。
- 注释会很详细。（完善ing...）
## 使用mirai-api-http-C
### 安装mirai-console-loader
1. 安装Java OpenJDK (版本必须>=11)
```shell
$apt update
$apt install openjdk-17-jre-headless
$java -version
```
2. 从Releases下载最新版mirai-console-loader
[mirai-console-loader](https://github.com/iTXTech/mirai-console-loader/releases)
3. 解压
4. `./mcl`启动mirai-ap-http

### 安装mirai-api-http
```shell
$./mcl --update-package net.mamoe:mirai-api-http --channel stable-v2 --type plugin
```
### 修改默认配置文件
```shell
$ vim mirai/config/net.mamoe.mirai-api-http/setting.yml
```
写入以下内容（详情请见[mirai-api-http](https://github.com/project-mirai/mirai-api-http)）。
```shell
adapters:
  - http
  - ws

enableVerify: true
verifyKey: 1234567890
debug: false

singleMode: false
cacheSize: 4096
adapterSettings:
  http:
    host: localhost
    port: 8080
    cors: ["*"]
  ws:
    host: localhost
    port: 8080
    reservedSyncId: -1
```
### 启动mirai-console-loader
```shell
$./mcl
```
### 登陆机器人qq号
```shell
$login <account> <passwd>
```
### 启动机器人
```shell
$./main
```

## 功能
### 基本功能
好友消息发送/接受，群消息发送/接受（支持图片、emoji）。
### 示例机器人
#### 复读机
机器人复读一次玩家发送的消息。
#### 哲学语录推送
QQ群输入`/philosophy`，robot将随即发送一条哲学语录。

## 主要接口
```c
/*发送post请求*/
void M_HTTP_post(int sockfd, const Post *post, int echo);
/*发送get请求*/
void M_HTTP_get(int sockfd, const Get *get, int echo);
/*默认初始化*/
void M_defaultInit(const char *verifyKey, const char *QQ);
/*个性初始化*/
void M_customInit(const int sockfd, const char *verifyKey, const char *QQ,const char *host, const char *connection, const char *contentType);
/*检查verifyKey*/
void M_checkVerifyKey(int echo);
/*绑定sessionKey*/
void M_bindSessionKey(int echo);
/*向消息发送队列添加emoji*/
void M_msgAddFace(int faceNum, ... /*const char**/);
/*向消息发送队列添加文本*/
void M_msgAddText(int textNum, ... /*const char* text*/);
/*向消息发送队列添加图片*/
void M_msgAddImg(int imgNum, ... /*const char* url*/);
/*将消息发送队列中的消息发送到指定群*/
void M_sendGroupMessage(const char *target, int echo);
/*将消息发送队列中的消息发送到指定好友*/
void M_sendFriendMessage(const char *target, int echo);
/*获取消息发送队列消息数*/
size_t M_getSendMsgNum();
/*获取消息接受队列消息数*/
size_t M_getRecvMsgNum();
/*获取未读消息数*/
size_t M_getUnreadMsgCount(int echo);
/*获取未读消息，放入缓冲区*/
void M_getUnreadMsg(size_t conut, int echo);
/*解析缓冲区群消息，放入消息接受队列*/
void M_parseGroupMessage();
```
更多描述请见`Details.md`（完善ing...）
