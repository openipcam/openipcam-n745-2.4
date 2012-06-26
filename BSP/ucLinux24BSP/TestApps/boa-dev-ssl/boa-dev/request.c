/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996,97 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1996,97 Jon Nelson <nels0988@tc.umn.edu>
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

/* boa: request.c */

#include "boa.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#ifdef SERVER_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#include <syslog.h>

request *get_sock_request(int sock_fd);


int sockbufsize = SOCKETBUF_SIZE;

//extern int server_s;			/* boa socket */
extern int do_sock;				/*Do normal sockets??*/

#ifdef SERVER_SSL
extern SSL_CTX *ctx;			/*The global connection context*/
#endif /*SERVER_SSL*/


/*
 * the types of requests we can handle
 */

static struct {
	char	*str;
	int		 type;
} request_types[] = {
	{ "GET ",	M_GET },
	{ "POST ",	M_POST },
	{ "HEAD ",	M_HEAD },
	{ NULL,		0 }
};

/*
 * return the request type for a request,  short or invalid
 */

int request_type(request *req)
{
	int i, n, max_out = 0;
#if 0	//CXH_MODIFY
	for (i = 0; request_types[i].str; i++) {
		n = strlen(request_types[i].str);
		if (req->client_stream_pos < n) {
			max_out = 1;
			continue;
		}
		if (!memcmp(req->client_stream, request_types[i].str, n))
			return(request_types[i].type);
	}
#else
	int iStart;
	for (iStart = 0; iStart < req->client_stream_pos; iStart++)
		if (req->client_stream[iStart] != '\r' && req->client_stream[iStart] != '\n')
			break;
	for (i = 0; request_types[i].str; i++) {
		n = strlen(request_types[i].str);
		if (req->client_stream_pos < n + iStart) {
			max_out = 1;
			continue;
		}
		if (!memcmp(req->client_stream + iStart, request_types[i].str, n))
			return(request_types[i].type);
	}
#endif
	return(max_out ? M_SHORT : M_INVALID);
}


/*
 * Name: new_request
 * Description: Obtains a request struct off the free list, or if the
 * free list is empty, allocates memory
 *
 * Return value: pointer to initialized request
 */

request *new_request(void)
{
	request *req;

	if (request_free) {
		req = request_free;		/* first on free list */
		dequeue(&request_free, request_free);	/* dequeue the head */
	} else {
		req = (request *) malloc(sizeof(request));
		if (!req)
		{
			//die(OUT_OF_MEMORY);
			return NULL;
		}
	}

#ifdef SERVER_SSL
	req->ssl = NULL;
#endif /*SERVER_SSL*/

	memset(req, 0, sizeof(request) - NO_ZERO_FILL_LENGTH);

	return req;
}


/*
 * Name: get_request
 *
 * Description: Polls the server socket for a request.  If one exists,
 * does some basic initialization and adds it to the ready queue;.
 */

void get_request(int ifd_Server)
{
	get_sock_request(ifd_Server);
}


#ifdef SERVER_SSL
static ssize_t My_SSL_read(request *req, void *buf, size_t count)
{
	return SSL_read(req->ssl, buf, count);
}

static ssize_t My_SSL_write(request *req, const void *buf, size_t count)
{
	return SSL_write(req->ssl, buf, count);
}

void get_ssl_request(int ifd_Server)
{
	request *conn;
//	SSL *ssl;
	int err;

	conn = get_sock_request(ifd_Server);
	if (!conn)
		return;
//	printf("ssl socket done - %x\n", conn->fd);/*fflush(stdout); sleep(1);*/
	conn->ssl = SSL_new (ctx);
	if(conn->ssl == NULL){
		printf("Couldn't create ssl connection stuff\n");/*fflush(stdout); sleep(1);*/
		return;
	}
	conn->read = My_SSL_read;
	conn->write = My_SSL_write;

	SSL_set_fd (conn->ssl, conn->fd);
//	printf("SSL - Set the file des. \n");
	if(SSL_accept(conn->ssl) <= 0){
		ERR_print_errors_fp(stderr);
		return;
	}
	else{/*printf("SSL_accepted\n");*/}
//	printf("SSL - err is %x\n", err);/*fflush(stdout); sleep(1);*/
}
#endif /*SERVER_SSL*/


/* This routine works around an interesting problem encountered with IE
 * and the 2.4 kernel (i.e. no problems with netscape or with the 2.0 kernel).
 * The hassle is that the connection socket has a couple of crap bytes sent to
 * it by IE after the HTTP request.  When we close a socket with some crap waiting
 * to be read, the 2.4 kernel shuts down with a reset whereas earlier kernels
 * did the standard fin stuff.  IE complains about the reset and aborts page
 * rendering immediately.
 *
 * We must not loop here otherwise a DoS will have us for breakfast.
 */
static void safe_close(int fd) {
	fd_set rfd;
	struct timeval to;
	char buf[32];

	to.tv_sec = 0;
	to.tv_usec = 100;
	FD_ZERO(&rfd);
	FD_SET(fd, &rfd);
	if ((select(fd+1, &rfd, NULL, NULL, &to)) > 0 && FD_ISSET(fd, &rfd))
		read(fd, buf, sizeof buf);
	close(fd);
}

/*
 * Name: free_request
 *
 * Description: Deallocates memory for a finished request and closes
 * down socket.
 */

void free_request(request ** list_head_addr, request * req)
{
	if (req->buffer_end)
		return;

	dequeue(list_head_addr, req);	/* dequeue from ready or block list */

	if (req->buffer_end)
		FD_CLR(req->fd, &block_write_fdset);
	else {
		switch (req->status) {
		case WRITE:
			FD_CLR(req->fd, &block_write_fdset);
			break;
		default:
			FD_CLR(req->fd, &block_read_fdset);
		}
	}

	if (req->data_mem)
		free(req->data_mem);

//	if (req->data_fd)
//		close(req->data_fd);

	if (req->pcPostBuf != NULL)
		free(req->pcPostBuf);

	if (req->response_status >= 400)
		status.errors++;

	if ((req->keepalive == KA_ACTIVE) &&
		(req->response_status < 400) &&
		(++req->kacount < ka_max)) {
#if 0
		request *conn;

		conn = new_request();
		conn->fd = req->fd;
		conn->status = READ_HEADER;
		conn->header_line = conn->client_stream;
		conn->time_last = time(NULL);
		conn->kacount = req->kacount;

		/* we don't need to reset the fd parms for conn->fd because
		   we already did that for req */
		/* for log file and possible use by CGI programs */

		conn->remote_ip_addr = req->remote_ip_addr;

		/* for possible use by CGI programs */
		conn->remote_port = req->remote_port;

		status.requests++;

		if (conn->kacount + 1 == ka_max)
			SQUASH_KA(conn);
		block_request(conn);
#else
		/* Save keep-alive information. */
		int fd = req->fd;
		int kacount = req->kacount;
		struct in_addr remote_ip_addr = req->remote_ip_addr;
		int remote_port = req->remote_port;
#ifdef SERVER_SSL
		void *ssl = req->ssl;
#endif /*SERVER_SSL*/
		void *fun_read = req->read;
		void *fun_write = req->write;

		status.requests++;

		/* Clear information in "req". */
		if (req->pathname)
			free(req->pathname);
		if (req->query_string)
			free(req->query_string);
		if (req->pcExtraHeader)
			free(req->pcExtraHeader);
		memset(req, 0, sizeof(request) - NO_ZERO_FILL_LENGTH);

		/* Write keep-alive and init information. */
		req->fd = fd;
		req->status = READ_HEADER;
		req->header_line = req->client_stream;
		req->time_last = time(NULL);
		req->kacount = kacount;
#ifdef SERVER_SSL
		req->ssl = ssl;
#endif /*SERVER_SSL*/
		req->read = fun_read;
		req->write = fun_write;

		req->remote_ip_addr = remote_ip_addr;
		req->remote_port = remote_port;

		if (req->kacount + 1 == ka_max)
			SQUASH_KA(req);
		block_request(req);
		return;
#endif
	}
	else{
		if (req->fd != -1) {
			status.connections--;
//fprintf(stderr, "%d %d -- %d %d %d ", req->keepalive, KA_ACTIVE,
//		req->response_status, req->kacount, ka_max);

			safe_close(req->fd);
		}
		req->fd = -1;
#ifdef SERVER_SSL
		SSL_free(req->ssl);
#endif /*SERVER_SSL*/
	}


	if (req->pathname)
		free(req->pathname);
	if (req->query_string)
		free(req->query_string);
#if 1
	if (req->pcExtraHeader)
		free(req->pcExtraHeader);
#endif

	enqueue(&request_free, req);	/* put request on the free list */

	return;
}


/*
 * Name: process_requests
 *
 * Description: Iterates through the ready queue, passing each request
 * to the appropriate handler for processing.  It monitors the
 * return value from handler functions, all of which return -1
 * to indicate a block, 0 on completion and 1 to remain on the
 * ready list for more procesing.
 */

void process_requests(void)
{
	int retval = 0;
	request *current, *trailer;

	current = request_ready;

	while (current) {
#ifdef CRASHDEBUG
		crashdebug_current = current;
#endif
		if (current->buffer_end) {
			req_flush(current);

			if (current->status == CLOSE)
				retval = 0;
			else
				retval = 1;

		} else {

			switch (current->status) {
			case READ_HEADER:
			case ONE_CR:
			case ONE_LF:
			case TWO_CR:
				retval = read_header(current);
				break;
			case BODY_READ:
				retval = read_body(current);
				break;
			case WRITE:
				retval = process_get(current);
				break;
			default:
				retval = 0;
#if 0
				fprintf(stderr, "Unknown status (%d), closing!\n",
						current->status);
#endif
				break;
			}
		}

		if (lame_duck_mode)
			SQUASH_KA(current);

		switch (retval) {
		case -1:				/* request blocked */
			trailer = current;
			current = current->next;
			block_request(trailer);
			break;
		default:			/* everything else means an error, jump ship */
			send_r_error(current);
			/* fall-through */
		case 0:				/* request complete */
			trailer = current;
			current = current->next;
			free_request(&request_ready, trailer);
			break;
		case 1:				/* more to do */
			current->time_last = time(NULL);
			current = current->next;
			break;
		}
	}
#ifdef CRASHDEBUG
		crashdebug_current = current;
#endif
}

/*
 * Name: process_logline
 *
 * Description: This is called with the first req->header_line received
 * by a request, called "logline" because it is logged to a file.
 * It is parsed to determine request type and method, then passed to
 * translate_uri for further parsing.  Also sets up CGI environment if
 * needed.
 */

int process_logline(request * req)
{
	char *stop, *stop2;
	int i;
	static char *SIMPLE_HTTP_VERSION = "HTTP/0.9";

#if 1	//CXH_MODIFY, IE send "\r\n" */
	if (req->header_line[0] == '\0')
		return 1;
#endif


	req->logline = req->header_line;
	req->method = request_type(req);
	if (req->method == M_INVALID || req->method == M_SHORT) {
#if 0
		fprintf(stderr, "malformed request: \"%s\"\n", req->logline);
#endif
		my_syslog(LOG_ERR, "malformed request: \"%s\"\n", req->logline);
		send_r_bad_request(req);
		return 0;
	}

	/* Guaranteed to find ' ' since we matched a method above */
	stop = req->logline + 3;
	if (*stop != ' ')
		++stop;

	/* scan to start of non-whitespace */
	while (*(++stop) == ' ');

	stop2 = stop;

	/* scan to end of non-whitespace */
	while (*stop2 != '\0' && *stop2 != ' ')
		++stop2;

	if (stop2 - stop > MAX_HEADER_LENGTH) {
#if 0
		fprintf(stderr, "URI too long %d: \"%s\"\n", MAX_HEADER_LENGTH,
				req->logline);
#endif
		my_syslog(LOG_ERR, "URI too long %d: \"%s\"\n", MAX_HEADER_LENGTH,
				req->logline);
		send_r_bad_request(req);
		return 0;
	}
	memcpy(req->request_uri, stop, stop2 - stop);
	req->request_uri[stop2 - stop] = '\0';

	if (*stop2 == ' ') {
		/* if found, we should get an HTTP/x.x */
		int p1, p2;

		if (sscanf(++stop2, "HTTP/%d.%d", &p1, &p2) == 2 && p1 >= 1) {
			req->http_version = stop2;
			req->simple = 0;
		} else {
#if 0
			fprintf(stderr, "bogus HTTP version: \"%s\"\n", stop2);
#endif
			my_syslog(LOG_ERR, "bogus HTTP version: \"%s\"\n", stop2);
			send_r_bad_request(req);
			return 0;
		}

	} else {
		req->http_version = SIMPLE_HTTP_VERSION;
		req->simple = 1;
	}

	if (req->method == M_HEAD && req->simple) {
		my_syslog(LOG_ERR, "Simple HEAD request not allowed\n");
		send_r_bad_request(req);
		return 0;
	}

	return 1;
}

/*
 * Name: process_header_end
 *
 * Description: takes a request and performs some final checking before
 * init_cgi or init_get
 * Returns 0 for error or NPH, or 1 for success
 */

int process_header_end(request * req)
{
	if (!req->logline) {
		send_r_error(req);
		return 0;
	}

	/*MATT2 - I figured this was a good place to check for the MAC address*/
#ifdef CHECK_IP_MAC
	get_mac_from_IP(req);
	do_mac_crap(req);

	/*they could be on a remote lan, or just not in the arp cache*/
#endif

        if (translate_uri(req) == 0) {  /* unescape, parse uri */
                SQUASH_KA(req);
                return 0;               /* failure, close down */
        }

	if (g_pfunOnRequestBegin != NULL)
	{
		if ((*g_pfunOnRequestBegin)(req, NULL) == 0)
			return 0;
	}

	//pathname, eg: ".//index.htm"
	if (RunEmbedFun(req->request_uri, (void *)req) == 0)
	{//注:如果是POST请求要继续处理，不应返回0
		if (req->status == WRITE) return 1;
		else return 0;
	}

	if (req->method == M_POST)
		return 1;

	req->status = WRITE;
	return init_get(req);		/* get and head */
}

/*
 * Name: process_option_line
 *
 * Description: Parses the contents of req->header_line and takes
 * appropriate action.
 */

void process_option_line(request * req)
{
	char c, *value, *line = req->header_line;

/* Start by aggressively hacking the in-place copy of the header line */

#ifdef FASCIST_LOGGING
	fprintf(stderr, "\"%s\"\n", line);
#endif

	value = strchr(line, ':');
	if (value == NULL)
		return;
	*value++ = '\0';			/* overwrite the : */
	to_upper(line);				/* header types are case-insensitive */
	while ((c = *value) && (c == ' ' || c == '\t'))
		value++;

	if (!memcmp(line, "IF_MODIFIED_SINCE", 18) && !req->if_modified_since)
		req->if_modified_since = value;

	else if (!memcmp(line, "CONTENT_TYPE", 13) && !req->content_type)
		req->content_type = value;

	else if (!memcmp(line, "CONTENT_LENGTH", 15) && !req->content_length)
		req->content_length = value;

	else if (!memcmp(line, "HOST",5) && !req->host)
		req->host = value;

	else if (!memcmp(line, "REFERER", 8) && !req->referer)
		req->referer = value;

#ifdef USE_AUTH
	else if (!memcmp(line,"AUTHORIZATION",14) && !req->authorization)
		req->authorization = value;
#endif

	else if (!memcmp(line, "CONNECTION", 11) &&
			 ka_max &&
			 req->keepalive != KA_STOPPED)
		req->keepalive = (!strncasecmp(value, "Keep-Alive", 10) ?
						  KA_ACTIVE : KA_STOPPED);

#ifdef ACCEPT_ON
	else if (!memcmp(line, "ACCEPT", 7))
		add_accept_header(req, value);
#endif

	return;
}

/*
 * Name: add_accept_header
 * Description: Adds a mime_type to a requests accept char buffer
 *   silently ignore any that don't fit -
 *   shouldn't happen because of relative buffer sizes
 */

void add_accept_header(request * req, char *mime_type)
{
#ifdef ACCEPT_ON
	int l = strlen(req->accept);

	if ((strlen(mime_type) + l + 2) >= MAX_HEADER_LENGTH)
		return;

	if (req->accept[0] == '\0')
		strcpy(req->accept, mime_type);
	else {
		sprintf(req->accept + l, ", %s", mime_type);
	}
#endif
}

void free_requests(void)

{
	request *ptr, *next;

	ptr = request_free;
	while(ptr != NULL) {
		next = ptr->next;
		/*Free the socket stuff if it exists*/

		free(ptr);
		ptr = next;
	}
	request_free = NULL;
}

#if 0
/*
 * Name: dump_request
 *
 * Description: Prints request to stderr for debugging purposes.
 */
void dump_request(request*req)
{
	fputs("-----[ REQUEST DUMP ]-----\n",stderr);
	if (!req)
	{
		fputs("no request!\n",stderr);
		return;
	}
	fprintf(stderr,"Logline: %s\n",req->logline);
	fprintf(stderr,"request_uri: %s\n",req->request_uri);
	fprintf(stderr,"Pathname: %s\n",req->pathname);
	fprintf(stderr,"Status: %u\n",req->status);
	fprintf(stderr,"Host: %s\n",req->host);
	fprintf(stderr,"remote_ip_addr: %x\n",req->remote_ip_addr);
	fputs("---------------------------\n\n",stderr);
}
#endif


ssize_t (My_Common_read)(request *req, void *buf, size_t count)
{
	return read(req->fd, buf, count);
}

ssize_t (My_Common_write)(request *req, const void *buf, size_t count)
{
	return write(req->fd, buf, count);
}

request *get_sock_request(int sock_fd)
{
	int fd;						/* socket */
	struct sockaddr_in remote_addr;		/* address */
	int remote_addrlen = sizeof(remote_addr);
	request *conn;				/* connection */

	if (max_connections != -1 && status.connections >= max_connections)
		return NULL;

	remote_addr.sin_family = 0xdead;
	fd = accept(sock_fd, (struct sockaddr *) &remote_addr, &remote_addrlen);
	if (fd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)	/* no requests */
			return NULL;
		else {					/* accept error */
#if 0
			perror("accept");
#endif
			return NULL;
		}
	}
#ifdef DEBUGNONINET
	/*  This shows up due to race conditions in some Linux kernels
	 *  when the client closes the socket sometime between
	 *  the select() and accept() syscalls.
	 *  Code and description by Larry Doolittle <ldoolitt@jlab.org>
	 */
#define HEX(x) (((x)>9)?(('a'-10)+(x)):('0'+(x)))
	if (remote_addr.sin_family != AF_INET) {
		struct sockaddr *bogus = (struct sockaddr *) &remote_addr;
		char *ap, ablock[44];
		int i;
		close(fd);

		for (ap = ablock, i = 0; i < remote_addrlen && i < 14; i++) {
			*ap++ = ' ';
			*ap++ = HEX((bogus->sa_data[i] >> 4) & 0x0f);
			*ap++ = HEX(bogus->sa_data[i] & 0x0f);
		}
		*ap = '\0';
#ifdef BOA_TIME_LOG
		fprintf(stderr, "non-INET connection attempt: socket %d, "
				"sa_family = %hu, sa_data[%d] = %s\n",
				fd, bogus->sa_family, remote_addrlen, ablock);
#endif
		return NULL;
	}
#endif

	if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
		sizeof(sock_opt))) == -1){
			die(NO_SETSOCKOPT);
			return NULL;
	}

	conn = new_request();
	if (conn == NULL)
	{
		close(fd);
		return NULL;
	}

	conn->read = My_Common_read;
	conn->write = My_Common_write;
	conn->fd = fd;
	conn->status = READ_HEADER;
	conn->header_line = conn->client_stream;
	conn->time_last = time(NULL);
#ifdef USE_CHARSET_HEADER
	conn->send_charset = 1;
#endif

	/* nonblocking socket */
	if (fcntl(conn->fd, F_SETFL, NOBLOCK) == -1) {
#if 0
		perror("request.c, fcntl");
#endif
	}
	/* set close on exec to true */
	if (fcntl(conn->fd, F_SETFD, 1) == -1) {
#if 0
		perror("request.c, fcntl-close-on-exec");
#endif
	}

	/* large buffers */
	if (setsockopt(conn->fd, SOL_SOCKET, SO_SNDBUF, (void *) &sockbufsize,
				   sizeof(sockbufsize)) == -1)
		die(NO_SETSOCKOPT);

	/* for log file and possible use by CGI programs */
	conn->remote_ip_addr = remote_addr.sin_addr;

	/* for possible use by CGI programs */
	conn->remote_port = ntohs(remote_addr.sin_port);


	status.requests++;
	status.connections++;

	/* Thanks to Jef Poskanzer <jef@acme.com> for this tweak */
	{
		int one = 1;
		if (setsockopt(conn->fd, IPPROTO_TCP, TCP_NODELAY, (void *) &one,
			sizeof(one)) == -1){
			die(NO_SETSOCKOPT);
			return NULL;
		}
	}
	enqueue(&request_ready, conn);
	return conn;
}

