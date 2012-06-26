/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <psp@well.com>
 *  Some changes Copyright (C) 1996 Larry Doolittle <ldoolitt@jlab.org>
 *  Some changes Copyright (C) 1997 Jon Nelson <nels0988@tc.umn.edu>
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

/* boa: hash.c */

#include "boa.h"
#include <syslog.h>

/*
 * There are two hash tables used, each with a key/value pair
 * stored in a hash_struct.  They are:
 *
 * mime_hashtable:
 *     key = file extension
 *   value = mime type
 *
 * passwd_hashtable:
 *     key = username
 *   value = home directory
 *
 */

struct _hash_struct_ {
	char *key;
	char *value;
	struct _hash_struct_ *next;
};

typedef struct _hash_struct_ hash_struct;

static hash_struct *mime_hashtable[MIME_HASHTABLE_SIZE];

#ifdef USE_BROWSERMATCH
static browsermatch_struct *browsermatch_hashtable[BROWSERMATCH_HASHTABLE_SIZE];
#endif

/*
 * Name: add_mime_type
 * Description: Adds a key/value pair to the mime_hashtable
 */

void add_mime_type(char *extension, char *type)
{
	int hash;
	hash_struct *current;

	if (!extension)
		return;

	hash = get_mime_hash_value(extension);

	current = mime_hashtable[hash];

	if (!current) {
		mime_hashtable[hash] = (hash_struct *) malloc(sizeof(hash_struct));
#if 1
		mime_hashtable[hash]->key = extension;
		mime_hashtable[hash]->value = type;
#else
		mime_hashtable[hash]->key = strdup(extension);
		mime_hashtable[hash]->value = strdup(type);
#endif
		mime_hashtable[hash]->next = NULL;
	} else {
		while (current) {
			if (!strcmp(current->key, extension))
			{
				//return;			/* don't add extension twice */
#if 1				
				current->value = type;
#else
				if (current->value != NULL) free(current->value);
				current->value = strdup(type);
#endif
				return;
			}
			if (current->next)
				current = current->next;
			else
				break;
		}

		current->next = (hash_struct *) malloc(sizeof(hash_struct));
		current = current->next;

#if 1
		current->key = extension;
		current->value = type;
#else
		current->key = strdup(extension);
		current->value = strdup(type);
#endif
		current->next = NULL;
	}
}

/*
 * Name: get_mime_hash_value
 *
 * Description: adds the ASCII values of the file extension letters
 * and mods by the hashtable size to get the hash value
 */

int get_mime_hash_value(char *extension)
{
	int hash = 0;
	int index = 0;
	char c;

	while ((c = extension[index++]))
		hash += (int) c;

	hash %= MIME_HASHTABLE_SIZE;

	return hash;
}

/*
 * Name: get_mime_type
 *
 * Description: Returns the mime type for a supplied filename.
 * Returns default type if not found.
 */

char *get_mime_type(char *filename)
{
	char *extension;
	hash_struct *current;

	int hash;

	extension = strrchr(filename, '.');

	if (!extension || *extension++ == '\0')
		return default_type;

	hash = get_mime_hash_value(extension);
	current = mime_hashtable[hash];

	while (current) {
		if (!strcmp(current->key, extension))	/* hit */
			return current->value;
		current = current->next;
	}

	return default_type;
}

void dump_mime(void)
{
	int i;
	hash_struct *temp;
	for (i = 0; i < MIME_HASHTABLE_SIZE; ++i) {		/* these limits OK? */
		if (mime_hashtable[i]) {
			temp = mime_hashtable[i];
			while (temp) {
				hash_struct *temp_next;

				temp_next = temp->next;
#if 0
				free(temp->key);
				free(temp->value);
#endif
				free(temp);

				temp = temp_next;
			}
			mime_hashtable[i] = NULL;
		}
	}
}

