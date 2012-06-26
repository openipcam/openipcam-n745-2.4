#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "Help/HttpServer.h"

#ifdef TEST_WITH_THREAD
#include <pthread.h>
#include "thcfg.h"
struct thread_cfg thcfg={
	each_thread_pages: 8
};
#endif

int t1(HTTPCONNECTION hConnection,
								char **ppcPostBuf,
								int *piPostBufLen,
								int *piPostDataLen,
								char *pcFillData,
								int iFillDataLen,
								int iIsMoreData/*bool*/,
								void *pParam/*other parameter for extend use*/)
{
	if (*ppcPostBuf == NULL)
	{
		*piPostBufLen = 3*1024*1024;
		*ppcPostBuf = (char *)malloc(*piPostBufLen);
		*piPostDataLen = 0;
	}

	memcpy(*ppcPostBuf + *piPostDataLen, pcFillData, iFillDataLen);
	*piPostDataLen += iFillDataLen;

	AddHttpBodyString(hConnection, "OK");
	SetHttpHeader(hConnection, 200, "OK", NULL, NULL, "text/plain", TRUE);
	return 0;

	if (!iIsMoreData)
	{
		AddHttpBody(hConnection, *ppcPostBuf, *piPostDataLen);
		SetHttpHeader(hConnection, 200, "OK", NULL, NULL, "text/plain", TRUE);
		return 1;
	}
	return 1;
}

#include "boa.h"
int tt(HTTPCONNECTION hc, void *p)
{
	static i=0;
	request *req = (request *)hc;
	printf("--->[%s]\n[%s]\n", req->pathname, req->query_string);

	//printf("-------------------->oks\n");
	if (GetHttpMethod(hc) == M_GET)
	{
		FILE *fp;
		struct stat st;
		int success = 0;
		char *pcF[] = {"/CamImg1542.jpg", "/CamImg2328.jpg"} ;
		char *pcFile;

		i++;
		i %= 2;

		pcFile = GetDocumentBasedPath(pcF[i]);
		if (pcFile)
		{
			if (stat(pcFile, &st) == 0)
			{
				fp = fopen(pcFile, "rb");
				if (fp)
				{
					char *pc;
					pc = malloc(st.st_size);
					fread(pc, st.st_size, 1, fp);
					AddHttpBody(hc, pc, st.st_size);
					SetHttpHeader(hc, 200, "OK", NULL, "Expires: 01 Jan 1970 00:00:00 GMT\r\nCache-Control: no-cache\r\n", "image/jpeg", TRUE);
					free(pc);
					fclose(fp);
					success = 1;
				}
			}
			free(pcFile);
		}
		if (!success)
			send_r_not_found(hc);

		return 0;
	}
	else
//		SetPostDataFun(hc, t1, p);
	return -1;
}

static int adddata(HTTPCONNECTION hConnection, time_t *ptLastFill, void *p)
{
	FILE *fp;
	time_t tCur;
	static ss=0;

	fp = (FILE *)p;

	if (!feof(fp))
	{
		char ac[1024 * 6];
		int rt;
		rt = fread(ac, 1, sizeof(ac), fp);
		AddHttpBody(hConnection, ac, rt);
printf("%d %d\n", rt, ss++);
		SetSendDataOverFun(hConnection, adddata, p);
		usleep(100000);
	}
	else
	{
PTE;
		fclose(fp);
		SetSendDataOverFun(hConnection, NULL, p);
	}
	return 1;
}

int t(HTTPCONNECTION hConnection, void *p)
{
	FILE *fp;
	fp = fopen("big.avi", "rb");

	SetSendDataOverFun(hConnection, adddata, fp);

	SetKeepAliveMode(hConnection, FALSE);
	SetHttpHeader(hConnection, 200, "OK", "",
				"Pragma: no-cache\r\nCache-Control: no-cache\r\nExpires: 01 Jan 1970 00:00:00 GMT\r\n",
				"video/mjpeg", FALSE);

	return 0;
}


static int addtextdata(HTTPCONNECTION hConnection, time_t *ptLastFill, void *p)
{
	time_t tCur;
	char ac[32];

	int i;
	i = (int)p;
	sprintf(ac, "&#9632;", i++);

	AddHttpBodyString(hConnection, ac);
	SetSendDataOverFun(hConnection, addtextdata, (void *)i);
	usleep(1000*500);
	PTE;
	return 1;
}

int text(HTTPCONNECTION hConnection, void *p)
{
	//SetSendDataOverFun(hConnection, addtextdata, 0);

	SetKeepAliveMode(hConnection, FALSE);
	AddHttpBodyString(hConnection, "<html><body><a href=/mf.cgi>Show directories</a></body></html>\n");

	SetHttpHeader(hConnection, 200, "OK", "",
				"Pragma: no-cache\r\nCache-Control: no-cache\r\nExpires: 01 Jan 1970 00:00:00 GMT\r\n",
				"text/html", FALSE);
	return 0;
}

int main(int argc, char **argv)
{
/*
	int i;
	char *pcDocRoot = NULL;
	int iPort = 0;

	for (i = 1; i < argc; i++)
	{
		if (i + 1 < argc && strcmp(argv[i], "-d") == 0)
			pcDocRoot = argv[++i];
		if (i + 1 < argc && strcmp(argv[i], "-p") == 0)
			iPort = String2Long(argv[++i]);
	}
	if (pcDocRoot == NULL || iPort == 0)
	{
		fprintf(stderr, "Usage:\n	-d HtmlDir -p port\n");
		return 0;
	}

	//RegisterEmbedFun("/Jpeg/*.jpg", tt, NULL);
	//RegisterEmbedFun("*", tt, NULL);
	RegisterEmbedFun("test", t, NULL);
*/
	char *pcDocRoot = "./";
	int aiPort[] = {80, 0};
	int aiSSLPort[] = {443, 0};
	RegisterEmbedFun("/", text, NULL);
	StartThttpdEx3(pcDocRoot, aiPort, aiSSLPort,
					10, 100, 3,
					NULL, NULL, NULL);

#if 0
	XML *pXML = CreateXML("eee");
	XML *pSub;
	XML *pSub1;
	XML *pSub2;
	SetString(pXML->plAttrib, "type", "text");
	SetString(pXML->plAttrib, "value", "mytest");
	SetString(pXML->plAttrib, "name", "test");

	pSub = AppendXML(pXML, NULL);

printf("%d %d %d\n", pXML, pSub,
GetParentXML(pSub)
);
	return 0;

	pSub = CreateXML("sub");
	pSub1 = AppendXML(pXML, pSub);
	SetString(pSub1->plAttrib, "sss", "eeeee");

	pSub = AppendXML(pXML, NULL);
	AppendXML(pSub, NULL);
	AppendXML(pSub, NULL);
	AppendXML(pSub, NULL);
	DumpXML(pXML, NULL, NULL);
	PTI;

	pSub2 = CreateXMLText("how ass s ss s s ");
	pSub2 = AppendXML(pSub1, pSub2);
	DumpXML(pSub1, NULL, NULL);

	PTI;
	pSub1 = DetachXML(pXML, pSub1);
	DumpXML(pXML, NULL, NULL);
	PTI;
	DumpXML(pSub1, NULL, NULL);

	DeleteXML(pXML);
	DeleteXML(pSub1);
	pt_mem_result();
#endif
}
