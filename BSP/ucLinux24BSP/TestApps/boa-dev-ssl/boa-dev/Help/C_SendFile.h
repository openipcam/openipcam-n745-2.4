#ifndef C_SENDFILE_H
#define C_SENDFILE_H

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

/* 用可变参数...替换字符串中的<!--Bfmt-->[text]<!--E-->部分，
** fmt为一字符串(不加引号),写法类似printf中的fmt,目前仅支持:
**	%s	字符串
**	%d	符号int
**	%u	无符号int
**	%p	ip地址unsigned long
**	%C	字符串生成函数	int Fun(HTTPCONNECTION hc, char *pcFillBegin), 该函数向pcFillBegin中填如干子串，返回填写的字符数
*/
void SendBufferWithParam( HTTPCONNECTION hc, char *pcBuffer, int iBufLen, ...);

/* 用可变参数...替换文件中的<!--Bfmt-->[text]<!--E-->部分，
** fmt为一字符串(不加引号),写法类似printf中的fmt,目前仅支持:
**	%s	字符串
**	%d	符号int
**	%u	无符号int
**	%p	ip地址unsigned long
**	%C	字符串生成函数	int Fun(HTTPCONNECTION hc, char *pcFillBegin), 该函数向pcFillBegin中填如干子串，返回填写的字符数
*/
void SendHtmlWithParam( HTTPCONNECTION hc, const char *pcFileName, ... );

#endif
