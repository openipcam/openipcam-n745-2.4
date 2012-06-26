/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1996,97 Jon Nelson <nels0988@tc.umn.edu>
 *  Some changes Copyright (C) 1996 Russ Nelson <nelson@crynwr.com>
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

/* boa: alias.c */

#include "boa.h"
#include <sys/stat.h>
#include <syslog.h>

/*
 * Name: translate_uri
 *
 * Description: Parse a request's virtual path.  Sets path_info,
 * query_string, pathname, and script_name data if it's a
 * ScriptAlias or a CGI.  Note -- this should be broken up.
 *
 * Return values:
 *   0: failure, close it down
 *   1: success, continue
 *
 * Note: If virtual server (directory) doesn't exist, boa returns 404 :-(
 */

int translate_uri(request * req)
{
	char *req_urip;

	/* clean pathname */
	clean_pathname(req->request_uri);

	/* Move anything after ? into req->query_string */

	req_urip = req->request_uri;
	if (req_urip[0] != '/') {
		send_r_not_found(req);
		return 0;
	}

	req_urip = strchr(req_urip, '?');
	if (req_urip != NULL)
	{
		*req_urip++ = '\0';
		req->query_string = strdup(req_urip);
	}

	/* Percent-decode request */
	if (unescape_uri(req->request_uri) == 0) {
		my_syslog(LOG_ERR, "Problem unescaping uri");
		send_r_bad_request(req);
		return 0;
	}

	req->pathname = GetDocumentBasedPath(req->request_uri);

	return 1;
}

