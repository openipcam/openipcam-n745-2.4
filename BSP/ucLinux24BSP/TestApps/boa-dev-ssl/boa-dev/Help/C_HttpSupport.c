#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "HttpServer.h"
#include "C_MultiPart.h"
#include "C_HttpSupport.h"


LIST *ParseString(const char *pcString)
{
	LIST *pStringList;
	LIST *pReturnList;
	LISTNODE *pStringNode;
	LISTNODE *pReturnNode;
	char *pcEqual;
	NAMEDSTRING_T *pParseItem;

	if (pcString == NULL) return NULL;

	pStringList = SplitString(pcString, '&');
	if (pStringList == NULL) return NULL;
	pReturnList = CreateList();
	if (pReturnList == NULL)
	{
		DeleteSplitString(pStringList);
		return NULL;
	}

	for (pStringNode = pStringList->pFirstNode;
		pStringNode != pStringList->pLastNode;
		pStringNode = pStringNode->pNextNode)
	{
		if (pStringNode->pValue == NULL) continue;

		pcEqual = strchr((char *)pStringNode->pValue, '=');
		if (pcEqual != NULL) *(pcEqual++) = '\0';

		pReturnNode = AppendNode(pReturnList);
		if (pReturnNode == NULL)
			break;

		pReturnNode->pValue = (void *)malloc(sizeof(NAMEDSTRING_T));
		if (pReturnNode->pValue != NULL)
		{
			pParseItem = (NAMEDSTRING_T *)pReturnNode->pValue;
			pParseItem->pcName = NULL;
			pParseItem->pcValue = NULL;
			pParseItem->pcName = strdup((char *)pStringNode->pValue);
			if (pParseItem->pcName == NULL)
			{
				free(pReturnNode->pValue);
				DeleteNode(pReturnNode);
				break;
			}

			else unescape_uri(pParseItem->pcName);
			if(pcEqual != NULL)
			{
				pParseItem->pcValue = strdup(pcEqual);
				if (pParseItem->pcValue != NULL)
					unescape_uri(pParseItem->pcValue);
			}
			else
				pParseItem->pcValue = strdup("");

			if (pParseItem->pcValue == NULL)
			{
				free(pParseItem->pcName);
				free(pReturnNode->pValue);
				DeleteNode(pReturnNode);
				break;
			}
		}
		else
		{
			PRINT_MEM_OUT;
			break;
		}
	}
	DeleteSplitString(pStringList);
	return pReturnList;
}

void DeleteParseString(LIST *pList)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pParseItem;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue != NULL)
		{
			pParseItem = (NAMEDSTRING_T *)pNode->pValue;
			if (pParseItem->pcName) free(pParseItem->pcName);
			if (pParseItem->pcValue) free(pParseItem->pcValue);
			free(pNode->pValue);
		}
	}
	DeleteList(pList);
}

LIST *ReadQueryList(HTTPCONNECTION hc)
{
	char *pcQuery;
	LIST *pList;

	pcQuery = GetQueryString(hc);
	if (pcQuery == NULL) pcQuery = "";
	pList = ParseString(pcQuery);
	return pList;
}

void DeleteQueryList(LIST *pList)
{
	DeleteParseString(pList);
}

char *GetBoundary(char *pcContentType)
{
	LIST *pList;
	const char *pcBoundary;
	char *pcRt;
	pcRt = NULL;

	pList = ParseDescriptionLine(pcContentType, strlen(pcContentType));
	if (pList == NULL) return NULL;
	pcBoundary = GetString(pList, "boundary");

	pcRt = strdup(pcBoundary);
	DeleteDescriptionLine(pList);
	return pcRt;
}

char *GetInnerPartName(INNERPART_T *pInner)
{
	LISTNODE *pNode;
	char *pcRt;

	if (pInner == NULL || pInner->plPartDiscription == NULL)
		return NULL;

	for (pNode = pInner->plPartDiscription->pFirstNode; pNode != pInner->plPartDiscription->pLastNode; pNode = pNode->pNextNode)
	{
		LISTNODE *pCheckExist;
		int iLen;
		const char *pc;

		pCheckExist = IsExistParam((LIST *)pNode->pValue, "name");
		pc = NULL;
		if (pCheckExist != NULL && pCheckExist->pValue != NULL)
			pc = ((NAMEDSTRING_T *)pCheckExist->pValue)->pcValue;

		if (pc != NULL)
		{
			iLen = strlen(pc);
			if (iLen >=2 && *pc == '\"' && *(pc+iLen-1) == '\"')
			{
				iLen -= 2;
				pc++;
			}

			pcRt = (char *)malloc(iLen > 7 ? iLen + 1 : 8);
			if (pcRt != NULL)
			{
				memcpy(pcRt, pc, iLen);
				pcRt[iLen] = '\0';
			}
			else PRINT_MEM_OUT;
			return pcRt;
		}
	}

	return NULL;
}

char *GetMultiPartString(LIST *pListMt, char *pcName, int *iLen)
{
	LISTNODE *pNodeMt;
	LIST *pList;
	LISTNODE *pNode;
	LISTNODE *pNodeInnerDes;

	for (pNodeMt = pListMt->pFirstNode; pNodeMt != pListMt->pLastNode; pNodeMt = pNodeMt->pNextNode)
	{
		char *pcInnerPartName;

		if (pNodeMt->pValue == NULL) continue;
		pcInnerPartName = GetInnerPartName((INNERPART_T *)pNodeMt->pValue);
		if (pcInnerPartName != NULL)
		{
			int rt;
			rt = strcmp(pcInnerPartName, pcName);
			free(pcInnerPartName);
			if (rt == 0)
			{
				*iLen = ((INNERPART_T *)pNodeMt->pValue)->iPartBodyLength;
				if (*iLen < 0) *iLen = 0;

				return ((INNERPART_T *)pNodeMt->pValue)->pcPartBody;
			}
		}
	}
	return NULL;
}

int HttpUpload(HTTPCONNECTION hConnection, char *pcReceiveBuf, int iReceiveLen, char *pcDefaultSavePath, char **ppcSavePath, LIST **ppFileList)
{
	char *pcBoundary;

	LIST *pListMt;
	LISTNODE *pNodeMt;
	LIST *pList;
	LISTNODE *pNode;

	int iNewPathLen;
	char *pcNewPath;

	int iSuccessSavedFile;
	int iFileShouldSave;
	iSuccessSavedFile = 0;
	iFileShouldSave = 0;

	if (ppcSavePath != NULL) *ppcSavePath = 0;
	if (ppFileList) *ppFileList = CreateList();
	if (pcReceiveBuf == NULL || iReceiveLen <= 0) return -2;

	if (GetHttpMethod(hConnection) != M_POST) return -1;
	pcBoundary = GetContentType(hConnection);
	if (pcBoundary != NULL)
		pcBoundary = GetBoundary(pcBoundary);
	if (pcBoundary == NULL)	return -1;

	pListMt = ParseMultiPart(pcReceiveBuf, iReceiveLen, pcBoundary);
	free(pcBoundary);
	if (pListMt == NULL)
		return -1;

	pcNewPath = GetMultiPartString(pListMt, "TargetPath", &iNewPathLen);
	if (iNewPathLen > 128) iNewPathLen = 128;

	if (ppcSavePath)
	{
		*ppcSavePath = malloc(iNewPathLen+2);
		if (*ppcSavePath)
		{
			memcpy(*ppcSavePath, pcNewPath, iNewPathLen);
			(*ppcSavePath)[iNewPathLen] = '\0';
		}
	}

	for (pNodeMt = pListMt->pFirstNode; pNodeMt != pListMt->pLastNode; pNodeMt = pNodeMt->pNextNode)
	{
		if (pNodeMt->pValue == NULL) continue;
		pList = ((INNERPART_T *)pNodeMt->pValue)->plPartDiscription;
		if (pList == NULL) continue;
		for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
		{
			LISTNODE *pNodeInnerDes;
			LIST *pListInnerDes;
			pListInnerDes = pNode->pValue;
			if (pListInnerDes == NULL) continue;
			for (pNodeInnerDes = pListInnerDes->pFirstNode;
				pNodeInnerDes != pListInnerDes->pLastNode;
				pNodeInnerDes = pNodeInnerDes->pNextNode)
			{
				NAMEDSTRING_T *ds;
				ds = (NAMEDSTRING_T *)pNodeInnerDes->pValue;
				if (strcmp(ds->pcName, "filename") == 0)
				{
					FILE *fp;
					char *pcFileNameBegin;
					char *pcFileNameEnd;
					char pcPath[256];

					iFileShouldSave++;

					if (pcNewPath != NULL && iNewPathLen > 0)
					{
						memcpy(pcPath, pcNewPath, iNewPathLen);
						pcPath[iNewPathLen] = '\0';
					}
					else strcpy(pcPath, pcDefaultSavePath);//xhchen - modify someday!

					if (pcPath[strlen(pcPath)-1] != '\\' && pcPath[strlen(pcPath)-1] != '/')
						strcat(pcPath, "/");

					for (pcFileNameBegin=ds->pcValue+strlen(ds->pcValue)-1; pcFileNameBegin>=ds->pcValue; pcFileNameBegin--)
						if (*pcFileNameBegin == '\\' || *pcFileNameBegin == '/')
							break;
					pcFileNameBegin++;
					for (pcFileNameEnd=pcFileNameBegin; *pcFileNameEnd!='\0'; pcFileNameEnd++)
						if (*pcFileNameEnd=='\"')
							break;
					if (pcFileNameEnd == pcFileNameBegin)
					{
						strcat(pcPath, "SaveFile.tmp");
					}
					else
					{
						int iLen;
						iLen = strlen(pcPath);
						memcpy(pcPath+iLen, pcFileNameBegin, pcFileNameEnd-pcFileNameBegin);
						pcPath[iLen+(pcFileNameEnd-pcFileNameBegin)] = '\0';
					}

					fp = fopen(pcPath, "wb");
					if (!fp)
						fprintf(stderr, "Cannot save file!\n");
					else
					{
						fwrite(((INNERPART_T *)pNodeMt->pValue)->pcPartBody,
							((INNERPART_T *)pNodeMt->pValue)->iPartBodyLength, 1, fp);
						fclose(fp);
						if (ppFileList && *ppFileList)
						{
							LISTNODE *pNode;
							pNode = AppendNode(*ppFileList);
							if (pNode)
								pNode->pValue = (void *)strdup(pcPath);
						}
						iSuccessSavedFile++;
					}
				}
			}
		}
	}


	DeleteMultiPart(pListMt);

	if (iSuccessSavedFile <= 0 || iFileShouldSave != iSuccessSavedFile) return -3;

	return 0;
}

void HttpUploadClear(HTTPCONNECTION hConnection, char **ppcSavePath, LIST **ppFileList)
{
	if (ppcSavePath != NULL)
		if (*ppcSavePath != NULL) free(*ppcSavePath);

	if (ppFileList != NULL)
	{
		if (*ppFileList != NULL)
		{
			LISTNODE *pNode;
			for (pNode = (*ppFileList)->pFirstNode; pNode != (*ppFileList)->pLastNode; pNode = pNode->pNextNode)
				if (pNode->pValue != NULL) free(pNode->pValue);
			DeleteList(*ppFileList);
		}
	}
}
