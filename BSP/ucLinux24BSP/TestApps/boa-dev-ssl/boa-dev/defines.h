/* vi:set tabstop=2 cindent shiftwidth=2: */
/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
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

#ifndef _DEFINES_H
#define _DEFINES_H

#ifndef __CYGWIN__
#include <linux/config.h>
#endif

/***** Change this, or use -c on the command line to specify it *****/

#ifndef SERVER_ROOT
#define SERVER_ROOT "/etc/Html"
#endif

/***** Logs *****************************************************************/

/* ...and this for agent log */
/* #define NO_AGENT_LOG     1  */
#define NO_AGENT_LOG 1

/****** Authorization ******************************************************/

#define USE_AUTH								1

/***** Various stuff that you may want to tweak, but probably shouldn't *****/

//#define SERVER_SSL 1
#undef ROOT_AUTH
#define CHECK_IP_MAC 1
//#undef CHECK_IP_MAC

#define SOCKETBUF_SIZE				1024
#define MAX_HEADER_LENGTH			1024
//#define CLIENT_STREAM_SIZE			1024
#define CLIENT_STREAM_SIZE		2048//CXH_MODIFY
#define BUFFER_SIZE				CLIENT_STREAM_SIZE

#define MIME_HASHTABLE_SIZE		47
#define ALIAS_HASHTABLE_SIZE		17
#define PASSWD_HASHTABLE_SIZE		47
#define BROWSERMATCH_HASHTABLE_SIZE	47
#define VIRTUALHOST_HASHTABLE_SIZE 47
#define CODEPAGE_HASHTABLE_SIZE 47
#define AUTH_HASHTABLE_SIZE 47

#define REQUEST_TIMEOUT				60

#define CRASHDEBUG 1

/***** CHANGE ANYTHING BELOW THIS LINE AT YOUR OWN PERIL *****/
/***** You will probably introduce buffer overruns unless you know
       what you are doing *****/
#include <sys/types.h>
#include <dirent.h>				/* for MAXNAMLEN */
#include <sys/socket.h>
#ifndef SO_MAXCONN
#define SO_MAXCONN 250
#endif
#define BYTES_TO_WRITE			81920		/* How much bytes will be send in
																			 process_get(). */

#define MAX_SITENAME_LENGTH			256
#define MAX_LOG_LENGTH				MAX_HEADER_LENGTH + 1024
#define MAX_FILE_LENGTH				NAME_MAX
#define MAX_PATH_LENGTH				PATH_MAX

#ifdef ACCEPT_ON
#define MAX_ACCEPT_LENGTH MAX_HEADER_LENGTH
#else
#define MAX_ACCEPT_LENGTH 0
#endif

#define SERVER_VERSION				"WYM/1.0"

/******************* RESPONSE CLASSES *****************/

#define R_INFORMATIONAL	1
#define R_SUCCESS	2
#define R_REDIRECTION	3
#define R_CLIENT_ERROR	4
#define R_SERVER_ERROR	5

/******************* RESPONSE CODES ******************/

#define R_REQUEST_OK	200
#define R_CREATED	201
#define R_ACCEPTED	202
#define R_PROVISIONAL	203		/* provisional information */
#define R_NO_CONTENT	204

#define R_MULTIPLE	300			/* multiple choices */
#define R_MOVED_PERM	301
#define R_MOVED_TEMP	302
#define R_NOT_MODIFIED	304

#define R_BAD_REQUEST	400
#define R_UNAUTHORIZED	401
#define R_PAYMENT	402			/* payment required */
#define R_FORBIDDEN	403
#define R_NOT_FOUND	404
#define R_METHOD_NA	405			/* method not allowed */
#define R_NONE_ACC	406			/* none acceptable */
#define R_PROXY		407			/* proxy authentication required */
#define R_REQUEST_TO	408		/* request timeout */
#define R_CONFLICT	409
#define R_GONE		410

#define R_ERROR		500			/* internal server error */
#define	R_NOT_IMP	501			/* not implemented */
#define	R_BAD_GATEWAY	502
#define R_SERVICE_UNAV	503		/* service unavailable */
#define	R_GATEWAY_TO	504		/* gateway timeout */
#define R_BAD_VERSION	505

/****************** METHODS *****************/

#define	M_INVALID	-1
#define	M_SHORT	0

#define M_GET		1
#define M_HEAD		2
#define M_PUT		3
#define M_POST		4
#define M_DELETE	5
#define M_LINK		6
#define M_UNLINK	7

/******************* ERRORS *****************/

#define SERVER_ERROR		1
#define OUT_OF_MEMORY		2
#define NO_CREATE_SOCKET	3
#define NO_FCNTL		4
#define NO_SETSOCKOPT		5
#define NO_BIND			6
#define NO_LISTEN		7
#define NO_SETGID		8
#define NO_SETUID		9
#define NO_OPEN_LOG		10
#define SELECT			11
#define GETPWUID		12
#define INITGROUPS		13
#define CANNOT_CHROOT           14

#define SHUTDOWN		15		/* do not change */

/***************** SSL ERRORS ****************/

#define NO_SSL			16

/************** REQUEST STATUS (req->status) ***************/

#define READ_HEADER             0
#define ONE_CR                  1
#define ONE_LF                  2
#define TWO_CR                  3
#define BODY_READ               4
#define BODY_WRITE1              5
#define WRITE                   6
#define CLOSE			9

#define CGI_READ				0
#define CGI_WRITE				1
#define CGI_CLOSE				2	/* used only for CGI_STATUS */

/************** ALIAS TYPES (aliasp->type) ***************/

#define ALIAS				0
#define SCRIPTALIAS	1
#define REDIRECT		2

/************** KEEPALIVE CONSTANTS (req->keepalive) *******/

#define KA_INACTIVE		0
#define KA_STOPPED     	1
#define KA_ACTIVE      	2

#define SQUASH_KA(req)	do {req->keepalive=KA_STOPPED;} while(0)
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

#endif
