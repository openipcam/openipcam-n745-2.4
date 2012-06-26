#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "C_List.h"
#include "C_String.h"

/* To do this because I found in some system strncpy Cannot work correctly. */
char *MyStrncpy(char *pcDest, const char *pcSrc, int iLength)
{
	int i;
	if (iLength <= 0) return NULL;

	for (i = 0; i < iLength; i++)
	{
		pcDest[i] = pcSrc[i];
		if (pcSrc[i] == '\0')
		{
			i++;
			break;
		}
	}
	pcDest[i-1] = '\0';
	return pcDest;
}

LIST *SplitString(const char *pcString, char cSeparator)
{
	LIST *pList;
	LISTNODE *pNode;
	const char *pcHeader;
	const char *pcCur;
	int iLen;

	if ((pList = CreateList()) == NULL) return NULL;

	if (pcString == NULL)
	{/* Don't need to split null string */
		return pList;
	}

	for (pcHeader = pcCur = pcString; ; pcCur++)
	{
		if (*pcCur == cSeparator || *pcCur == '\0')
		{
			if ((pNode = AppendNode(pList)) == NULL)
			{
				DeleteSplitString(pList);
				return NULL;
			}
			iLen = pcCur - pcHeader;
			if ((pNode->pValue = malloc(iLen + 1)) == NULL)
			{
				DeleteSplitString(pList);
				PRINT_MEM_OUT;
				return NULL;
			}
			memcpy(pNode->pValue, pcHeader, iLen);
			((char *)pNode->pValue)[iLen] = '\0';
			pcHeader = pcCur + 1;
		}
		if (*pcCur == '\0') break;
	}

	return pList;
}

void DeleteSplitString(LIST *pList)
{
	LISTNODE *pNode;
	if (pList == NULL) return;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
		if (pNode->pValue != NULL) free(pNode->pValue);
	DeleteList(pList);
}


char *GetTrimString(const char *pcString)
{
	const char *pcBegin;
	const char *pcEnd;
	const char *pc;
	char *pcReturn;
	BOOL bExpectBegin;
	int iLen;

	if (pcString == NULL) pcString = "";

	pcBegin = pcString;
	pcEnd = pcBegin;
	bExpectBegin = TRUE;
	for (pc = pcString; *pc != '\0'; pc++)
	{
		if (*pc != ' ' && *pc != '\t' && *pc != '\r' && *pc != '\n')
		{
			if (bExpectBegin)
			{
				pcBegin = pc;
				bExpectBegin = FALSE;
			}
			pcEnd = pc + 1;
		}
	}
	iLen = pcEnd - pcBegin;

	if ((pcReturn = (char *)malloc(iLen + 1)) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	memcpy(pcReturn, pcBegin, iLen);
	pcReturn[iLen] = '\0';
	return pcReturn;
}

char *GetCStyleString(const char *pcString)
{
	int i, iLen;
	char *pcReturn;
	char *pc;
	char *pcCStyleFrom = "\b\n\r\t\'\\\v\a";
	char *pcCStyleTo = "bnrt\'\\va";

	if (pcString == NULL) pcString = "";

	for (i = 0, iLen = 0; pcString[i] != '\0'; i++, iLen++)
		if (strchr(pcCStyleFrom, pcString[i]) != NULL) iLen++;

	if ((pcReturn = (char *)malloc(iLen + 1)) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	for (i = 0, iLen = 0; pcString[i] != '\0'; i++)
	{
		if ((pc = strchr(pcCStyleFrom, pcString[i])) != NULL)
		{
			pcReturn[iLen++] = '\\';
			pcReturn[iLen++] =  pcCStyleTo[pc - pcCStyleFrom];
		}
		else pcReturn[iLen++] = pcString[i];
	}
	pcReturn[iLen] = '\0';
	return pcReturn;
}

char *GetWebStyleString(const char *pcString)
{
	int i, iLen;
	char *pcReturn;
	char *pc;
	char *pcWebStyleFrom="<>\" &";
	char ppcWebStyleTo[][7]={"&lt;", "&gt;", "&quot;", "&#32;", "&amp;"};

	if (pcString == NULL) pcString = "";

	for (i = 0, iLen = 0; pcString[i] != '\0'; i++, iLen++)
		if (strchr(pcWebStyleFrom, pcString[i]) != NULL) iLen += 5;

	if ((pcReturn = (char *)malloc(iLen + 1)) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	for (i = 0, iLen = 0; pcString[i] != '\0'; i++)
	{
		if ((pc = strchr(pcWebStyleFrom, pcString[i])) != NULL)
		{
			strcpy(pcReturn + iLen, ppcWebStyleTo[pc - pcWebStyleFrom]);
			iLen += strlen(ppcWebStyleTo[pc - pcWebStyleFrom]);
		}
		else pcReturn[iLen++] = pcString[i];
	}
	pcReturn[iLen] = '\0';
	return pcReturn;
}

char *GetWebInCString(const char *pcStr)
{
	char *pcWeb;
	char *pcRt;
	pcWeb = GetWebStyleString(pcStr);
	pcRt = GetCStyleString(pcWeb);
	if (pcWeb != NULL) free(pcWeb);

	return pcRt;
}

unsigned long String2IP(const char *pcString)
{
	unsigned long ip[4];
	unsigned long ipRt;
	int i;
	const char *pc;

	for (i = 0, pc = pcString; pc != NULL && i < 4; i++)
	{
		ip[i] = (unsigned long)strtol(pc, NULL, 10);
		if (ip[i] >= 256) return 0;
		pc = strchr(pc, '.');
		if (pc != NULL) pc++;
	}
	if (i != 4) return 0;

	for (ipRt=0, i=3; i>=0; i--)
	{
		ipRt = (ipRt<<8) + ip[i];
	}

	return ipRt;
}

int IP2String(unsigned long ulIP, char *pcStringOut)
{
	int iRt;
	if (pcStringOut == NULL) return -1;

	bzero(pcStringOut, 16);

	iRt = sprintf(pcStringOut, "%u.%u.%u.%u",
		(unsigned int)(ulIP & 0x000000FF),
		(unsigned int)((ulIP>>8) & 0x000000FF),
		(unsigned int)((ulIP>>16) & 0x000000FF),
		(unsigned int)((ulIP>>24) & 0x000000FF));
	return iRt;
}

long String2Long(const char *pcString)
{
	long lRt;
	if (pcString == NULL) lRt = 0;
	else lRt = atoi(pcString);
	return lRt;
}

int Long2String(long lVal, char *pcStringOut)
{
	if (pcStringOut == NULL) return -1;
	return sprintf(pcStringOut, "%d", (int)lVal);
}

BOOL String2Bool(const char *pcString)
{
	char *pTrim;
	BOOL bRt;

	if (pcString == NULL) return FALSE;

	pTrim = GetTrimString(pcString);
	if (pTrim==NULL) pTrim = (char *)pcString;

	if (strcasecmp(pTrim, "TRUE") == 0
		|| strcasecmp(pTrim, "YES") == 0
		|| strcasecmp(pTrim, "T") == 0
		|| strcasecmp(pTrim, "ON") == 0
		|| strcasecmp(pTrim, "1") == 0)
		bRt=TRUE;
	else bRt = FALSE;

	if (pTrim != pcString) free(pTrim);
	return bRt;
}

int Bool2String(BOOL bVal, char *pcStringOut)
{
	if (pcStringOut == NULL) return -1;
	return sprintf(pcStringOut, (bVal?"true":"false"));
}

char __ishexchar(char ch)
{
	if ((ch>='0' && ch<='9')
		|| (ch>='a' && ch<='f')
		|| (ch>='A' && ch<='F')) return 1;
	else return 0;
}

char __hex2char(char ch)
{
	if (ch>='0' && ch<='9') ch -= '0';
	else if (ch>='a' && ch<='f') ch = ch - 'a' + 10;
	else if (ch>='A' && ch<='F') ch = ch - 'A' + 10;
	return ch;
}

char Hex2Char(const char *str)
{
	const char *pc;
	char c1, c2;

	for (pc = str; *pc != '\0'; pc++)
		if (__ishexchar(*pc)) break;
	if (*pc == '\0') return '\0';

	if (__ishexchar(*(pc+1)))
	{
		c1 = __hex2char(*pc);
		c2 = __hex2char(*(pc+1));
	}
	else
	{
		c1 = '\0';
		c2 = __hex2char(*pc);
	}
	return ((c1<<4)&0xf0) + (c2&0x0f);
}

int Char2Hex(char ch, char *pcRtHex)
{
	char c1, c2;
	c1 = (ch >> 4) & 0x0F;
	c2 = ch & 0x0F;
	if (c1 > '\11') c1 = c1 + 'A' - '\12';
	else c1 += '0';
	if (c2 > '\11') c2 = c2 + 'A' - '\12';
	else c2 += '0';
	pcRtHex[0] = c1;
	pcRtHex[1] = c2;
	pcRtHex[2] = '\0';
	return 2;
}

LISTNODE *IsExistParam(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pParseItem;

	if (pList == NULL) return NULL;
	if (pcName == NULL) pcName = "";

	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue != NULL)
		{
			pParseItem = (NAMEDSTRING_T *)pNode->pValue;
			if (strcmp(pParseItem->pcName, pcName) == 0)
				return pNode;
		}
	}
	return NULL;
}

const char *GetString(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;
	static const char *pcRtNull="";

	pNode = IsExistParam(pList, pcName);

	if (pNode == NULL || pNode->pValue == NULL) return pcRtNull;
	else return ((NAMEDSTRING_T *)pNode->pValue)->pcValue;
}

unsigned long GetIP(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;
	static const char *pcRtNull="";

	pNode = IsExistParam(pList, pcName);

	if (pNode == NULL || pNode->pValue == NULL) return 0L;
	else return String2IP(((NAMEDSTRING_T *)pNode->pValue)->pcValue);
}

unsigned long GetIP4(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pParseItem;
	unsigned long lRtIP;
	int i;

	for (lRtIP = 0, i = 0, pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue != NULL)
		{
			pParseItem = (NAMEDSTRING_T *)pNode->pValue;
			if (strcmp(pParseItem->pcName, pcName) == 0)
			{
				if (pParseItem->pcValue)
				{
					lRtIP |= (atoi(pParseItem->pcValue) & 0x000000ff) << i*8;
				}
				i++;
				if (i>=4) break;
			}
		}
	}
	return lRtIP;
}

long GetLong(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;

	pNode = IsExistParam(pList, pcName);

	if (pNode == NULL || pNode->pValue == NULL) return 0L;
	else return String2Long(((NAMEDSTRING_T *)pNode->pValue)->pcValue);
}

BOOL GetBool(const LIST *pList, const char *pcName)
{
	LISTNODE *pNode;

	pNode = IsExistParam(pList, pcName);
	if (pNode == NULL || pNode->pValue == NULL) return FALSE;
	else return String2Bool(((NAMEDSTRING_T *)pNode->pValue)->pcValue);
}

LISTNODE *SetString(LIST *pList, const char *pcName, const char *pcValue)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pParseItem;

	if (pList == NULL) return NULL;
	if (pcName == NULL) pcName = "";
	if (pcValue == NULL) pcValue = "";

	pNode = IsExistParam(pList, pcName);

	if (pNode == NULL)
	{
		pNode = AppendNode(pList);

		if (pNode == NULL) return NULL;
		if ((pNode->pValue = malloc(sizeof(NAMEDSTRING_T))) == NULL)
		{
			PRINT_MEM_OUT;
			DeleteNode(pNode);
			return NULL;
		}
		((NAMEDSTRING_T *)pNode->pValue)->pcValue = NULL;
		if ((((NAMEDSTRING_T *)pNode->pValue)->pcName = strdup(pcName)) == NULL)
		{
			free(pNode->pValue);
			DeleteNode(pNode);
			return NULL;
		}
	}

	pParseItem = (NAMEDSTRING_T *)pNode->pValue;
	if (pParseItem->pcValue != NULL) free(pParseItem->pcValue);
	pParseItem->pcValue = strdup(pcValue);

	return pNode;
}

LISTNODE *SetIP(LIST *pList, const char *pcName, unsigned long ulValue)
{
	LISTNODE *pNode;
	pNode = SetString(pList, pcName, "1234567890123456"); /* 给pcName预留17个字节空间 */
	if (pNode == NULL) return NULL;
	IP2String(ulValue, ((NAMEDSTRING_T *)pNode->pValue)->pcValue);
	return pNode;
}

LISTNODE *SetLong(LIST *pList, const char *pcName, long lValue)
{
	LISTNODE *pNode;
	pNode = SetString(pList, pcName, "123456789012"); /* 给pcName预留13个字节空间 */
	if (pNode == NULL) return NULL;
	sprintf(((NAMEDSTRING_T *)pNode->pValue)->pcValue, "%d", lValue); /* UINT32转换成字符串不会超过12字节，包含'\0'在内 */
	return pNode;
}

LISTNODE *SetBool(LIST *pList, const char *pcName, BOOL bValue)
{
	return SetString(pList, pcName, (bValue?"1":"0"));
}

void DeleteNamedString(NAMEDSTRING_T *pNStr)
{
	if (pNStr != NULL)
	{
		if (pNStr->pcName != NULL) free(pNStr->pcName);
		if (pNStr->pcValue != NULL) free(pNStr->pcValue);
		free(pNStr);
	}
}

LIST *CreateDict()
{
	return CreateList();
}

void DeleteDict(LIST *pList)
{
	LISTNODE *pNode;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
		DeleteNamedString(pNode->pValue);
	DeleteList(pList);
}

BOOL DelDictParam(LIST *pList, const char *pcName)
{
	LISTNODE *pNode;
	pNode = IsExistParam(pList, pcName);
	if (pNode != NULL)
	{
		DeleteNamedString(pNode->pValue);
		DeleteNode(pNode);
		return TRUE;
	}
	return FALSE;
}






/* Get the parent XML that contains "pXML". */
inline XML *GetParentXML(XML *pXML)
{
	if (pXML == NULL) return NULL;
	return pXML->pParXML;
}



XML *__CreateXML(const char *pcName, BOOL bPureText)
{
	XML *pXML = (XML *)malloc(sizeof(XML));
	if (pXML == NULL) goto lE_malloc;
	memset(pXML, 0, sizeof(XML));

	if (pcName == NULL) pcName = "XML_Name";

	pXML->pcName = strdup(pcName);
	if (pXML->pcName == NULL) goto lE_strdup;
	
	if (bPureText) return pXML;
	
	pXML->plSubXML = CreateList();
	if (pXML->plSubXML == NULL) goto lE_CreateList;
	pXML->plAttrib = CreateDict();
	if (pXML->plAttrib == NULL) goto lE_CreateDict;
	return pXML;

lE_CreateDict:
	DeleteList(pXML->plSubXML);
lE_CreateList:
	free(pXML->pcName);
lE_strdup:
	free(pXML);
lE_malloc:
	return NULL;
}

/* Create a new XML */
XML *CreateXML(const char *pcName)
{
	__CreateXML(pcName, FALSE);
}

XML *CreateXMLText(const char *pcText)
{
	__CreateXML(pcText, TRUE);
}

/* Detete a existing XML */
void DeleteXML(XML *pXML)
{
	LISTNODE *pNode;
	LISTNODE *pNextNode;

	if (pXML == NULL) return;

	if (pXML->pParXMLNode != NULL)
		DeleteNode(pXML->pParXMLNode);
	
	if (pXML->plSubXML != NULL)
	{
		for (pNode = pXML->plSubXML->pFirstNode;
			pNode != pXML->plSubXML->pLastNode;
			pNode = pNextNode)
		{
			//pNode is meanless after DeleteXML((XML *)pNode->pValue);
			pNextNode = pNode->pNextNode;
			DeleteXML((XML *)pNode->pValue);
		}

		DeleteList(pXML->plSubXML);
	}
	
	if (pXML->plAttrib != NULL)
		DeleteDict(pXML->plAttrib);
	
	free(pXML->pcName);
	free(pXML);
}


static XML *__InsertXML(XML *pParXML, LISTNODE *pXMLNode, XML *pInsert,
	LISTNODE *(*pfunInsertor)(LISTNODE *pNode))
{
	LISTNODE *pNode;
	XML *pNew;
	
	if (pParXML == NULL) return NULL;
	if (pXMLNode == NULL) return NULL;
	
	pNode = (pfunInsertor)(pXMLNode);
	if (pNode == NULL) goto lE_InsertNodeBefore;
	
	if (pInsert != NULL) pNew = pInsert;
	else pNew = CreateXML(NULL);
	if (pNew == NULL) goto lE_CreateXML;
	
	pNew->pParXML = pParXML;
	pNew->pParXMLNode = pNode;
	pNode->pValue = (void *)pNew;
	return pNew;
	
lE_CreateXML:
	DeleteNode(pNode);
lE_InsertNodeBefore:
	return NULL;
}


/* Insert pInsert before pXML, pXML must be a sub node of an XML
 * Return pInsert if succeed, otherwise NULL.
 */
XML *InsertXMLBefore(XML *pXML, XML *pInsert)
{
	if (pXML == NULL) return NULL;
	return __InsertXML(pXML->pParXML, pXML->pParXMLNode, pInsert, InsertNodeBefore);
}

/* Insert pInsert after pXML, pXML must be a sub node of an XML
 * Return pInsert if succeed, otherwise NULL.
 */
XML *InsertXMLAfter(XML *pXML, XML *pInsert)
{
	if (pXML == NULL) return NULL;
	return __InsertXML(pXML->pParXML, pXML->pParXMLNode, pInsert, InsertNodeAfter);
}

/* Append pInsert to pXML, pXML is to be the parent of pAppend
 * Return pInsert if succeed, otherwise NULL.
 */
XML *AppendXML(XML *pXML, XML *pSubXML)
{
	if (pXML == NULL) return NULL;
	return __InsertXML(pXML, pXML->plSubXML->pLastNode, pSubXML, InsertNodeBefore);
}

XML *DetachXML(XML *pXML, XML *pSubXML)
{
	if (pXML == NULL || pSubXML == NULL) return NULL;
	DeleteNode(pSubXML->pParXMLNode);
	pSubXML->pParXML = NULL;
	pSubXML->pParXMLNode = NULL;
	return pSubXML;
}



static void PT(const char *s, void *pParam)
{
	printf("%s", s);
}

static inline PTN(const char *s, int iCount, void (*Dumper)(const char *, void *), void *pParam)
{
	int i;
	for (i = 0; i < iCount; i++)
		(*Dumper)(s, pParam);
}

static void __DumpXML(XML *pXML, int iTabs, void (*Dumper)(const char *, void *), void *pParam)
{
	BOOL bAttrib;
	BOOL bSub;
	LISTNODE *pNode;
	if (pXML == NULL) return;
	
	PTN("    ", iTabs, Dumper, pParam);
	if (pXML->plAttrib == NULL)
	{
		(*Dumper)(pXML->pcName, pParam);
		(*Dumper)("\n", pParam);
		return;
	}
	
	
	(*Dumper)("<", pParam);
	(*Dumper)(pXML->pcName, pParam);
	
	if (pXML->plAttrib->pFirstNode != pXML->plAttrib->pLastNode)
	{
		(*Dumper)("\n", pParam);
		for (pNode = pXML->plAttrib->pFirstNode;
			pNode != pXML->plAttrib->pLastNode;
			pNode = pNode->pNextNode)
		{
			char *pcWebStr;
			NAMEDSTRING_T *pNStr = (NAMEDSTRING_T *)pNode->pValue;
			if (pNStr == NULL) continue;
			PTN("    ", iTabs + 1, Dumper, pParam);
			(*Dumper)(pNStr->pcName, pParam);
			(*Dumper)("=", pParam);
			pcWebStr = GetWebStyleString(pNStr->pcValue);
			if (pcWebStr != NULL)
			{
				(*Dumper)("\"", pParam);
				(*Dumper)(pcWebStr, pParam);
				(*Dumper)("\"", pParam);
				free(pcWebStr);
			}
			else (*Dumper)(pNStr->pcValue, pParam);
			(*Dumper)("\n", pParam);
		}
		PTN("    ", iTabs, Dumper, pParam);
	}

	if (pXML->plSubXML->pFirstNode == pXML->plSubXML->pLastNode)
	{
		if (pXML->plAttrib->pFirstNode != pXML->plAttrib->pLastNode)
			(*Dumper)("/>\n", pParam);
		else
			(*Dumper)(" />\n", pParam);
	}
	else
	{
		if (pXML->plSubXML->pFirstNode->pNextNode == pXML->plSubXML->pLastNode
			&& ((XML *)pXML->plSubXML->pFirstNode->pValue)->plAttrib == NULL)
		{
			(*Dumper)(">", pParam);
			(*Dumper)(((XML *)pXML->plSubXML->pFirstNode->pValue)->pcName, pParam);
		}
		else
		{
			(*Dumper)(">\n", pParam);
	
			for (pNode = pXML->plSubXML->pFirstNode;
				pNode != pXML->plSubXML->pLastNode;
				pNode = pNode->pNextNode)
				__DumpXML((XML *)pNode->pValue, iTabs + 1, Dumper, pParam);
			PTN("    ", iTabs, Dumper, pParam);
		}
		(*Dumper)("</", pParam);
		(*Dumper)(pXML->pcName, pParam);
		(*Dumper)(">\n", pParam);
	}
}

void DumpXML(XML *pXML, void (*Dumper)(const char *, void *), void *pParam)
{
	if (Dumper == NULL) Dumper = PT;
	return __DumpXML(pXML, 0, Dumper, pParam);
}
