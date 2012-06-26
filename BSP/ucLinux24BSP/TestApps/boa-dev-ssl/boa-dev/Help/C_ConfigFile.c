#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "C_List.h"
#include "C_String.h"
#include "C_ConfigFile.h"


/* 以#开头的行(前导空白不计)或空白行都将得到FALSE */
BOOL IsValidLine(char *pcThisLinePos, char *pcNextLinePos);

/* 检查该行是否为pcSegName为标题的段标题
** pcSegName为NULL时，只要该行为段标题行，都将返回TRUE */
BOOL IsSegTitle(char *pcThisLinePos, char *pcNextLinePos, const char *pcSegName);

/* 从一行中复制Name, Value对到NAMEDSTRING_T结构中，用完要FREE，复制完的Name,Value经去处前后空白处理
** 如该行不合法（无Name,注释#行，空白行等），则返回NULL */
NAMEDSTRING_T *CopyItem(char *pcThisLinePos, char *pcNextLinePos);
/* 释放CopyItem(...)分配的资源 */
void FreeCopyedItem(NAMEDSTRING_T *pItem);

BOOL ReadWholeFile(const char *pcFileName, char **ppcFileBuf, int *piFileLength)
{
	FILE *fp;
	int iLen;
	int i;
	struct stat sb;

	if (piFileLength != NULL) *piFileLength = 0;
	if ( stat( pcFileName, &sb ) != 0 )
	{
		return FALSE;
	}

	if (S_ISDIR(sb.st_mode)) return FALSE;

	if (sb.st_size == 0)
	{/* may be it's a temp file, read in another way */
		fp = fopen(pcFileName, "rb");
		if (!fp) return FALSE;
		else
		{

			if (piFileLength != NULL) *piFileLength = 0;
			for (i=0, *ppcFileBuf = NULL; ; i++)
			{
				char *pcNewFileBuf;
				pcNewFileBuf = realloc(*ppcFileBuf, (i+1)*256);
				if (pcNewFileBuf == NULL)
				{
					free(*ppcFileBuf);
					fclose(fp);
					*ppcFileBuf = NULL;
					PRINT_MEM_OUT;
					return FALSE;
				}
				else
					*ppcFileBuf = pcNewFileBuf;

				iLen = fread(*ppcFileBuf+i*256, 1, 256, fp);
				if (piFileLength != NULL) *piFileLength += iLen;
				if (iLen < 256) break;
			}

			fclose(fp);
			return TRUE;
		}
	}

	*ppcFileBuf = malloc(sb.st_size+1);
	if (*ppcFileBuf == NULL)
	{
		PRINT_MEM_OUT;
		return FALSE;
	}

	fp = fopen(pcFileName, "rb");
    if (!fp)
	{
		free(*ppcFileBuf);
		*ppcFileBuf = NULL;
		return FALSE;
	}

	iLen = fread(*ppcFileBuf, 1, sb.st_size, fp);
	((char *)(*ppcFileBuf))[iLen] = '\0';

	if (piFileLength != NULL)
		*piFileLength = iLen;
	fclose(fp);
	return TRUE;
}

BOOL ReadLineFromFileBuf(char *pcFileBuf, int iFileLength, char *pcThisLinePos, char **ppcNextLinePos)
{
	char *pcFileTail = NULL;

	pcFileTail = pcFileBuf + iFileLength;
	if (pcThisLinePos < pcFileBuf
		|| pcThisLinePos >= pcFileTail)
		return FALSE;

	for (*ppcNextLinePos=pcThisLinePos; *ppcNextLinePos < pcFileTail; (*ppcNextLinePos)++)
	{
		if (**ppcNextLinePos == '\n')
		{
			(*ppcNextLinePos)++;
			break;
		}
	}
	return TRUE;
}

BOOL IsValidLine(char *pcThisLinePos, char *pcNextLinePos)
{
	char *pc;
	if (pcThisLinePos >= pcNextLinePos) return FALSE;
	for (pc=pcThisLinePos; pc<pcNextLinePos; pc++)
	{
		if (*pc=='#') return FALSE;
		if (!((signed char)*pc >= 0 && (signed char)*pc <= 32)) return TRUE;
	}
	return FALSE;
}

BOOL IsSegTitle(char *pcThisLinePos, char *pcNextLinePos, const char *pcSegName)
{
	char *pcB;
	char *pcE;
	if (!IsValidLine(pcThisLinePos, pcNextLinePos)) return FALSE;

	for (pcB=pcThisLinePos; pcB<pcNextLinePos; pcB++)
	{
		if ((signed char)*pcB >= 0 && (signed char)*pcB <= 32) continue;
		if (*pcB != '[') return FALSE;
		else break;
	}
	if (*pcB != '[') return FALSE;

	for (pcE=pcNextLinePos-1; pcE>=pcThisLinePos; pcE--)
	{
		if ((signed char)*pcE >= 0 && (signed char)*pcE <= 32) continue;
		if (*pcE != ']') return FALSE;
		else break;
	}
	if (*pcE != ']') return FALSE;

	if (pcSegName == NULL) return TRUE;
	else
	{
		if (strncasecmp(pcB+1, pcSegName, pcE - pcB - 1) == 0
			&& pcE - pcB - 1 == strlen(pcSegName)) return TRUE;
		else return FALSE;
	}
}

NAMEDSTRING_T *CopyItem(char *pcThisLinePos, char *pcNextLinePos)
{
	char *pc;
	char *pcDup;
	NAMEDSTRING_T *rtVal;

	if (!IsValidLine(pcThisLinePos, pcNextLinePos)) return NULL;

	/* 这儿多补充2字节，一个放'\0', 一个为可能的多加一个'='预留 */
	if ((pcDup = malloc(pcNextLinePos - pcThisLinePos + 2)) == NULL) return NULL;
	if ((rtVal = (NAMEDSTRING_T *)malloc(sizeof(NAMEDSTRING_T))) == NULL)
	{
		free(pcDup);
		return NULL;
	}

	strncpy(pcDup, pcThisLinePos, pcNextLinePos - pcThisLinePos);
	pcDup[pcNextLinePos - pcThisLinePos] = '\0';

	for (pc=pcDup; *pc!='\0'; pc++)
	{
		if (*pc == '=') break;
	}
	if (*pc == '\0')
	{ /* 没找到'=',末尾加上一个 */
		*pc = '=';
		*(pc + 1) = '\0';
	}

	*(pc++) = '\0';
	rtVal->pcName = GetTrimString(pcDup);
	if (rtVal->pcName == NULL || rtVal->pcName[0] == '\0')
	{ /* 舍弃没有名字的一行 */
		if (rtVal->pcName) free(rtVal->pcName);
		free(pcDup);
		free(rtVal);
		return NULL;
	}

	rtVal->pcValue = GetTrimString(pc);
	free(pcDup);

	return rtVal;
}

void FreeCopyedItem(NAMEDSTRING_T *pItem)
{
	if(pItem->pcName) free(pItem->pcName);
	if(pItem->pcValue) free(pItem->pcValue);
	free(pItem);
}


LIST *ReadConfigSegment(const char *pcFileName, const char *pcSegName)
{
	char *pcFileBuf;
	char *pcThisLinePos;
	char *pcNextLinePos;
	int iFileLength;
	LIST *pList;
	LISTNODE *pNode;
	NAMEDSTRING_T *pItem;

	pList = NULL;
	if (!(ReadWholeFile(pcFileName, &pcFileBuf, &iFileLength))) return NULL;

	pcThisLinePos = pcFileBuf;
	while (ReadLineFromFileBuf(pcFileBuf, iFileLength, pcThisLinePos, &pcNextLinePos))
	{
		if (IsSegTitle(pcThisLinePos, pcNextLinePos, pcSegName))
		{/* 找到所要的段 */
			if (pList == NULL) pList = CreateDict();
			if (pList == NULL)
			{
				PRINT_MEM_OUT;
				free(pcFileBuf);
				return NULL;
			}

			pcThisLinePos = pcNextLinePos;

			while (ReadLineFromFileBuf(pcFileBuf, iFileLength, pcThisLinePos, &pcNextLinePos))
			{
				if (IsSegTitle(pcThisLinePos, pcNextLinePos, NULL)) break;
				if((pItem = CopyItem(pcThisLinePos, pcNextLinePos)) != NULL)
				{
					if((pNode = AppendNode(pList)) == NULL)
					{
						FreeCopyedItem(pItem);
						break;
					}
					pNode->pValue = (void *)pItem;
				}
				pcThisLinePos = pcNextLinePos;
			}
			break;
		}
		pcThisLinePos = pcNextLinePos; /* 将buf指针移到下一行的行首 */
	}

	free(pcFileBuf);
	return pList;
}

void DeleteConfigSegmentList(LIST *pList)
{
	DeleteDict(pList);
}

void WriteConfigItem(FILE *fp, const char *pcSegName, LIST *pListConfigItem)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pItem;

	fprintf(fp,"[%s]\r\n", pcSegName);
	for (pNode = pListConfigItem->pFirstNode; pNode != pListConfigItem->pLastNode; pNode = pNode->pNextNode)
	{
		pItem = (NAMEDSTRING_T *)pNode->pValue;
		if	(pNode->pValue != NULL)
		{
			if (pItem->pcName != NULL)
				fprintf(fp, "%s=%s\r\n", pItem->pcName, (pItem->pcValue?pItem->pcValue:""));
		}
	}
	fprintf(fp, "\r\n");
}

BOOL WriteConfigSegment(const char *pcFileName, const char *pcSegName, LIST *pListConfigItem)
{
	FILE *fp;
	LISTNODE *pNode;
	NAMEDSTRING_T *pItem;

	char *pcFileBuf;
	int iFileLength;
	char *pcThisLinePos;
	char *pcNextLinePos;
	BOOL bIsWrite;

	if (!(ReadWholeFile(pcFileName, &pcFileBuf, &iFileLength)))
	{
		if (!(fp = fopen( pcFileName, "wt" ))) return FALSE;
		else
		{
			WriteConfigItem(fp, pcSegName, pListConfigItem);
			fclose(fp);
			return TRUE;
		}
	}
	else
	{
		if (!(fp = fopen( pcFileName, "wt" )))
		{
			free(pcFileBuf);
			return FALSE;
		}

		bIsWrite = FALSE;
		pcThisLinePos = pcFileBuf;
		while (ReadLineFromFileBuf(pcFileBuf, iFileLength, pcThisLinePos, &pcNextLinePos))
		{
			if (IsSegTitle(pcThisLinePos, pcNextLinePos, pcSegName))
			{/* 找到所要的段 */
				/* 跳过本段 */
				pcThisLinePos = pcNextLinePos;
				while (ReadLineFromFileBuf(pcFileBuf, iFileLength, pcThisLinePos, &pcNextLinePos))
				{
					if (IsSegTitle(pcThisLinePos, pcNextLinePos, NULL)) break;
					pcThisLinePos = pcNextLinePos;
				}
				pcNextLinePos = pcThisLinePos;

				if(!bIsWrite)
				{/* 如果没有写过该段，则在此处插入该段 */
					WriteConfigItem(fp, pcSegName, pListConfigItem);
					bIsWrite = TRUE;
				}
			}
			else
			{
				fwrite(pcThisLinePos, pcNextLinePos - pcThisLinePos, 1, fp);
			}
			pcThisLinePos = pcNextLinePos;
		}

		if(!bIsWrite)
			WriteConfigItem(fp, pcSegName, pListConfigItem);

		fclose(fp);

		free(pcFileBuf);
		return TRUE;
	}
}

BOOL ReadConfigValue(const char *pcFileName, const char *pcSegName, const char *pcVarName, char *pcVarValue, int iMaxReadSize)
{
	LIST *pList;
	LISTNODE *pNode;
	NAMEDSTRING_T *pParseItem;

	if ((pList = ReadConfigSegment(pcFileName, pcSegName)) == NULL) return FALSE;

	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue != NULL)
		{
			pParseItem = (NAMEDSTRING_T *)pNode->pValue;
			if (strcmp(pParseItem->pcName, pcVarName) == 0)
			{
				MyStrncpy(pcVarValue, pParseItem->pcValue, iMaxReadSize);
				break;
			}
		}
	}
	if (pNode == pList->pLastNode) pNode = NULL;
	DeleteConfigSegmentList(pList);
	if (pNode == NULL) return FALSE;
	else return TRUE;
}

BOOL WriteConfigValue(const char *pcFileName, const char *pcSegName, const char *pcVarName, const char *pcVarValue)
{
	LIST *pList;
	BOOL bRt;
	if ((pList = ReadConfigSegment(pcFileName, pcSegName)) == NULL)
	{
		if ((pList = CreateDict()) == NULL) return FALSE;
	}
	SetString(pList, pcVarName, pcVarValue);
	bRt = WriteConfigSegment(pcFileName, pcSegName, pList);
	DeleteConfigSegmentList(pList);
	return bRt;
}
