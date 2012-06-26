/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1996,97 Jon Nelson <nels0988@tc.umn.edu>
 *  Some changes Copyright (C) 1998 Martin Hinner <martin@tdp.cz>
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

#include <syslog.h>
#include "boa.h"

/*
 * Name: init_get
 * Description: Initializes a non-script GET or HEAD request.
 *
 * Return values:
 *   0: finished or error, request will be freed
 *   1: successfully initialized, added to ready queue
 */

int init_get(request * req)
{
	int data_fd;
	char buf[MAX_PATH_LENGTH];
	struct stat statbuf;

	data_fd = open(req->pathname, O_RDONLY, 0);

	if (data_fd == -1) {		/* cannot open */
			int errno_save = errno;
			errno = errno_save;
#if 0
			perror("document open");
#endif
printf("%s", req->pathname);PTI;
			my_syslog(LOG_ERR, "Error opening %s, %s\n", req->pathname,
					strerror(errno_save));

			errno = errno_save;

			if (errno == ENOENT)
				send_r_not_found(req);
			else if (errno == EACCES)
				send_r_forbidden(req);
			else
				send_r_bad_request(req);
			return 0;
	}
	fstat(data_fd, &statbuf);

	if (S_ISDIR(statbuf.st_mode)) {		/* directory */
		close(data_fd);			/* close dir */

		data_fd = get_dir(req, &statbuf);	/* updates statbuf */

		if (data_fd == -1)		/* couldn't do it */
			return 0;			/* errors reported by get_dir */
		else if (data_fd == 0)
			return 1;
	}
	if (req->if_modified_since &&
		!modified_since(&(statbuf.st_mtime), req->if_modified_since)) {
		send_r_not_modified(req);
		close(data_fd);
		return 0;
	}
	req->filesize = statbuf.st_size;
	req->last_modified = statbuf.st_mtime;

	if (req->method == M_HEAD) {
		send_r_request_ok(req);
		close(data_fd);
		return 0;
	}

	req->data_mem_length = req->filesize;
	req->data_mem = (char *)malloc(req->filesize);
	if (req->data_mem)
	{
		if (read(data_fd, req->data_mem, req->filesize) != req->filesize)
		{
			free(req->data_mem);
			req->data_mem = NULL;
		}
	}
	close(data_fd);
	if (req->data_mem == NULL)
	{
		boa_perror(req, "mmap");
		return 0;
	}

	send_r_request_ok(req);		/* All's well */
	{
		int bob;
		bob = BUFFER_SIZE - req->buffer_end;
		if (bob > 0) {
			if (bob > req->filesize - req->filepos)
				bob = req->filesize - req->filepos;
			memcpy(req->buffer + req->buffer_end,
					req->data_mem + req->filepos,
					bob);
			req->buffer_end += bob;
			req->filepos += bob;
		}
	}

	if (req->filepos == req->filesize) {
		req->status = CLOSE;
		return 0; /* done! */
	}

	/* We lose statbuf here, so make sure response has been sent */
	return 1;
}

/*
 * Name: process_get
 * Description: Writes a chunk of data to the socket.
 *
 * Return values:
 *  -1: request blocked, move to blocked queue
 *   0: EOF or error, close it down
 *   1: successful write, recycle in ready queue
 */

int process_get(request * req)
{
	int bytes_written, bytes_to_write;

	bytes_to_write = req->filesize - req->filepos;
	bytes_written = (*req->write)(req, req->data_mem + req->filepos,
						  bytes_to_write);

	if (bytes_written == -1)
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return -1;			/* request blocked at the pipe level, but keep going */
		else {
			if (errno != EPIPE) {
#if 0
				perror("write");	/* OK to disable if your logs get too big */
#endif
			}
			return 0;
		}
	req->filepos += bytes_written;
//printf("%d\n", bytes_written);
	if (req->filepos == req->filesize)	/* EOF */
	{
		req->filepos = 0;
		req->filesize = 0;
		if (req->funOnSendDataOver)
		{
			if ((*req->funOnSendDataOver)((HTTPCONNECTION)req, &req->tLastFillData2Send, req->pParamForFunSendDataOver) == 0)
				return 0;
			else return 1;
		}
		else return 0;
	}
	else
		return 1;				/* more to do */
}

/*
 * Name: get_dir
 * Description: Called from process_get if the request is a directory.
 * statbuf must describe directory on input, since we may need its
 *   device, inode, and mtime.
 * statbuf is updated, since we may need to check mtimes of a cache.
 * returns:
 *  -1 error
 *  0  cgi (either gunzip or auto-generated)
 *  >0  file descriptor of file
 */

int get_dir(request * req, struct stat *statbuf)
{

	char pathname_with_index[MAX_PATH_LENGTH];
	int data_fd;
	if (req->pathname[strlen(req->pathname)-1] != '/')
	{
		send_r_forbidden(req);
		return -1;
	}
	sprintf(pathname_with_index, "%s%s", req->pathname, directory_index);

	data_fd = open(pathname_with_index, O_RDONLY, 0);

	if (data_fd != -1) {		/* user's index file */
		strcat(req->request_uri, directory_index); /* for mimetype */
		fstat(data_fd, statbuf);
		return data_fd;
	}

	if (errno == EACCES) {
		send_r_forbidden(req);
		return -1;
	}

	send_r_forbidden(req);
	return -1;					/* nothing worked */
}

