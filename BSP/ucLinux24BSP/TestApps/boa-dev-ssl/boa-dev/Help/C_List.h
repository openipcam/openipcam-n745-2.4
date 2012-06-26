#ifndef LIST_H
#define LIST_H

#define PTE do {fprintf(stderr, "\33[1m\33[41mError: \33[0m \33[1m\33[36m%s\33[0m, \33[1m\33[36m%d\33[0m.\n", __FILE__, __LINE__);} while(0)
#define PTI do {printf("\33[1m\33[32mRun:\33[0m \33[1m\33[36m%s\33[0m, \33[1m\33[36m%d\33[0m.\n", __FILE__, __LINE__);} while(0)
#define PAZ do {fprintf(stderr, "\33[1m\33[33mPause:\33[0m \33[1m\33[36m%s\33[0m, \33[1m\33[36m%d\33[0m.\n", __FILE__, __LINE__); getc(stdin);} while(0)
#define PRINT_MEM_OUT do {fprintf(stderr, "Not enough memory in %s %d.\n", __FILE__, __LINE__);} while(0)
//#define PRINT_MEM_OUT

struct tagLIST;
typedef struct tagLISTNODE
{
	void *pValue;
	struct tagLISTNODE *pPreNode;
	struct tagLISTNODE *pNextNode;
	struct tagLIST *pList;
} LISTNODE;

typedef struct tagLIST
{
	LISTNODE *pFirstNode;
	LISTNODE *pLastNode;
} LIST;


LIST *CreateList( void );
void DeleteList(LIST *ppList);
void DeleteNode(LISTNODE *pNode);
LISTNODE *InsertNodeAfter(LISTNODE *pNode);
LISTNODE *InsertNodeBefore(LISTNODE *pNode);
LISTNODE *AppendNode(LIST *pList);
LISTNODE *GetNodeAt(const LIST *pList, int iIndex);
int GetNodeIndex(const LISTNODE *pNode);
int GetListLength(const LIST *pList);

#endif
