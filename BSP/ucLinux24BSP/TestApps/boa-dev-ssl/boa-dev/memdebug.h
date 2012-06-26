#ifndef MEMDEBUG_H
#define MEMDEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

static inline void *save_calloc(size_t nmemb, size_t nbytes)
{
	return calloc(nmemb, nbytes);
}
#undef calloc
#define calloc(nmemb,nbytes) my_calloc(nmemb,nbytes,__LINE__,__FILE__)
void *my_calloc(size_t nmemb, size_t nbytes, int iline, char *pcfile);

static inline void *save_malloc(size_t nbytes)
{
	return malloc(nbytes);
}
#undef malloc
#define malloc(nbytes) my_malloc(nbytes,__LINE__,__FILE__)
void *my_malloc(size_t nbytes, int iline, char *pcfile);

static inline void *save_realloc(void *aptr, size_t nbytes)
{
	return realloc(aptr, nbytes);
}
#undef realloc
#define realloc(aptr,nbytes) my_realloc(aptr,nbytes,__LINE__,__FILE__)
void *my_realloc(void * aptr, size_t nbytes, int iline, char *pcfile);

static inline void save_free(void *aptr)
{
	return free(aptr);
}
#undef free
#define free(aptr) my_free(aptr,__LINE__,__FILE__)
void my_free(void * aptr, int iline, char *pcfile);

static inline char *save_strdup(const char *s)
{
	return strdup(s);
}
#undef strdup
#define strdup(s) my_strdup(s,__LINE__,__FILE__)
char *my_strdup(const char *s, int iline, char *pcfile);

static inline char *save_strndup(const char *s, size_t n)
{
	/* strndup may not exists on some platform.
	 * return strndup(s, n);
	 */

	char *p;
	size_t l;

	if (s == NULL) return NULL;
	l = strlen(s);
	if (l > n) l = n;

	p = (char *)save_malloc(l + 1);
	if (p != NULL)
	{
		memcpy(p, s, l);
		p[l] = '\0';
	}

	return p;
}
#undef strndup
#define strndup(s,n) my_strndup(s,n,__LINE__,__FILE__)
char *my_strndup(const char *s, size_t n, int iline, char *pcfile);

static inline int save_open(const char *pathname, int flags, mode_t mode)
{
	return open(pathname, flags, mode);
}
#undef open
#define open(pathname,flags...) my_open(__LINE__,__FILE__,pathname,flags)
int my_open(int iline, char *pcfile, const char *pathname, int flags, ...);

static inline int save_creat(const char *pathname, mode_t mode)
{
	return creat(pathname, mode);
}
#undef creat
#define creat(pathname,mode) my_creat(__LINE__,__FILE__,pathname,mode)
int my_creat(int iline, char *pcfile, const char *pathname, mode_t mode);

static inline int save_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
}
#undef socket
#define socket(domain,type,protocol) my_socket(domain,type,protocol,__LINE__,__FILE__)
int my_socket(int domain, int type, int protocol, int iline, char *pcfile);

static inline int save_accept(int s, struct sockaddr *addr, int *addrlen)
{
	return accept(s, addr, addrlen);
}
#undef accept
#define accept(s,addr, addrlen) my_accept(s,addr,addrlen,__LINE__,__FILE__)
int my_accept(int s, struct sockaddr *addr, int *addrlen, int iline, char *pcfile);

static inline int save_pipe(int filedes[2])
{
	return pipe(filedes);
}
#undef pipe
#define pipe(filedes) my_pipe(filedes,__LINE__,__FILE__)
int my_pipe(int filedes[2], int iline, char *pcfile);

static inline int save_dup(int oldfd)
{
	return dup(oldfd);
}
#undef dup
#define dup(oldfd) my_dup(oldfd,__LINE__,__FILE__)
int my_dup(int oldfd, int iline, char *pcfile);

//dup2???

int my_close(int fd, int iline, char *pcfile);
static inline int save_close(int fd)
{
	return close(fd);
}
#undef close
#define close(a) my_close(a,__LINE__,__FILE__)
int my_close(int fd, int iline, char *pcfile);



static inline FILE *save_fopen(const char *path, const char *mode)
{
	return fopen(path, mode);
}
#undef fopen
#define fopen(path,mode) my_fopen(path,mode,__LINE__,__FILE__)
FILE *my_fopen(const char *path, const char *mode, int iline, char *pcfile);

static inline int save_fclose(FILE *stream)
{
	return fclose(stream);
}
#define fclose(stream) my_fclose(stream,__LINE__,__FILE__)
int my_fclose(FILE *stream, int iline, char *pcfile);


void pt_mem_result();
void pt_fd_result();
void pt_fp_result();

#endif
