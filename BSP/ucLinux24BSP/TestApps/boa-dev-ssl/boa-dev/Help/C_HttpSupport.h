#ifndef C_HTTPSUPPORT_H
#define C_HTTPSUPPORT_H

/* 分解一个请求子串，存放在一个LIST结构的双向链表中
** eg: pcString = "name1=&name2=value2&name3=val%20ue3",
** 分解后得到的链表为: "name1"、NULL, "name2"、 "value2", "name3"、 "val u3"
** 链表中每个节点包含一NAMEDSTRING_T类型(CommonDef.h)的数据 */
LIST *ParseString(const char *pcString);

/* 释放ParseString函数生成的链表及其中的字符串资源 */
void DeleteParseString(LIST *pList);


/* 读取用Get方法发回的字符，并将该字符分解成Name,Value对的形式，
** 是GetQueryString和ParseString的综合
** 返回的链表中每个节点包含一NAMEDSTRING_T类型的数据 */
LIST *ReadQueryList(HTTPCONNECTION hc);
/* 释放ReadQueryList生成的链表及其使用的资源 */
void DeleteQueryList(LIST *pList);

/* http方式上传文件
** 上传成功返回0,失败返回一个负的错误号.
** 注意调用后用同样的参数调一遍HttpUploadClear
		- 如果ppcSavePath不为NULL, 则将ppcSavePath置为保存的目录
		- 如果pFileList不为NULL, 则纪录保存的文件列表，需要调用
** return 0		成功
** return -1	解析错误
** return -2	文件超过大小
** return -3	文件保存错误
*/
int HttpUpload(HTTPCONNECTION hConnection, char *pcReceiveBuf, int iReceiveLen, char *pcDefaultSavePath, char **ppcSavePath, LIST **ppFileList);
/* HttpUpload善后 */
void HttpUploadClear(HTTPCONNECTION hConnection, char **ppcSavePath, LIST **ppFileList);

#endif
