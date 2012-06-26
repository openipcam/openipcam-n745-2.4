#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


typedef struct
{
	int init;
	pthread_mutex_t ptm;
} my_pthread_mutex_t;

#define MY_PTHREAD_MUTEX_INITIALIZER {0, }

static int my_pthread_mutex_lock(my_pthread_mutex_t *mutex)
{
	if (!mutex->init)
	{
		mutex->init = 1;
		pthread_mutex_init(&mutex->ptm, NULL);
	}

	return pthread_mutex_lock(&mutex->ptm);
}

static int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex)
{
	return pthread_mutex_unlock(&mutex->ptm);
}


typedef struct my_mem_list_struct
{
	void *ptr;
	int isize;
	int iline;
	char *pcfile;
	struct my_mem_list_struct *pnext;
} my_mem_list_struct_t;

my_mem_list_struct_t *g_pmemheader = NULL;
my_pthread_mutex_t g_ptmmem = MY_PTHREAD_MUTEX_INITIALIZER;

static add_mem_list(my_mem_list_struct_t **header, char *ptr, int isize, int iline, char *pcfile)
{
	my_mem_list_struct_t *p;
	if (ptr == NULL) return;

	my_pthread_mutex_lock(&g_ptmmem);
	for (p=*header; p!=NULL; p=p->pnext)
	{
		if (p->ptr == ptr)
		{
			fprintf(stderr, "add_mem_list error in %s(%s %d).!", __FILE__, pcfile, iline);
			my_pthread_mutex_unlock(&g_ptmmem);
			exit(-1);
		}
	}

	p = (my_mem_list_struct_t *)save_malloc(sizeof(my_mem_list_struct_t));
	if (p == NULL)
	{
		fprintf(stderr, "not enough memory in %s.\n", __FILE__);
		my_pthread_mutex_unlock(&g_ptmmem);
		exit(-1);
	}

	p->ptr = ptr;
	p->isize = isize;
	p->iline = iline;
	p->pcfile = save_strdup(pcfile);
	p->pnext = *header;
	*header = p;
	my_pthread_mutex_unlock(&g_ptmmem);
}


static del_mem_list(my_mem_list_struct_t **header, char *ptr, int iline, char *pcfile)
{
	my_mem_list_struct_t *p, *pre;

	if (ptr == NULL) return;

	my_pthread_mutex_lock(&g_ptmmem);
	for (p=*header; p!=NULL; pre=p, p=p->pnext)
	{
		if (p->ptr == ptr)
			break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "del_mem_list error in %s(%s %d %d).!", __FILE__, pcfile, iline, ptr);
		for (p = *header; p!=NULL; p=p->pnext)
			printf("%d ", p->ptr);
		my_pthread_mutex_unlock(&g_ptmmem);
//		return;
		exit(-1);
	}

	if (p == *header)
		*header = p->pnext;
	else
		pre->pnext = p->pnext;
	save_free(p->pcfile);
	save_free(p);
	my_pthread_mutex_unlock(&g_ptmmem);
}

void *my_calloc(size_t nmemb, size_t nbytes, int iline, char *pcfile)
{
	char *p;
	p = save_calloc(nmemb, nbytes);

	add_mem_list(&g_pmemheader, p, nbytes, iline, pcfile);
	return p;
}

void *my_malloc(size_t nbytes, int iline, char *pcfile)
{
	void *p;
	p = save_malloc(nbytes);

	add_mem_list(&g_pmemheader, p, nbytes, iline, pcfile);
	return p;
}

void *my_realloc(void * aptr, size_t nbytes, int iline, char *pcfile)
{
	void *p;
	p = save_realloc(aptr, nbytes);
	if (p != NULL)
	{
		del_mem_list(&g_pmemheader, aptr, iline, pcfile);
		add_mem_list(&g_pmemheader, p, nbytes, iline, pcfile);
	}
	return p;
}

void my_free(void * aptr, int iline, char *pcfile)
{
	save_free(aptr);
	del_mem_list(&g_pmemheader, aptr, iline, pcfile);
	return;
}

char *my_strdup(const char *s, int iline, char *pcfile)
{
	char *p;

	p = save_strdup(s);
	if (p != NULL)
		add_mem_list(&g_pmemheader, p, strlen(p) + 1, iline, pcfile);
	return p;
}

char *my_strndup(const char *s, size_t n, int iline, char *pcfile)
{
	char *p;

	p = save_strndup(s, n);
	if (p != NULL)
		add_mem_list(&g_pmemheader, p, strlen(p) + 1, iline, pcfile);
	return p;
}


////////////////////////////////////////////////////////////////////////

typedef struct my_fd_list_struct
{
	int fd;
	int iline;
	char *pcfile;
	pthread_t pid;
	struct my_fd_list_struct *pnext;
} my_fd_list_struct_t;
my_fd_list_struct_t *g_pfdheader = NULL;
my_pthread_mutex_t g_ptmfd = MY_PTHREAD_MUTEX_INITIALIZER;


static add_fd_list(my_fd_list_struct_t **header, int fd, int iline, char *pcfile)
{
	pthread_t pid;
	my_fd_list_struct_t *p;
	if (fd == 0) return;

	pid=0;//pid = pthread_self();
	my_pthread_mutex_lock(&g_ptmfd);

	for (p=*header; p!=NULL; p=p->pnext)
	{
		if (p->fd == fd && p->pid == pid)
		{
			fprintf(stderr, "add_fd_list error in %s(%s %d).!", __FILE__, pcfile, iline);
			my_pthread_mutex_unlock(&g_ptmfd);
			exit(-1);
		}
	}

	p = (my_fd_list_struct_t *)save_malloc(sizeof(my_fd_list_struct_t));
	if (p == NULL)
	{
		fprintf(stderr, "not enough memory in %s.\n", __FILE__);
		my_pthread_mutex_unlock(&g_ptmfd);
		exit(-1);
	}

	p->fd = fd;
	p->iline = iline;
	p->pcfile = save_strdup(pcfile);
	p->pnext = *header;
	p->pid = pid;
	*header = p;
	my_pthread_mutex_unlock(&g_ptmfd);
}


static del_fd_list(my_fd_list_struct_t **header, int fd, int iline, char *pcfile)
{
	my_fd_list_struct_t *p, *pre;
	pthread_t pid;

	if (fd == 0) return;

	pid=0;//pid = pthread_self();
	my_pthread_mutex_lock(&g_ptmfd);

	for (p=*header; p!=NULL; pre=p, p=p->pnext)
	{
		if (p->fd == fd && p->pid == pid)
			break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "del_fd_list error in %s(%s %d %d).!", __FILE__, pcfile, iline, fd);
		for (p = *header; p!=NULL; p=p->pnext)
			printf("%d ", p->fd);
		my_pthread_mutex_unlock(&g_ptmfd);
//		return;
		exit(-1);
	}

	if (p == *header)
		*header = p->pnext;
	else
		pre->pnext = p->pnext;
	save_free(p->pcfile);
	save_free(p);
	my_pthread_mutex_unlock(&g_ptmfd);
}

int my_open(int iline, char *pcfile, const char *pathname, int flags, ...)
{
	int r;
        mode_t mode;

        if (flags & O_CREAT)
        {
		va_list arg;
		va_start(arg, flags);
		mode = va_arg(arg, mode_t);
		va_end(arg);
	}
	else mode = 0;
	
	r = save_open(pathname, flags, mode);
	if (r >= 0) add_fd_list(&g_pfdheader, r, iline, pcfile);
	return r;
}

int my_creat(int iline, char *pcfile, const char *pathname, mode_t mode)
{
	int r;
	r = save_creat(pathname, mode);
	if (r >= 0) add_fd_list(&g_pfdheader, r, iline, pcfile);
	return r;
}

int my_socket(int domain, int type, int protocol, int iline, char *pcfile)
{
	int r;
	r = save_socket(domain, type, protocol);
	if (r >= 0) add_fd_list(&g_pfdheader, r, iline, pcfile);
	return r;
}

int my_accept(int s, struct sockaddr *addr, int *addrlen, int iline, char *pcfile)
{
	int r;

	r = save_accept(s, addr, addrlen);
	if (r >= 0) add_fd_list(&g_pfdheader, r, iline, pcfile);
	return r;
}

int my_pipe(int filedes[2], int iline, char *pcfile)
{
	int r;
	r = save_pipe(filedes);
	if (r != -1)
	{
		add_fd_list(&g_pfdheader, filedes[0], iline, pcfile);
		add_fd_list(&g_pfdheader, filedes[1], iline, pcfile);
	}
	return r;
}

int my_dup(int oldfd, int iline, char *pcfile)
{
	int r;
	r = save_dup(oldfd);

	if (r != -1) add_fd_list(&g_pfdheader, r, iline, pcfile);
	return r;
}

int my_close(int fd, int iline, char *pcfile)
{
	int r;
	r = save_close(fd);
	if (r == 0) del_fd_list(&g_pfdheader, fd, iline, pcfile);
	return r;
}



///////////////////////////////////////////

typedef struct my_fp_list_struct
{
	FILE *fp;
	int iline;
	char *pcfile;
	pthread_t pid;
	struct my_fp_list_struct *pnext;
} my_fp_list_struct_t;
my_fp_list_struct_t *g_pfpheader = NULL;
my_pthread_mutex_t g_ptmfp = MY_PTHREAD_MUTEX_INITIALIZER;



static add_fp_list(my_fp_list_struct_t **header, FILE *fp, int iline, char *pcfile)
{
	pthread_t pid;
	my_fp_list_struct_t *p;
	if (fp == NULL) return;

	pid=0;//pid = pthread_self();
	my_pthread_mutex_lock(&g_ptmfp);
	for (p=*header; p!=NULL; p=p->pnext)
	{
		if (p->fp == fp && p->pid == pid)
		{
			fprintf(stderr, "add_fp_list error in %s(%s %d).!", __FILE__, pcfile, iline);
			my_pthread_mutex_unlock(&g_ptmfp);
			exit(-1);
		}
	}

	p = (my_fp_list_struct_t *)save_malloc(sizeof(my_fp_list_struct_t));
	if (p == NULL)
	{
		fprintf(stderr, "not enough memory in %s.\n", __FILE__);
		my_pthread_mutex_unlock(&g_ptmfp);
		exit(-1);
	}

	p->fp = fp;
	p->iline = iline;
	p->pcfile = save_strdup(pcfile);
	p->pnext = *header;
	p->pid = pid;
	*header = p;
	my_pthread_mutex_unlock(&g_ptmfp);
}


static del_fp_list(my_fp_list_struct_t **header, FILE *fp, int iline, char *pcfile)
{
	my_fp_list_struct_t *p, *pre;
	pthread_t pid;

	if (fp == NULL) return;
	pid=0;//pid = pthread_self();
	my_pthread_mutex_lock(&g_ptmfp);

	for (p=*header; p!=NULL; pre=p, p=p->pnext)
	{
		if (p->fp == fp && p->pid == pid)
			break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "del_fp_list error in %s(%s %d %d).!", __FILE__, pcfile, iline, fp);
		for (p = *header; p!=NULL; p=p->pnext)
			printf("%d ", p->fp);
		my_pthread_mutex_unlock(&g_ptmfp);
//		return;
		exit(-1);
	}

	if (p == *header)
		*header = p->pnext;
	else
		pre->pnext = p->pnext;
	save_free(p->pcfile);
	save_free(p);
	my_pthread_mutex_unlock(&g_ptmfp);
}


FILE *my_fopen(const char *path, const char *mode, int iline, char *pcfile)
{
	FILE *fp;
	fp = save_fopen(path, mode);
	add_fp_list(&g_pfpheader, fp, iline, pcfile);
	return fp;
}

int my_fclose(FILE *stream, int iline, char *pcfile)
{
	int r;
	r = save_fclose(stream);
	if (r == 0) del_fp_list(&g_pfpheader, stream, iline, pcfile);
	return r;
}
















//-------------------------
void pt_mem_result()
{
	int i;
	my_mem_list_struct_t *p;
	if (g_pmemheader == NULL)
		fprintf(stderr, "No memory leaks.\n");

	for (p=g_pmemheader; p!=NULL; p=p->pnext)
	{
		fprintf(stderr, "\33[1m\33[41mMemory leak:\33[0m \33[1m\33[32m%s\33[0m \33[1m\33[33m%d\33[0m, \33[4m%d bytes\33[0m\n", p->pcfile, p->iline, p->isize);
	}
}

void pt_fd_result()
{
	my_fd_list_struct_t *p;
	if (g_pfdheader == NULL)
		fprintf(stderr, "No fd open()ing.\n");

	for (p=g_pfdheader; p!=NULL; p=p->pnext)
	{
		fprintf(stderr, "\33[1m\33[41mfd non-close:\33[0m \33[1m\33[32m%s\33[0m \33[1m\33[33m%d\33[0m\n", p->pcfile, p->iline);
	}
}

void pt_fp_result()
{
	my_fp_list_struct_t *p;
	if (g_pfpheader == NULL)
		fprintf(stderr, "No fp fopen()ing.\n");

	for (p=g_pfpheader; p!=NULL; p=p->pnext)
	{
		fprintf(stderr, "\33[1m\33[41mfp non-fclose:\33[0m \33[1m\33[32m%s\33[0m \33[1m\33[33m%d\33[0m\n", p->pcfile, p->iline);
	}
}

