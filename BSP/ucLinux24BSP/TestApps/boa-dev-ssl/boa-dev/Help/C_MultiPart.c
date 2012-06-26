#include <stdio.h>
#include <stdlib.h>
#include "C_List.h"
#include "C_String.h"
#include "C_MultiPart.h"

static char *__SeperatorByString(char *pcBeing, char *pcEnd, char *pcCur, void *pSeperatorParam)
{
	char *pcSep;
	int i;
	pcSep = (char *)pSeperatorParam;

	for (i = 0; pcCur + i < pcEnd && pcSep[i] != '\0'; i++)
	{
		if (pcSep[i] != pcCur[i]) return NULL;
	}
	return pcCur+i;
}

static LISTNODE *InsertBufferSeg(char *pcSegBegin, char *pcCur, LISTNODE *pNode)
{
	pNode = InsertNodeAfter(pNode);
	if (pNode != NULL)
	{
		pNode->pValue = (void *)malloc(sizeof(SPLIT_ITEM_T));
		if (pNode->pValue != NULL)
		{
			SPLIT_ITEM_T *pSplit;
			pSplit = (SPLIT_ITEM_T *)pNode->pValue;
			pSplit->pcSegStart = pcSegBegin;
			pSplit->pcSegEnd = pcCur;
		}
		else
		{
			PRINT_MEM_OUT;
			DeleteNode(pNode);
			return NULL;
		}
	}

	return pNode;
}

LIST *SplitBuffer(char *pcBuffer, int iBufferLength, int iMaxReturn, char *pcSeperator(char *pcBeing, char *pcEnd, char *pcCur, void *pSeperatorParam), void *pSeperatorParam)
{
	LIST *pList;
	LISTNODE *pNode;

	char *pcCur;
	char *pcNext;
	char *pcSegBegin;
	char *pcBegin;
	char *pcEnd;

	int iSegNumber;

	if ((pList = CreateList()) == NULL) return NULL;

	if (pcBuffer == NULL || iBufferLength < 0) return pList;
	if (pcSeperator == NULL)
		pcSeperator = __SeperatorByString;

	pcBegin = pcBuffer;
	pcEnd = pcBegin + iBufferLength;
	iSegNumber = 1;

	for (pNode=pList->pFirstNode, pcSegBegin=pcBegin, pcCur=pcBegin; pcCur < pcEnd; )
	{
		if (iMaxReturn >0 && iSegNumber >= iMaxReturn)
		{
			pcCur = pcEnd;
			break;
		}
		pcNext = (*pcSeperator)(pcBegin, pcEnd, pcCur, pSeperatorParam);
		if (pcNext == NULL) pcCur++;
		else
		{
			//[pcSegBegin, pcCur)  ---> new segment
			pNode = InsertBufferSeg(pcSegBegin, pcCur, pNode);
			if (pNode == NULL)
			{
				DeleteSplitBuffer(pList);
				return NULL;
			}

			iSegNumber++;
			pcSegBegin = pcNext;
			pcCur = pcNext;
		}
	}
	InsertBufferSeg(pcSegBegin, pcCur, pNode);

	return pList;
}

void DeleteSplitBuffer(LIST *pList)
{
	LISTNODE *pNode;
	if (pList == NULL) return;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
		if (pNode->pValue) free(pNode->pValue);
	DeleteList(pList);
}

static NAMEDSTRING_T *CreateInnerDescription(char *pcInnerDescription, int iInnerDescriptionLineLen, char cSeperator)
{
	NAMEDSTRING_T *pRt;
	char *pcLine;
	int i;

	if (pcInnerDescription == 0 || iInnerDescriptionLineLen==0) return NULL;

	if (!(pcLine = (char *)malloc(iInnerDescriptionLineLen + 2))) return NULL;
	memcpy(pcLine, pcInnerDescription, iInnerDescriptionLineLen);
	pcLine[iInnerDescriptionLineLen] = '\0';

	for (i=0; i<iInnerDescriptionLineLen; i++)
		if (pcLine[i] == cSeperator) break;
	if (i >= iInnerDescriptionLineLen)
	{
		free(pcLine);
		return NULL;
	}
	pcLine[i] = '\0';

	if (!(pRt = (NAMEDSTRING_T *)malloc(sizeof(NAMEDSTRING_T))))
	{
		free(pcLine);
		return NULL;
	}

	pRt->pcName = GetTrimString(pcLine);
	pRt->pcValue = GetTrimString(pcLine+i+1);

	free(pcLine);
	return pRt;
}

static char *SeperatorDescriptionLine(char *pcBegin, char *pcEnd, char *pcCur, void *pSeperatorParam)
{
	char *pc;
	int iQuotationMarks;
	if (*pcCur == ';')
	{
		iQuotationMarks = 0;
		for (pc=pcBegin; pc<pcCur; pc++)
		{
			if (*pc == '\"') iQuotationMarks++;
		}
		if ((iQuotationMarks & 0x1) != 0x1) return pcCur+1;
	}
	return NULL;
}

LIST *ParseDescriptionLine(char *pcDescription, int iDescriptionLength)
{
	LIST *pList;
	LISTNODE *pNode;
	LIST *pDescriptionItem;
	LISTNODE *pNodeItem;

	pList = CreateList();
	if (pList == NULL) return NULL;
	pNode = pList->pFirstNode;

	pDescriptionItem = SplitBuffer(pcDescription, iDescriptionLength, 0, SeperatorDescriptionLine, NULL);
	if (pDescriptionItem == NULL)
	{
		DeleteList(pList);
		return NULL;
	}

	for (pNodeItem = pDescriptionItem->pFirstNode; pNodeItem != pDescriptionItem->pLastNode; pNodeItem = pNodeItem->pNextNode)
	{
		NAMEDSTRING_T *pInner;
		SPLIT_ITEM_T *pSplitItem;
		pSplitItem = (SPLIT_ITEM_T *)pNodeItem->pValue;

		if (pSplitItem != NULL)
		{
			pInner = CreateInnerDescription(pSplitItem->pcSegStart,
				pSplitItem->pcSegEnd - pSplitItem->pcSegStart,
				(pNodeItem == pDescriptionItem->pFirstNode?':':'='));
			if (pInner != NULL)
			{
				pNode = InsertNodeAfter(pNode);
				pNode->pValue = (void *)pInner;
			}
		}
	}

	DeleteSplitBuffer(pDescriptionItem);
	return pList;
}

void DeleteDescriptionLine(LIST *pList)
{
	LISTNODE *pNode;
	NAMEDSTRING_T *pInner;

	if (pList == NULL) return;

	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue)
		{
			pInner = (NAMEDSTRING_T *)pNode->pValue;
			if (pInner->pcName) free(pInner->pcName);
			if (pInner->pcValue) free(pInner->pcValue);
			free(pNode->pValue);
		}
	}
	DeleteList(pList);
}

char *SeperatorDescription(char *pcBegin, char *pcEnd, char *pcCur, void *pSeperatorParam)
{
	char *pc;
	if ((signed char)*pcCur >= '\0' && (signed char)*pcCur < '\40')
	{
		for (pc=pcCur-1; pc>=pcBegin; pc--)
		{
			if (*pc == ';') return NULL;
			if (!((signed char)*pc >= '\0' && (signed char)*pc < '\40')) break;
		}

		for (pc=pcCur; pc<pcEnd; pc++)
		{
			if (!((signed char)*pc >= '\0' && (signed char)*pc < '\40')) return NULL;
			if (*pc == '\n')
			{
				return pc+1;
			}
		}

	}
	return NULL;
}


LIST *ParseDescription(char *pcDescription, int iDescriptionLength)
{
	LIST *pListDescriptionLine;
	LIST *pListReturn;
	LISTNODE *pNode;
	LISTNODE *pNodeReturn;
	SPLIT_ITEM_T *pSplit;

	pListReturn = CreateList();
	if (pListReturn == NULL) return NULL;
	pListDescriptionLine = SplitBuffer(pcDescription, iDescriptionLength, 0, SeperatorDescription, NULL);
	if (pListDescriptionLine == NULL)
	{
		DeleteList(pListReturn);
		return NULL;
	}

	for (pNodeReturn = pListReturn->pFirstNode, pNode = pListDescriptionLine->pFirstNode;
		pNode != pListDescriptionLine->pLastNode;
		pNode = pNode->pNextNode)
	{
		pSplit = (SPLIT_ITEM_T *)pNode->pValue;
		if (pSplit != NULL)
		{
			LIST *pListLine;
			pListLine = ParseDescriptionLine(pSplit->pcSegStart, pSplit->pcSegEnd-pSplit->pcSegStart);
			if (pListLine != NULL)
			{
				pNodeReturn = InsertNodeAfter(pNodeReturn);
				if (pNodeReturn != NULL)
				{
					pNodeReturn->pValue = pListLine;
				}
				else
				{
					DeleteDescriptionLine(pListLine);
					DeleteDescription(pListReturn);
					return NULL;
				}
			}
		}
	}

	DeleteSplitBuffer(pListDescriptionLine);
	return pListReturn;
}

void DeleteDescription(LIST *pList)
{
	LISTNODE *pNode;
	if (pList == NULL) return;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue)
		{
			DeleteDescriptionLine(pNode->pValue);
		}
	}
	DeleteList(pList);
}


char *SeperatorSinglePart(char *pcBegin, char *pcEnd, char *pcCur, void *pSeperatorParam)
{
	if (*pcCur == '\r' || *pcCur == '\n')
	{
		if (pcCur-1 < pcBegin || *(pcCur-1) == '\n')
		{
			if (*pcCur == '\r' && pcCur+1 < pcEnd && *(pcCur+1) == '\n')
				return pcCur+2;
			else return pcCur+1;
		}
	}
	return NULL;
}

INNERPART_T *ParseSinglePart(char *pcSinglePartSource, int iSinglePartSourceLength)
{
	INNERPART_T *pInnerPart;
	LIST *pList;
	LISTNODE *pNode;
	SPLIT_ITEM_T *pSplit;

	if ((pInnerPart = (INNERPART_T *)malloc(sizeof(INNERPART_T))) == NULL)
		return NULL;

	pInnerPart->pcPartBody = NULL;
	pInnerPart->iPartBodyLength = 0;
	pInnerPart->plPartDiscription = NULL;

	pList = SplitBuffer(pcSinglePartSource, iSinglePartSourceLength, 2, SeperatorSinglePart, NULL);
	if (pList == NULL)
	{
		free(pInnerPart);
		return NULL;
	}

	pNode = pList->pFirstNode;
	if (pNode != pList->pLastNode)
	{
		pSplit = (SPLIT_ITEM_T *)pNode->pValue;
		if (pSplit != NULL)
			pInnerPart->plPartDiscription = ParseDescription(pSplit->pcSegStart, pSplit->pcSegEnd - pSplit->pcSegStart);
	}
	pNode = pNode->pNextNode;
	if (pNode != pList->pLastNode)
	{
		pSplit = (SPLIT_ITEM_T *)pNode->pValue;
		if (pSplit != NULL)
		{
			pInnerPart->pcPartBody = pSplit->pcSegStart;
			pInnerPart->iPartBodyLength = pSplit->pcSegEnd - pSplit->pcSegStart;
		}
	}
	DeleteSplitBuffer(pList);

	return pInnerPart;
}

void DeleteSinglePart(INNERPART_T *pInnerPart)
{
	if (pInnerPart == NULL) return;
	DeleteDescription(pInnerPart->plPartDiscription);
	free(pInnerPart);
}


LIST *ParseMultiPart(char *pcMultiPartSource, int iMultiPartSourceLength, char *pcBoundary)
{
	LIST *pListSplit;
	LISTNODE *pSplitNode;
	LIST *pListReturn;
	LISTNODE *pNode;
	char *pcSep;

	if (pcBoundary == NULL) return NULL;
	if ((pcSep = (char *)malloc(strlen(pcBoundary)+4)) == NULL)
		return NULL;
	memcpy(pcSep+2, pcBoundary, strlen(pcBoundary)+1);
	pcSep[0] = '-';
	pcSep[1] = '-';

	if ((pListReturn = CreateList()) == NULL)
	{
		free(pcSep);
		return NULL;
	}

	pListSplit = SplitBuffer(pcMultiPartSource, iMultiPartSourceLength, 0, NULL, pcSep);
	free(pcSep);
	if (pListSplit == NULL
		|| (pSplitNode = pListSplit->pFirstNode) == pListSplit->pLastNode)
	{
		DeleteList(pListReturn);
		return NULL;
	}

	for (pNode = pListReturn->pFirstNode, pSplitNode = pSplitNode->pNextNode;
		pSplitNode != pListSplit->pLastNode;
		pSplitNode = pSplitNode->pNextNode)
	{
		SPLIT_ITEM_T *pSplitItem;
		INNERPART_T *pInnerPart;
		char *pc;

		pSplitItem = (SPLIT_ITEM_T *)pSplitNode->pValue;
		if (pSplitItem->pcSegEnd - pSplitItem->pcSegStart <= 2
			|| (*(pSplitItem->pcSegStart) == '-' && *(pSplitItem->pcSegStart + 1) == '-'))
			break;

		if (pSplitItem->pcSegEnd - pcMultiPartSource > 2)
		{
			if (*(pSplitItem->pcSegEnd-1) == '\n' && *(pSplitItem->pcSegEnd-2) == '\r')
				pSplitItem->pcSegEnd -= 2;
			else if (*(pSplitItem->pcSegEnd-1) == '\n')
				pSplitItem->pcSegEnd -= 1;
		}
		for (pc = pSplitItem->pcSegStart; pc < pSplitItem->pcSegEnd; pc++)
		{
			if (*pc == '\n')
			{
				pc++;
				break;
			}
		}
		pInnerPart = ParseSinglePart(pc, pSplitItem->pcSegEnd - pc);

		pNode = InsertNodeAfter(pNode);
		if (pNode != NULL)
			pNode->pValue = pInnerPart;
		else DeleteSinglePart(pInnerPart);
	}

	DeleteSplitBuffer(pListSplit);
	return pListReturn;
}

void DeleteMultiPart(LIST *pList)
{
	LISTNODE *pNode;
	if (pList == NULL) return;
	for (pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (pNode->pValue)
			DeleteSinglePart(pNode->pValue);
	}
	DeleteList(pList);
}









////////////////////////////////////////////////////////////////////////////////////
