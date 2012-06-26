#include <stdarg.h>
#include <stdio.h>

#include "HttpServer.h"
#include "C_SendFile.h"

/* 寻找<!--$B%s%d-->...<!--$E-->定义的可替代块 */
BOOL SearchReplaceBlock(char *pcFileBuf,
						int iFileLength,
						char *pcBeginSearchPos,
						char **ppcBlockHeader,
						char **ppcBlockTail,
						char **ppcNextBeginSearchPos);

BOOL SearchReplaceBlock(char *pcFileBuf,
						int iFileLength,
						char *pcBeginSearchPos,
						char **ppcBlockHeader,
						char **ppcBlockTail,
						char **ppcNextBeginSearchPos)
{
	char *pcB;
	char *pcE;
	char *pcN;
	char *pcFileBufEnd;

	pcFileBufEnd = pcFileBuf + iFileLength;

	for (pcB=pcBeginSearchPos+5; pcB<pcFileBufEnd; pcB++)
	{
		if(*(pcB-5)=='<' && *(pcB-4)=='!' && *(pcB-3)=='-' && *(pcB-2)=='-' && *(pcB-1)=='$' && (*pcB=='B' || *pcB=='b'))
		{
			for (pcE=pcB+3; pcE<pcFileBufEnd; pcE++)
			{
				if(*(pcE-2)=='-' && *(pcE-1)=='-' && *(pcE)=='>')
				{
					*ppcBlockHeader = pcB + 1;
					*ppcBlockTail = pcE - 2;
					break;
				}
			}

			if (pcE == pcFileBufEnd) return FALSE;
			else
			{
				for (pcN=*ppcBlockTail+8; pcN<pcFileBufEnd; pcN++)
				{
					if (*(pcN-8)=='<' && *(pcN-7)=='!' && *(pcN-6)=='-' && *(pcN-5)=='-' && *(pcN-4)=='$' && (*(pcN-3)=='B' || *(pcN-3)=='b'))
					{
						*ppcNextBeginSearchPos = pcE + 1;
						return TRUE;
					}
					if (*(pcN-8)=='<' && *(pcN-7)=='!' && *(pcN-6)=='-' && *(pcN-5)=='-' && *(pcN-4)=='$' && (*(pcN-3)=='E' || *(pcN-3)=='e') && *(pcN-2)=='-' && *(pcN-1)=='-' && *(pcN)=='>')
					{
						*ppcNextBeginSearchPos = pcN + 1;
						break;
					}
				}
				if (pcN >= pcFileBufEnd) *ppcNextBeginSearchPos = pcE + 1;
				return TRUE;
			}
		}
	}
	return FALSE;
}

typedef int (*SendFunctionType)(HTTPCONNECTION , char *);

void __SendBufferWithParam( HTTPCONNECTION hc, char *pcBuffer, int iBufLen, va_list arg )
{
	char *pcFileBuf;
	int iFileLen;
	char *pcBeginSearchPos;
	char *pcBlockBegin;
	char *pcBlockEnd;
	char *pcNextBeginSearchPos;
	char *pcFmt;
	int i, j;
	char *pcBuf;
	char *pc;

	char *pcArg;
	int iArg;
	unsigned int uiArg;
	int (*pfArg)(HTTPCONNECTION, char *);

	if ((pcBuf = (char *)malloc(1024 * 10)) == NULL)
	{
		//(CXH_MODIFY) 这儿可能会超出pcBuf，从而产生严重的问题。???How to
		PRINT_MEM_OUT;
		return;
	}

	pcFileBuf = pcBuffer;
	iFileLen = iBufLen;

	pcBeginSearchPos = pcFileBuf;
	while(SearchReplaceBlock(pcFileBuf, iFileLen,
						pcBeginSearchPos,
						&pcBlockBegin, &pcBlockEnd, &pcNextBeginSearchPos))
	{
		AddHttpBody(hc, pcBeginSearchPos, pcBlockBegin - 6 - pcBeginSearchPos);

		for (i=0,pcFmt=pcBlockBegin; pcFmt<pcBlockEnd;)
		{
			if(*pcFmt != '%')
			{
				pcBuf[i++] = *pcFmt++;
			}
			else
			{
				if (++pcFmt >= pcBlockEnd) break;
				if (*pcFmt == 's')
				{
					pcArg = va_arg(arg, char *);
					for (j=0; pcArg[j]!='\0'; j++) pcBuf[i++] = pcArg[j];
					pcFmt++;
				}
				else if (*pcFmt == 'j')	//放在javascript中的字符串
				{
					pcArg = va_arg(arg, char *);
					pc = GetWebInCString(pcArg);
					if (pc != NULL)
					{
						for (j=0; pc[j]!='\0'; j++) pcBuf[i++] = pc[j];
						free(pc);
					}
					pcFmt++;
				}
				else if (*pcFmt == 'd')
				{
					iArg = va_arg(arg, int);
					i += sprintf(&pcBuf[i], "%d", iArg);
					pcFmt++;
				}
				else if (*pcFmt == 'u')
				{
					uiArg = va_arg(arg, unsigned int);
					i += sprintf(&pcBuf[i], "%u", uiArg);
					pcFmt++;
				}
				else if (*pcFmt == 'p')
				{
					uiArg = va_arg(arg, unsigned int);
					i += IP2String(uiArg, &pcBuf[i]);
					pcFmt++;
				}
				else if (*pcFmt == 'C')
				{
					pfArg = va_arg(arg, SendFunctionType);
					i += pfArg(hc, &pcBuf[i]);
					pcFmt++;
				}
				else
				{
					pcBuf[i++] = *(pcFmt-1);
					pcBuf[i++] = *pcFmt++;
				}
			}
		}
		pcBuf[i] = '\0';
		AddHttpBody(hc, pcBuf, i);

		pcBeginSearchPos = pcNextBeginSearchPos;
	}

	AddHttpBody(hc, pcBeginSearchPos, pcFileBuf + iFileLen - pcBeginSearchPos);

	free(pcBuf);
	SetHttpHeader(hc, 200, "OK", "", "Pragma: no-cache\r\nCache-Control: no-cache\r\nExpires: 01 Jan 1970 00:00:00 GMT\r\n", "text/html", TRUE);
}

void SendBufferWithParam( HTTPCONNECTION hc, char *pcBuffer, int iBufLen, ...)
{
	va_list arg;
	va_start(arg, iBufLen);
	__SendBufferWithParam(hc, pcBuffer, iBufLen, arg);
	va_end(arg);
}

void SendHtmlWithParam(HTTPCONNECTION hc, const char *pcFileName, ...)
{
	char *pcFileBuf;
	int iFileLen;

	va_list arg;

	if(!ReadWholeFile(pcFileName, &pcFileBuf, &iFileLen))
	{
		fprintf(stderr, "Cannot read file! FILE: %s LINE: %d.\n", __FILE__, __LINE__);
		send_r_not_found((void *)hc);
		return;
	}

	va_start(arg, pcFileName);
	__SendBufferWithParam(hc, pcFileBuf, iFileLen, arg);
	va_end(arg);

	free(pcFileBuf);
	return;
}
