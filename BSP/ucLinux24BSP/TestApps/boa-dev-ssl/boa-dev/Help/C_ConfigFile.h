#ifndef C_CONFIGFILE_H
#define C_CONFIGFILE_H

/* 根据pcSegment指定的段读取文件内容，结果放在一个链表中,
** 该链表中每个节点元素为NAMEDSTRING_T(Name,Value对)类型
eg: 示例文件config.ini如下(不包括---------------------)：
-------------------------------
#This is file config.ini
[Test Segment]
name1=test1
name2=test2

name3=test3

	[New Segment]
name1=new1
#new2, this line won't be read
name2=new2
--------------------------------
任何以#开头的行都被作为注解不计，空白行不计，前导空白不计。
使用完需要调用DeleteConfigSegmentList(..)对返回的链表释放 */
LIST *ReadConfigSegment(const char *pcFileName, const char *pcSegName);
BOOL WriteConfigSegment(const char *pcFileName, const char *pcSegName, LIST *pListConfigItem);
void DeleteConfigSegmentList(LIST *pList);

BOOL ReadConfigValue(const char *pcFileName, const char *pcSegName, const char *pcVarName, char *pcVarValue, int iMaxReadSize);
BOOL WriteConfigValue(const char *pcFileName, const char *pcSegName, const char *pcVarName, const char *pcVarValue);

/* 将整个文件读到ppFileBuf中，用完要free */
BOOL ReadWholeFile(const char *pcFileName, char **ppcFileBuf, int *piFileLength);

/* 从文件buffer(pFileBuf)中的pcThisLinePos处得到一行，并指定下一行的行首指针*ppcNextLinePos
** 如果没有下一行了，*ppcNextLinePos将指向文件结尾字符+1处
** 如果从pcThisLinePos处以后没有行了，返回FALSE; 反之返回TRUE */
BOOL ReadLineFromFileBuf(char *pcFileBuf, int iFileLength, char *pcThisLinePos, char **ppcNextLinePos);
#endif
