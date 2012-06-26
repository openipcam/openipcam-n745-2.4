/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Authorization "module" (c) 1998,99 Martin Hinner <martin@tdp.cz>
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

#include <stdio.h>
#include <fcntl.h>
#ifdef __UC_LIBC__
#include <unistd.h>
#else
#include <crypt.h>
#endif
#include <syslog.h>
#include "md5.h"
#include "boa.h"
#ifdef SHADOW_AUTH
#include <shadow.h>
#endif
#ifdef OLD_CONFIG_PASSWORDS
#include <crypt_old.h>
#endif
#ifdef EMBED
#include <sys/types.h>
#include <pwd.h>
#endif

#ifdef USE_AUTH

struct _auth_dir_ {
	char *directory;
	FILE *authfile;
	int dir_len;
	struct _auth_dir_ *next;
};

typedef struct _auth_dir_ auth_dir;

static auth_dir *auth_hashtable [AUTH_HASHTABLE_SIZE];
char auth_userpass[0x80];
#ifdef OLD_CONFIG_PASSWORDS
char auth_old_password[16];
#endif

/*
 * Name: get_auth_hash_value
 *
 * Description: adds the ASCII values of the file letters
 * and mods by the hashtable size to get the hash value
 */
inline int get_auth_hash_value(char *dir)
{
	unsigned int hash = 0;
	unsigned int index = 0;
	unsigned char c;

	hash = dir[index++];
	while ((c = dir[index++]) && c != '/')
		hash += (unsigned int) c;

	return hash % AUTH_HASHTABLE_SIZE;
}

/*
 * Name: auth_add
 *
 * Description: adds 
 */
void auth_add(char *directory,char *file)
{
	auth_dir *new_a,*old;
	int f;
	int hash;
	
  old = auth_hashtable [get_auth_hash_value(directory)];
	while (old)
	{
		if (!strcmp(directory,old->directory))
			return;
		old = old->next;
	}
	
	new_a = (auth_dir *)malloc(sizeof(auth_dir));
	/* success of this call will be checked later... */
	new_a->authfile = fopen(file,"rt");
	new_a->directory = strdup(directory);
	new_a->dir_len = strlen(directory);
	new_a->next = auth_hashtable [get_auth_hash_value(directory)];
	auth_hashtable [get_auth_hash_value(directory)] = new_a;
}

void auth_check()
{
	int hash;
	auth_dir *cur;
	
	for (hash=0;hash<AUTH_HASHTABLE_SIZE;hash++)
	{
  	cur = auth_hashtable [hash];
	  while (cur)
		{
			if (!cur->authfile)
			{
				log_error_time();
				fprintf(stderr,"Authentication password file for %s not found!\n",
						cur->directory);
			}
			cur = cur->next;
		}
	}
}

/*
 * Name: auth_check_userpass
 *
 * Description: Checks user's password. Returns 0 when sucessful and password
 * 	is ok, else returns nonzero; As one-way function is used RSA's MD5 w/
 *  BASE64 encoding.
#ifdef EMBED
 * On embedded environments we use crypt(), instead of MD5.
#endif
 */
int auth_check_userpass(char *user,char *pass,FILE *authfile)
{
#ifdef SHADOW_AUTH
	struct spwd *sp;

	sp = getspnam(user);
	if (!sp)
		return 1;

	if (!strcmp(crypt(pass, sp->sp_pwdp), sp->sp_pwdp))
		return 0;

#else

#ifndef EMBED
	char temps[0x100],*pwd;
	struct MD5Context mc;
 	unsigned char final[16];
	char encoded_passwd[0x40];
  /* Encode password ('pass') using one-way function and then use base64
		 encoding. */
	MD5Init(&mc);
	MD5Update(&mc, pass, strlen(pass));
	MD5Final(final, &mc);
	strcpy(encoded_passwd,"$1$");
	base64encode(final, encoded_passwd+3, 16);

	DBG(printf("auth_check_userpass(%s,%s,...);\n",user,pass);)

	fseek(authfile,0,SEEK_SET);
	while (fgets(temps,0x100,authfile))
	{
		if (temps[strlen(temps)-1]=='\n')
			temps[strlen(temps)-1] = 0;
		pwd = strchr(temps,':');
		if (pwd)
		{
			*pwd++=0;
			if (!strcmp(temps,user))
			{
				if (!strcmp(pwd,encoded_passwd))
					return 0;
			}
		}
	}
#else
	struct passwd *pwp;

	pwp = getpwnam(user);
	if (pwp != NULL) {
		if (strcmp(crypt(pass, pwp->pw_passwd), pwp->pw_passwd) == 0)
			return 0;
	}
#ifdef OLD_CONFIG_PASSWORDS
	/* For backwards compatibility we allow the global root password to work too */
	if (auth_old_password[0] != '\0') {
//mcli del		if (strcmp(crypt_old(pass,auth_old_password),auth_old_password) == 0 ||
				if(strcmp(crypt(pass,auth_old_password),auth_old_password) == 0) {
			strcpy(user, "root");
			return 0;
		}
	}
#endif	/* OLD_CONFIG_PASSWORDS */
#endif	/* ! EMBED */
#endif	/* SHADOW_AUTH */
	return 1;
}

int auth_authorize(request * req)
{
	auth_dir *current;
  int hash;
	char *pwd;
	static char current_client[20];

  DBG(printf("auth_authorize\n");)
					
	hash = get_auth_hash_value(req->request_uri);
  current = auth_hashtable[hash];
					
  while (current) {
		if (!memcmp(req->request_uri, current->directory,
								current->dir_len)) {
			if (current->directory[current->dir_len - 1] != '/' &&
				        req->request_uri[current->dir_len] != '/' &&
								req->request_uri[current->dir_len] != '\0') {
				break;
			}
			if (req->authorization)
			{
				if (current->authfile==0)
				{
					send_r_error(req);
					return 0;
				}
				if (strncasecmp(req->authorization,"Basic ",6))
				{
					syslog(LOG_ERR, "Can only handle Basic auth\n");
					send_r_bad_request(req);
					return 0;
				}
				
				base64decode(auth_userpass,req->authorization+6,0x100);
				
				if ( (pwd = strchr(auth_userpass,':')) == 0 )
				{
					syslog(LOG_ERR, "No user:pass in Basic auth\n");
					send_r_bad_request(req);
					return 0;
				}
				
				*pwd++=0;

				if (auth_check_userpass(auth_userpass,pwd,current->authfile) )
				{
					syslog(LOG_ERR, "Authentication attempt failed for %s from %s\n", auth_userpass, req->remote_ip_addr);
					bzero(current_client, 20);
					send_r_unauthorized(req,server_name);
					return 0;
				}
				if (strcmp(current_client, req->remote_ip_addr) != 0) {
					strncpy(current_client, req->remote_ip_addr, 20);
					syslog(LOG_INFO, "Authentication successful for %s from %s\n", auth_userpass,  req->remote_ip_addr);
				}
				/* Copy user's name to request structure */
				strncpy(req->user, auth_userpass, 15);
				req->user[15] = '\0';
				return 1;
			}else
			{
				/* No credentials were supplied. Tell them that some are required */
				send_r_unauthorized(req,server_name);
				return 0;
			}
		}
	  current = current->next;
  }
						
	return 1;
}

void dump_auth(void)
{
	int i;
	auth_dir *temp;

	for (i = 0; i < AUTH_HASHTABLE_SIZE; ++i) {
		if (auth_hashtable[i]) {
			temp = auth_hashtable[i];
			while (temp) {
				auth_dir *temp_next;

				if (temp->directory)
					free(temp->directory);
				if (temp->authfile)
					fclose(temp->authfile);
				temp_next = temp->next;
				free(temp);
				temp = temp_next;
			}
			auth_hashtable[i] = NULL;
		}
	}
}

#endif
