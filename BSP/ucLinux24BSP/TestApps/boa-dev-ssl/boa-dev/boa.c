/* vi:set tabstop=2 cindent shiftwidth=2: */
/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996 Charles F. Randall <crandall@goldsys.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1996,97,98 Jon Nelson <nels0988@tc.umn.edu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* boa: boa.c */

#include "boa.h"
#include <grp.h>
#include <syslog.h>
#include <sys/param.h>
#ifdef SERVER_SSL
#include <openssl/ssl.h>
#endif /*SERVER_SSL*/

#ifdef SERVER_SSL
#ifdef EMBED
#define SSL_KEYF "/etc/ssl_key.pem"
#define SSL_CERTF "/etc/ssl_cert.pem"
#else
#define SSL_KEYF "./ssl_key.pem"
#define SSL_CERTF "./ssl_cert.pem"
#endif
//	int server_ssl;				/*ssl socket */
	SSL_CTX *ctx;				/*SSL context information*/
	SSL_METHOD *meth;			/*SSL method information*/
#endif /*SERVER_SSL*/

int backlog = SO_MAXCONN;

struct timeval req_timeout;		/* timeval for select */

fd_set block_read_fdset;
fd_set block_write_fdset;

int lame_duck_mode = 0;

int sock_opt = 1;

static int max_fd = 0;

#ifdef DEBUG
int Http_DebugGetFile(HTTPCONNECTION hConnection, void *pParam)
{
	char ac[128];
	char *pc;
	char *pcBuf;
	int iBufLen;
	char *pcFilePath = GetQueryString(hConnection);
	request *req = (request *)hConnection;

	if (pcFilePath == NULL)
	{
		send_r_not_found(req);
		return 0;
	}

	if (req->pathname != NULL) free(req->pathname);
	req->pathname = strdup(pcFilePath);
	unescape_uri(req->pathname);
	strcpy(req->request_uri, req->pathname);

	for (pc = req->pathname + strlen(req->pathname); pc >= req->pathname; pc--)
		if (*pc == '/' || *pc == '\\') break;
	sprintf(ac, "Content-Disposition: attachment; filename=\"%s\"\r\n", pc + 1);

	if (ReadWholeFile(req->pathname, &pcBuf, &iBufLen))
	{
		printf("%d\n", iBufLen);
		AddHttpBody(hConnection, pcBuf, iBufLen);
		SetHttpHeader(hConnection, 200, "OK", "",
				ac,
				"Unknown", TRUE);
		return 0;
	}
	else
	{
		send_r_forbidden(req);
		return 0;
	}
}

int Http_ManageFile(HTTPCONNECTION hConnection, void *pParam)
{
	LIST *pList;
	int iAction;
	const char *pcFile;
	const char *pcParam;
	FILE *fp;
	char *pcBuf = NULL;
	int iBuf;
	mode_t mode;
	int i;
	LIST *plParam;
	char *apcParam[32];
	volatile BOOL bSuc = FALSE;

	pList = ReadQueryList(hConnection);
	if (pList == NULL)
	{
		send_r_forbidden(hConnection);
		return 0;
	}
	iAction = GetLong(pList, "sl");
	pcFile = GetString(pList, "file");
	pcParam = GetString(pList, "param");

	bSuc = FALSE;
	switch (iAction)
	{
	case 0:	//Copy To:
		if (ReadWholeFile(pcFile, &pcBuf, &iBuf))
		{
			fp = fopen(pcParam, "wb");
			if (fp != NULL)
			{
				fwrite(pcBuf, iBuf, 1, fp);
				bSuc = TRUE;
				fclose(fp);
			}
			free(pcBuf);
		}
		break;
	case 1:	//Delete
		if (unlink(pcFile) == 0) bSuc = TRUE;
		break;
	case 2:	//Chmod
		if (pcParam[0] != '\0')
			if (sscanf(pcParam, "%o", &mode) == 1)
				if (chmod(pcFile, mode) == 0) bSuc = TRUE;
		break;
	case 3:	//Execute
		plParam = SplitString(pcParam, ' ');
		if (plParam != NULL)
		{
			LISTNODE *pNode;
			apcParam[0] = (char *)pcFile;
			for (i = 1, pNode = plParam->pFirstNode;
				i < (sizeof(apcParam) / sizeof(char *) - 1)
				&& pNode != plParam->pLastNode;
				pNode = pNode->pNextNode)
			{
				char *pc = GetTrimString(pNode->pValue);
				if (pc != NULL)
				{
					apcParam[i++] = pc;
				}
			}
			apcParam[i] = NULL;
			DeleteSplitString(plParam);

			if (vfork() == 0)
			{
				bSuc = TRUE;
				execv(apcParam[0], apcParam);
				bSuc = FALSE;
				_exit(0);
			}

			for (i = 1; apcParam[i] != NULL; i++)
				free(apcParam[i]);
		}
		break;
	}
	AddHttpBodyString(hConnection, (bSuc?"Success":"Failed"));
	SetHttpHeader(hConnection, 200, "OK", NULL, NULL, "text/html", TRUE);

	DeleteQueryList(pList);
	return 0;
}

int Http_DebugGetFileInit(HTTPCONNECTION hConnection, void *pParam)
{
	char *pcFileDoc = "<HTML>\n<HEAD>\n<STYLE>\n.body	{font-size: 10pt;}\n</STYLE>\n</HEAD>\n<BODY>\nFile: <a href='/wymDownFile.cgi?<!--$B%s-->'><!--$B%s-->&nbsp;(<script>document.write(new Date(1000 * <!--$B%d-->));</script>)</a><p>\nAction:<br>\n<form name=cf method=get action='/wymManageFile.cgi'>\n<select name=sl>\n	<option value=0>Copy To</option>\n	<option value=1>Delete</option>\n	<option value=2>Chmod</option>\n	<option value=3>Execute</option>\n</select>\n<input name=param type=text>\n<input name=file type=hidden value='<!--$B%s-->'>\n<input type=submit value=OK>\n<form>\n</BODY>\n</HTML>\n";
	char *pcDirDoc1 = "<HTML>\n<HEAD>\n<STYLE>\n.body	{font-size: 10pt;}\n</STYLE>\n</HEAD>\n<BODY>\nDir: ";
	char *pcDirDoc2 = "';if (d.substr(d.length - 1) == '/') d = d.substr(0, d.length - 1);\ns = (d + '&nbsp;(' + new Date(1000 * tm) + ')<p><table align=center width=90%>');\nfor (i = 0; i < a.length - 1; i++)\n{\nif (a[i][0] == '.') continue;\nelse\n{\nif (a[i][0] == '..')\n{\nf = d.substr(0, d.lastIndexOf('/') + 1);\nif (f.length == 0) f = '/';\n}\nelse f = d + '/' + a[i][0];\ns += '<tr><td>(' + a[i][4] + ')</td><td>' + a[i][1] + '</td><td><a href=\"/mf.cgi?' + f + '\"><font color=' + (a[i][2] ? 'blue' : 'green') + '>' + a[i][0] + (a[i][2] ? '/' : '') + '</font></a></td></tr>';\n}\n}\ns += '</table>';\ndocument.write(s);\n</SCRIPT></BODY>\n</HTML>\n";
	char *pcFilePath = GetQueryString(hConnection);
	struct stat st;
	char ac[24];
	char acRootPath[4];

	if (pcFilePath == NULL || pcFilePath[0] == '\0')
	{
		pcFilePath = acRootPath;
		pcFilePath[0] = '/';
		pcFilePath[1] = '\0';
	}

	unescape_uri(pcFilePath);
	if (stat(pcFilePath, &st) == 0)
	{
		if (S_ISDIR(st.st_mode))
		{
			DIR *dir;
			struct dirent *next;
			char *pc;

			AddHttpBodyString(hConnection, pcDirDoc1);
			AddHttpBodyString(hConnection, "<script>tm=");
			Long2String(st.st_mtime, ac);
			AddHttpBodyString(hConnection, ac);
			AddHttpBodyString(hConnection, ";\na=[\n");

			if (pcFilePath[strlen(pcFilePath) - 1] == '/')
				pcFilePath[strlen(pcFilePath) - 1] == '\0';
			dir = opendir(pcFilePath);
			if (dir)
			{
				while ((next = readdir(dir)) != NULL)
				{
					char *pcPath = malloc(strlen(pcFilePath) + 3 + strlen(next->d_name));
					if (pcPath == NULL) continue;
					strcpy(pcPath, pcFilePath);
					strcat(pcPath, "/");
					strcat(pcPath, next->d_name);
					if (stat(pcPath, &st) != 0)
					{
						free(pcPath);
						continue;
					}
					free(pcPath);

					pc = GetCStyleString(next->d_name);
					if (pc == NULL) continue;
					AddHttpBodyString(hConnection, "['");
					AddHttpBodyString(hConnection, pc);
					AddHttpBodyString(hConnection, "',");
					Long2String(st.st_size, ac);
					AddHttpBodyString(hConnection, ac);
					AddHttpBodyString(hConnection, ",");
					Bool2String(S_ISDIR(st.st_mode), ac);
					AddHttpBodyString(hConnection, ac);
					AddHttpBodyString(hConnection, ",");
					Long2String(st.st_ctime, ac);
					AddHttpBodyString(hConnection, ac);
					AddHttpBodyString(hConnection, ",'");
					sprintf(ac, "%03o", st.st_mode);
					AddHttpBodyString(hConnection, ac);
					AddHttpBodyString(hConnection, "'],\n");
					free(pc);
				}
				closedir(dir);
			}
			AddHttpBodyString(hConnection, "];\nd='");
			if ((pc = GetCStyleString(pcFilePath)) != NULL)
			{
				AddHttpBodyString(hConnection, pc);
				free(pc);
			}
			AddHttpBodyString(hConnection, pcDirDoc2);
			SetHttpHeader(hConnection, 200, "OK", NULL, NULL, "text/html", TRUE);
			return 0;
		}
		else
		{
			SendBufferWithParam(hConnection, pcFileDoc, strlen(pcFileDoc),
				pcFilePath,
				pcFilePath,
				st.st_mtime,
				pcFilePath);
			return 0;
		}
	}

	AddHttpBodyString(hConnection, "You don't specify a valid file.");
	SetHttpHeader(hConnection, 200, "OK", NULL, NULL, "text/html", TRUE);

	return 0;
}
#endif

/*
 * Name: fdset_update
 *
 * Description: iterate through the blocked requests, checking whether
 * that file descriptor has been set by select.  Update the fd_set to
 * reflect current status.
 */

void fdset_update(int *pifd_Server, int iPortNum)
{
	request *current, *next;
	time_t current_time;
	int i;

	current = request_block;

	current_time = time(NULL);

	while (current) {
		time_t time_since;
		next = current->next;

		time_since = current_time - current->time_last;

		/* hmm, what if we are in "the middle" of a request and not
		 * just waiting for a new one... perhaps check to see if anything
		 * has been read via header position, etc... */

		if (current->kacount && (time_since >= ka_timeout) && !current->logline) {
			SQUASH_KA(current);
			free_request(&request_block, current);
		} else if (time_since > REQUEST_TIMEOUT) {
			SQUASH_KA(current);
			free_request(&request_block, current);
		} else if (current->buffer_end) {
			if (FD_ISSET(current->fd, &block_write_fdset))
				ready_request(current);
		} else {
			switch (current->status) {
			case WRITE:
				if (FD_ISSET(current->fd, &block_write_fdset))
					ready_request(current);
				else
					FD_SET(current->fd, &block_write_fdset);
				break;
			default:
				if (FD_ISSET(current->fd, &block_read_fdset))
					ready_request(current);
				else
					FD_SET(current->fd, &block_read_fdset);
				break;
			}
		}
		current = next;
	}

	if (!lame_duck_mode &&
      (max_connections == -1 || status.connections < max_connections)) {
      	for (i = 0; i < iPortNum; i++)
			FD_SET(pifd_Server[i], &block_read_fdset);	/* server always set */
	} else {
		for (i = 0; i < iPortNum; i++)
		{
			if (pifd_Server[i] != -1)
			FD_CLR(pifd_Server[i], &block_read_fdset);
		}
	}

	req_timeout.tv_sec = (ka_timeout ? ka_timeout : REQUEST_TIMEOUT);
	req_timeout.tv_usec = 0l;	/* reset timeout */
}


int create_listen_socket(int port,
							int (*pOnListenSocketCreate)(int fd, int iPort)
							)
{
	struct sockaddr_in server_sockaddr;		/* boa socket address */
	int fd;

	if (port <= 0) return -1;

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		PTE;
		return -1;
	}

	/* server socket is nonblocking */
	if (fcntl(fd, F_SETFL, NOBLOCK) == -1)
	{
		PTE;
		close(fd);
		return -1;
	}

	if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
					sizeof(sock_opt))) == -1)
	{
		PTE;
		close(fd);
		return -1;
	}

	if (pOnListenSocketCreate != NULL)
	{
		(*pOnListenSocketCreate)(fd, port);
	}

	/* internet socket */
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sockaddr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr *) &server_sockaddr,
			 sizeof(server_sockaddr)) == -1)
	{
		PTE;
		close(fd);
		return -1;
	}

	if (listen(fd, backlog) == -1)
	{
		PTE;
		close(fd);
		return -1;
	}

	return fd;
}


HTTPSERVER StartThttpdEx3(char *pcServerRoot,
				int *piPort,
				int *piSSLPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnListenSocketCreate)(int fd, int iPort),
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin)
{
	int aiPort[2] = {80, 0};
	int aifd_Server[32];					/* boa socket */
#ifdef SERVER_SSL
	int aiServerSSL[32];					/* 0 - common, 1 - ssl */
	int use_ssl = 0;
#endif
	struct sockaddr_in server_sockaddr;		/* boa socket address */
	int iPortNum;
	int i;

#ifdef DEBUG
	RegisterEmbedFunEx("/mf.cgi", Http_DebugGetFileInit, AUTH_ANY, NULL);
	RegisterEmbedFunEx("/wymDownFile.cgi", Http_DebugGetFile, AUTH_ANY, NULL);
	RegisterEmbedFunEx("/wymManageFile.cgi", Http_ManageFile, AUTH_ANY, NULL);
#endif

	if ((document_root = FixupDocumentRoot(pcServerRoot)) == NULL)
		exit(0);
	read_config_files();

#ifdef SERVER_SSL
	if (piSSLPort != NULL && piSSLPort[0] != 0)
	{
		use_ssl = InitSSLStuff();
		if (use_ssl != 1)
		{
			/*TO DO - emit warning the SSL stuff will not work*/
			fprintf(stderr, "Failure initialising SSL support");
		}
	}
#endif /*SERVER_SSL*/


	ka_timeout = iKeepAliveTimeout;
	ka_max = iKeepAliveMax;
	max_connections = iMaxConnections;
	g_pfunOnRequestBegin = pOnRequestBegin;

	/* listen: large number just in case your kernel is nicely tweaked */
	if (max_connections != -1)
		backlog = MIN(backlog, max_connections);


	if (piPort == NULL || piPort[0] == 0) piPort = aiPort;
	for (iPortNum = 0, i = 0;
			i < sizeof(aifd_Server) / sizeof(int) && piPort[iPortNum] != 0;
			iPortNum++)
	{
		aifd_Server[i] = create_listen_socket(piPort[iPortNum], pOnListenSocketCreate);
		if (aifd_Server[i] < 0)
		{
			fprintf(stderr, "Create listening socket on %d - failed.\n", piPort[iPortNum]);
			continue;
		}
#ifdef SERVER_SSL
		aiServerSSL[i] = 0;
#endif
		if (aifd_Server[i] > max_fd)
			max_fd = aifd_Server[i];
		i++;
	}
#ifdef SERVER_SSL
	if (use_ssl)
	{
		for (iPortNum = 0;
			i < sizeof(aifd_Server) / sizeof(int) && piSSLPort[iPortNum] != 0;
			iPortNum++)
		{
			aifd_Server[i] = create_listen_socket(piSSLPort[iPortNum], pOnListenSocketCreate);
			if (aifd_Server[i] < 0)
			{
				fprintf(stderr, "Create listening socket on %d - failed.\n", piSSLPort[iPortNum]);
				continue;
			}
			aiServerSSL[i] = 1;	//indicate that the socket is for ssl.
			if (aifd_Server[i] > max_fd)
				max_fd = aifd_Server[i];
			i++;
		}
	}
#endif
	iPortNum = i;	//valid port number.
	if (iPortNum <= 0)
	{
		die(NO_CREATE_SOCKET);
		return NULL;
	}

	init_signals();

	/* callback for init */
	if (pOnHttpInit != NULL)
		(*pOnHttpInit)(NULL);

	/* main loop */

	FD_ZERO(&block_read_fdset);
	FD_ZERO(&block_write_fdset);

	status.connections = 0;
	status.requests = 0;
	status.errors = 0;

	while (1) {

		switch(lame_duck_mode) {
			case 1:
				lame_duck_mode_run(aifd_Server, iPortNum);
			case 2:
				if (!request_ready && !request_block)
					die(SHUTDOWN);
				break;
			default:
				break;
		}

		/* move selected req's from request_block to request_ready */
		fdset_update(aifd_Server, iPortNum);

		if (!request_ready) {
			request *current;

			max_fd = 0;
			for (i = 0; i < iPortNum; i++)
				max_fd = MAX(aifd_Server[i], max_fd);
			for (current = request_block; current; current = current->next) {
				max_fd = MAX(current->fd, max_fd);
				//max_fd = MAX(current->data_fd, max_fd);
			}

			if (select(max_fd + 1, &block_read_fdset, &block_write_fdset, NULL,
					   (request_block ? &req_timeout : NULL)) == -1)
				if (errno == EINTR || errno == EBADF)
					continue;	/* while(1) */
				else
					die(SELECT);

			for (i = 0; i < iPortNum; i++)
				if (FD_ISSET(aifd_Server[i], &block_read_fdset))
				{
#ifdef SERVER_SSL
					if (aiServerSSL[i])
						get_ssl_request(aifd_Server[i]);
					else
#endif
						get_request(aifd_Server[i]);
				}
		}
		else
		{//cxh_add, for connect immediately.
#if 1
			fd_set listen_set;
			struct timeval to;
			FD_ZERO(&listen_set);
			if (!lame_duck_mode &&
    			(max_connections == -1 || status.connections < max_connections))
    		{
				max_fd = 0;
				for (i = 0; i < iPortNum; i++)
				{
					FD_SET(aifd_Server[i], &listen_set);	/* server always set */
					max_fd = MAX(aifd_Server[i], max_fd);
				}

				to.tv_sec = 0;
				to.tv_usec = 0;
				if (select(max_fd + 1, &listen_set, NULL, NULL, &to) != -1)
				{
					for (i = 0; i < iPortNum; i++)
					{
						if (FD_ISSET(aifd_Server[i], &listen_set))
						{
#ifdef SERVER_SSL
							if (aiServerSSL[i])
								get_ssl_request(aifd_Server[i]);
							else
#endif
								get_request(aifd_Server[i]);
						}
					}
				}
			}
#endif
		}


		process_requests();		/* any blocked req's move from request_ready to request_block */
	}
}


HTTPSERVER StartThttpdEx2(char *pcServerRoot,
				int *piPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnListenSocketCreate)(int fd, int iPort),
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin)
{
	return StartThttpdEx3(pcServerRoot,
				piPort,
				NULL,
				iKeepAliveTimeout,
				iKeepAliveMax,
				iMaxConnections,
				pOnListenSocketCreate,
				pOnHttpInit,
				pOnRequestBegin);
}


HTTPSERVER StartThttpdEx(char *pcServerRoot,
				int *piPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin)
{
	return StartThttpdEx2(pcServerRoot,
				piPort,
				iKeepAliveTimeout,
				iKeepAliveMax,
				iMaxConnections,
				NULL,
				pOnHttpInit,
				pOnRequestBegin);
}

HTTPSERVER StartThttpd(char *pcServerRoot,
				int iPort,
				int iKeepAliveTimeout,
				int iKeepAliveMax,
				int iMaxConnections,
				int (*pOnHttpInit)(HTTPSERVER hServer),
				REQUEST_CALLBACK_PFUN pOnRequestBegin)
{
	int aiPort[2];
	aiPort[0] = iPort;
	aiPort[1] = 0;
	StartThttpdEx(pcServerRoot, aiPort,
		iKeepAliveTimeout, iKeepAliveMax,
		iMaxConnections,
		pOnHttpInit,
		pOnRequestBegin);
}


/*
 * Name: die
 * Description: die with fatal error
 */

void die(int exit_code)
{
	fprintf(stderr, "Die! code: %d\n", exit_code);
	ClearEmbedFun();
	dump_mime();
#ifdef MEMDEBUG_H
	pt_mem_result();
	pt_fd_result();
	pt_fp_result();
#endif
	exit(exit_code);
}

#ifdef SERVER_SSL

int
InitSSLStuff()
{
	/*Init all of the ssl stuff*/
//	i don't know why this line is commented out... i found it like that - damion may-02
/*	SSL_load_error_strings();	*/
	SSLeay_add_ssl_algorithms();
	meth = SSLv23_server_method();
	if(meth == NULL){
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "Couldn't create the SSL method\n");
		die(NO_SSL);
		return 0;
	}
	ctx = SSL_CTX_new(meth);
	if(!ctx){
		fprintf(stderr, "Couldn't create a connection context\n");
		ERR_print_errors_fp(stderr);
		die(NO_SSL);
		return 0;
	}

	if (SSL_CTX_use_certificate_file(ctx, SSL_CERTF, SSL_FILETYPE_PEM) <= 0)
	{
		fprintf(stderr, "Failure reading SSL certificate file: %s\n", SSL_CERTF);
		fflush(NULL);
		return 0;
	}
	printf("Loaded SSL certificate file: %s\n", SSL_CERTF);
	fflush(NULL);

	if (SSL_CTX_use_PrivateKey_file(ctx, SSL_KEYF, SSL_FILETYPE_PEM) <= 0)
	{
		fprintf(stderr, "Failure reading private key file: %s\n", SSL_KEYF);fflush(NULL);
		return 0;
	}
	printf("Opened private key file: %s\n", SSL_KEYF);fflush(NULL);

	if (!SSL_CTX_check_private_key(ctx)) {
		fprintf(stderr, "Private key does not match the certificate public key\n");fflush(NULL);
		return 0;
	}

	/*load and check that the key files are appropriate.*/
	printf("SSL security system enabled\n");
	return 1;
}
#endif /*SERVER_SSL*/
