#include <stdio.h>
#include <stdlib.h>
#include "C_List.h"

LIST *CreateList( void )
{
	LIST *pList;

	if ((pList = (LIST*)malloc(sizeof(LIST))) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}
	if ((pList->pLastNode = (LISTNODE *)malloc(sizeof(LISTNODE))) == NULL)
	{
		PRINT_MEM_OUT;
		free(pList);
		return NULL;
	}

	pList->pLastNode->pList = pList;
	pList->pLastNode->pValue = NULL;
	pList->pLastNode->pPreNode = pList->pLastNode;
	pList->pLastNode->pNextNode = pList->pLastNode;
	pList->pFirstNode = pList->pLastNode;

	return pList;
}

void DeleteList(LIST *pList)
{
	LISTNODE *pNode;
	LISTNODE *pNextNode;
	if (pList == NULL) return;

	for (pNode = pList->pFirstNode; ; pNode = pNextNode)
	{
		pNextNode = pNode->pNextNode;
		free(pNode);
		if (pNode == pList->pLastNode) break;
	}

	free(pList);
}

void DeleteNode(LISTNODE *pNode)
{
	if (pNode == NULL) return;
	if (pNode == pNode->pList->pLastNode) return;

	pNode->pPreNode->pNextNode = pNode->pNextNode;
	pNode->pNextNode->pPreNode = pNode->pPreNode;

	pNode->pList->pFirstNode = pNode->pList->pLastNode->pNextNode;

	free(pNode);
}

LISTNODE *InsertNodeAfter(LISTNODE *pNode)
{
	LISTNODE *pNewNode;

	if (pNode == NULL) return NULL;

	if ((pNewNode = (LISTNODE *)malloc(sizeof(LISTNODE))) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	pNewNode->pList = pNode->pList;
	pNewNode->pValue = NULL;
	pNewNode->pPreNode = pNode;
	pNewNode->pNextNode = pNode->pNextNode;
	pNode->pNextNode->pPreNode = pNewNode;
	pNode->pNextNode = pNewNode;

	pNode->pList->pFirstNode = pNode->pList->pLastNode->pNextNode;

	return pNewNode;
}

LISTNODE *InsertNodeBefore(LISTNODE *pNode)
{
	LISTNODE *pNewNode;

	if (pNode == NULL) return NULL;

	if ((pNewNode = (LISTNODE *)malloc(sizeof(LISTNODE))) == NULL)
	{
		PRINT_MEM_OUT;
		return NULL;
	}

	pNewNode->pList = pNode->pList;
	pNewNode->pValue = NULL;
	pNewNode->pPreNode = pNode->pPreNode;
	pNewNode->pNextNode = pNode;
	pNode->pPreNode->pNextNode = pNewNode;
	pNode->pPreNode = pNewNode;

	pNode->pList->pFirstNode = pNode->pList->pLastNode->pNextNode;

	return pNewNode;
}

LISTNODE *AppendNode(LIST *pList)
{
	return InsertNodeBefore(pList->pLastNode);
}

LISTNODE *GetNodeAt(const LIST *pList, int iIndex)
{
	int i;
	LISTNODE *pNode;
	if (pList == NULL || iIndex < 0) return NULL;

	for (i = 0, pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
	{
		if (i++ >= iIndex) return pNode;
	}

	return NULL;
}

int GetNodeIndex(const LISTNODE *pNode)
{
	int i;
	LISTNODE *pThisNode;
	if (pNode == NULL) return -1;

	for (i = 0, pThisNode = pNode->pList->pFirstNode; pThisNode != pNode->pList->pLastNode; pThisNode = pThisNode->pNextNode)
	{
		if (pThisNode == pNode) return i;
		i++;
	}

	return -1;
}

int GetListLength(const LIST *pList)
{
	int i;
	LISTNODE *pNode;
	for (i = 0, pNode = pList->pFirstNode; pNode != pList->pLastNode; pNode = pNode->pNextNode)
		i++;
	return i;
}
