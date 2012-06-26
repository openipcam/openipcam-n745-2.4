#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#define weak_alias(name, aliasname) \
extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)));

typedef void *HTTPCONNECTION;
typedef int (*REQUEST_CALLBACK_PFUN)(HTTPCONNECTION hc, void *pParam);

typedef void *HTTPSERVER;

#include <sys/time.h>
#include "C_List.h"
#include "C_String.h"
#include "C_ConfigFile.h"
#include "C_HttpSupport.h"
#include "C_SendFile.h"
#include "C_MultiPart.h"

/* 挂载一个处理函数, 当用户提交http://hostname/<cpAccessName>?[parameters]的请求时，
** 转向该处理函数，
** 处理函数返回0表示不需再进行缺省处理，返回非0表示还要进行缺省处理 */
int RegisterEmbedFun(const char *pcAccessName, REQUEST_CALLBACK_PFUN funRequestCallBack, void *pParam);
/* 解除该处理函数的挂载 */
int UnregisterEmbedFun(const char *pcAccessName);
/* 清除所有已经挂载的函数 */
void ClearEmbedFun(void);

/* 定义当数据发送完毕后的回调函数 */
typedef int (*SEND_DATA_OVER_PFUN)(HTTPCONNECTION hConnection, time_t *tLastFill, void *pParam);
void SetSendDataOverFun(HTTPCONNECTION hConnection, SEND_DATA_OVER_PFUN funOnSendDataOver, void *pParam);

/* 定义收到POST方法后的回调函数
** Return 0, 该请求处理完毕, 当关闭此次请求
** Return 1, 该请求需要继续处理, 当保持此次请求
** Return -1, 该请求发生错误, 当关闭此次请求 */
typedef int (*POST_DATA_PFUN)(HTTPCONNECTION hConnection,
								int *piPostState,
								char **ppcPostBuf,
								int *piPostBufLen,
								int *piPostDataLen,
								char *pcFillData,
								int iFillDataLen,
								int iIsMoreData/*bool*/,
								void *pParam/*other parameter for extend use*/);

void SetPostDataFun(HTTPCONNECTION hConnection, POST_DATA_PFUN funPostDataGot, void *pParam);
void AddHttpBody(HTTPCONNECTION hConnection, const char *pcBuf2Add, int iBufLen);
void AddHttpBodyString(HTTPCONNECTION hConnection, const char *pcString);

/* 设置Http KeepAlive属性, must be called before SetHttpHeader() */
void SetKeepAliveMode(HTTPCONNECTION hConnection, BOOL bIsKeepAlive);
void SetHttpHeader(HTTPCONNECTION hConnection, int iStatus, const char *pcTitle, const char *pcEncodings, const char *pcExtraheads, const char *pcContentType, BOOL bShowLength);
/* 取得加上document root后的字符串, 用完需要free */
char *GetDocumentBasedPath(char *pcUriPath);
/* 启动Http服务器，支持ssl */
HTTPSERVER StartThttpdEx3(char *pcServerRoot,
				int *piPort,
				int *piSSLPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnListenSocketCreate)(int fd, int iPort),
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin);
/* 启动Http服务器, 支持跟踪listen socket的建立 */
HTTPSERVER StartThttpdEx2(char *pcServerRoot,
				int *piPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnListenSocketCreate)(int fd, int iPort),
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin);
/* 启动Http服务器, 支持多个port */
HTTPSERVER StartThttpdEx(char *pcServerRoot,
				int *piPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin);
/* 启动Http服务器 */
HTTPSERVER StartThttpd(char *pcServerRoot,
				int iPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin);


/****************** METHODS *****************/
#define	M_INVALID	-1
#define	M_SHORT	0
#define M_GET		1
#define M_HEAD		2
#define M_PUT		3
#define M_POST		4
#define M_DELETE	5
#define M_LINK		6
#define M_UNLINK	7
/* 取得请求的方法 HEAD, GET, POST, ...*/
int GetHttpMethod(HTTPCONNECTION hConnection);
/* 取得Client地址 */
struct in_addr GetClientAddr(HTTPCONNECTION hConnection);
/* 取得Client MAC地址, 返回一个长度为6的字符数组 */
char *GetClientMac(HTTPCONNECTION hConnection);
/* 取得用户用GET方法提交请求时所加的参数, 未解码 */
char *GetQueryString(HTTPCONNECTION hConnection);
/* 取得解码后的请求路径, ip以后的部分, 不含query */
char *GetRequestPath(HTTPCONNECTION hConnection);
/* 取得请求的实际文件路径 */
char *GetRequestFilePath(HTTPCONNECTION hConnection);
/* 取得Referer */
char *GetReferer(HTTPCONNECTION hConnection);
/* 取得Content-type */
char *GetContentType(HTTPCONNECTION hConnection);
/* 取得Content-length */
int GetContentLength(HTTPCONNECTION hConnection);



/* 以下权限从低到高，不要改变数值! */
#define AUTH_ANY -1
#define AUTH_USER 0
#define AUTH_ADMIN 1
#define AUTH_SYSTEM 2
int RegisterEmbedFunEx(const char *pcAccessName, REQUEST_CALLBACK_PFUN funRequestCallBack, int iPrivilegeRequired, void *pParam);
BOOL IsRegisterEmbedFunEx(const char *pcAccessName, REQUEST_CALLBACK_PFUN *pfunRequestCallBack, int *piPrivilegeRequired, void **ppParam);

char *GetCurrentUser(HTTPCONNECTION hConnection, char *pcUser, int iUserLen);
int GetAuthPrivilege(HTTPCONNECTION hConnection);
int SendAuthRequired(HTTPCONNECTION hConnection, int iPrivilege);
void SetAuthPrompt(const char *pcUserPrompt, const char *pcAdminPrompt, const char *pcSystemPrompt);
BOOL AuthGetUser(const char *pcUserName, char *pcPassword, int iMaxPassLen, int *piPrivilege);
int AuthSetUser(const char *pcUserName, const char *pcPassword, int iPrivilege);
int AuthDelUser(const char *pUserName);
LIST **GetAuthUserList();
void EnableUserCheck(BOOL bIsEnable);
BOOL IsEnableUserCheck(void);


#endif
