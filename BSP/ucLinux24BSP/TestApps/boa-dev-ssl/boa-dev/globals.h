/* vi:set tabstop=2: */
/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996,97 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1997 Jon Nelson <nels0988@tc.umn.edu>
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

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "defines.h"
#include "compat.h"

#if 1
#include "Help/HttpServer.h"

typedef struct tagEMBEDFUN_T
{
	char *pcAccessName;
	REQUEST_CALLBACK_PFUN funRequestCallBack;
	void *pParam;
#ifdef USE_AUTH
	int iPrivilegeRequired;	/* visit's privilege must >= iVisitPrivilege */
#endif
	struct tagEMBEDFUN_T	*pNextFun;
} EMBEDFUN_T;

extern REQUEST_CALLBACK_PFUN g_pfunOnRequestBegin;
extern EMBEDFUN_T *g_pEmbedFunList;
int RunEmbedFun(char *pcAccessName, HTTPCONNECTION hc);

#ifdef USE_AUTH
extern LIST *g_pAuthList[3];
extern int g_iAuthEnable;
#endif
#endif

typedef void * DATA_SEND_OVER_PFUN;

struct request {				/* pending requests */
	int fd;						/* client's socket fd */
	char *pathname;				/* pathname of requested file */
	int status:5;					/* see #defines.h */
	int simple:1;					/* simple request? */
	int keepalive:3;				/* keepalive status */
	int kacount;				/* keepalive count */

//	int data_fd;				/* fd of data */
	unsigned long filesize;		/* filesize */
	unsigned long filepos;		/* position in file */
	char *data_mem;				/* mmapped/malloced char array */
#if 1
	int data_mem_length;
#endif
	time_t time_last;			/* time of last succ. op. */
	int method;					/* M_GET, M_POST, etc. */

	char *logline;				/* line to log file */

	int client_stream_pos;		/* how much have we read... */
	char *header_line;
	char *header_end;
	int buffer_start;
	int buffer_end;

	char *http_version;			/* HTTP/?.? of req */
	int response_status;		/* R_NOT_FOUND etc. */

	char *if_modified_since;	/* If-Modified-Since */
	struct in_addr remote_ip_addr;	/* do NOT after inet_ntoa */
#ifdef CHECK_IP_MAC
	unsigned char acRemoteMac[6];
#endif
	int remote_port;			/* could be used for ident */
	char *host;                     /* used for HTTP Host: line */

	time_t last_modified;		/* Last-modified: */
#ifdef USE_CHARSET_HEADER
	int send_charset;
#endif

#if 1
	char *pcExtraHeader;

	int iPostState;	/* 当前POST数据的读取状态 */
	char *pcPostBuf;	/* 存放POST数据的缓冲区 */
	int iPostBufLen;	/* POST缓冲区长度 */
	int iPostDataLen;	/* POST缓冲区中实际有效数据的长度 */
	POST_DATA_PFUN funPostDataGot;	/* 当POST缓冲区中达到iPostBufLen或不再有时的回调函数 */
	void *pParamForFunPostDataGot;	/* 其他传给funPostDataGot的参数 */

	SEND_DATA_OVER_PFUN funOnSendDataOver;	/* 当所有数据都写完 */
	time_t tLastFillData2Send;
	void *pParamForFunSendDataOver;
#endif
	/* CGI needed vars */

	int cgi_status;				/* similar to status */

	char *query_string;			/* env variable */
	char *content_type;			/* env variable */
	char *content_length;		/* env variable */
	char *referer;				/* referer*/

#ifdef USE_AUTH
	char *authorization;
	int iVisitPrivilege;
	int iPrivilegeRequired;	/* iVisitPrivilege >= iPrivilegeRequired*/
#endif

	struct request *next;		/* next */
	struct request *prev;		/* previous */

	char buffer[BUFFER_SIZE + 1];			/* generic I/O buffer */
	char request_uri[MAX_HEADER_LENGTH + 1];	/* uri */
	char client_stream[CLIENT_STREAM_SIZE];		/* data from client - fit or be hosed */
#ifdef ACCEPT_ON
	char accept[MAX_ACCEPT_LENGTH];		/* Accept: fields */
#endif
#ifdef SERVER_SSL
	void* ssl;
#endif /*SERVER_SSL*/
	ssize_t (*read)(struct request *req, void *buf, size_t count);
	ssize_t (*write)(struct request *req, const void *buf, size_t count);
};

#ifdef ACCEPT_ON

#define NO_ZERO_FILL_LENGTH (BUFFER_SIZE + 1 + \
                             MAX_HEADER_LENGTH + 1 + \
                             CLIENT_STREAM_SIZE + \
                             MAX_ACCEPT_LENGTH)

#else

#define NO_ZERO_FILL_LENGTH (BUFFER_SIZE + 1 + \
                             MAX_HEADER_LENGTH + 1 + \
                             CLIENT_STREAM_SIZE)

#endif

/* how does an array of chars of size zero get treated */

typedef struct request request;
#ifdef CRASHDEBUG
request *crashdebug_current;
#endif

struct status {
	long requests;
	long errors;
	long connections;
};

struct status status;

extern request *request_ready;	/* first in ready list */
extern request *request_block;	/* first in blocked list */
extern request *request_free;	/* first in free list */

extern fd_set block_read_fdset;	/* fds blocked on read */
extern fd_set block_write_fdset;	/* fds blocked on write */

extern int sock_opt;			/* sock_opt = 1: for setsockopt */

/* global server variables */

extern char *document_root;
extern char *directory_index;
extern char *default_type;

extern int ka_timeout;
extern int ka_max;

extern int lame_duck_mode;

extern int backlog;

extern int max_connections;

#endif
