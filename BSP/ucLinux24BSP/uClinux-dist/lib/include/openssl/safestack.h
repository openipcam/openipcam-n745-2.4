/* ====================================================================
 * Copyright (c) 1999 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#ifndef HEADER_SAFESTACK_H
#define HEADER_SAFESTACK_H

#include <openssl/stack.h>

#ifdef DEBUG_SAFESTACK

#define STACK_OF(type) struct stack_st_##type
#define PREDECLARE_STACK_OF(type) STACK_OF(type);

#define DECLARE_STACK_OF(type) \
STACK_OF(type) \
    { \
    STACK stack; \
    };

#define IMPLEMENT_STACK_OF(type) /* nada (obsolete in new safestack approach)*/

/* SKM_sk_... stack macros are internal to safestack.h:
 * never use them directly, use sk_<type>_... instead */
#define SKM_sk_new(type, cmp) \
	((STACK_OF(type) * (*)(int (*)(const type * const *, const type * const *)))sk_new)(cmp)
#define SKM_sk_new_null(type) \
	((STACK_OF(type) * (*)(void))sk_new_null)()
#define SKM_sk_free(type, st) \
	((void (*)(STACK_OF(type) *))sk_free)(st)
#define SKM_sk_num(type, st) \
	((int (*)(const STACK_OF(type) *))sk_num)(st)
#define SKM_sk_value(type, st,i) \
	((type * (*)(const STACK_OF(type) *, int))sk_value)(st, i)
#define SKM_sk_set(type, st,i,val) \
	((type * (*)(STACK_OF(type) *, int, type *))sk_set)(st, i, val)
#define SKM_sk_zero(type, st) \
	((void (*)(STACK_OF(type) *))sk_zero)(st)
#define SKM_sk_push(type, st,val) \
	((int (*)(STACK_OF(type) *, type *))sk_push)(st, val)
#define SKM_sk_unshift(type, st,val) \
	((int (*)(STACK_OF(type) *, type *))sk_unshift)(st, val)
#define SKM_sk_find(type, st,val) \
	((int (*)(STACK_OF(type) *, type *))sk_find)(st, val)
#define SKM_sk_delete(type, st,i) \
	((type * (*)(STACK_OF(type) *, int))sk_delete)(st, i)
#define SKM_sk_delete_ptr(type, st,ptr) \
	((type * (*)(STACK_OF(type) *, type *))sk_delete_ptr)(st, ptr)
#define SKM_sk_insert(type, st,val,i) \
	((int (*)(STACK_OF(type) *, type *, int))sk_insert)(st, val, i)
#define SKM_sk_set_cmp_func(type, st,cmp) \
	((int (*(*)(STACK_OF(type) *, int (*)(const type * const *, const type * const *))) \
	  (const type * const *, const type * const *))sk_set_cmp_func)\
	(st, cmp)
#define SKM_sk_dup(type, st) \
	((STACK_OF(type) *(*)(STACK_OF(type) *))sk_dup)(st)
#define SKM_sk_pop_free(type, st,free_func) \
	((void (*)(STACK_OF(type) *, void (*)(type *)))sk_pop_free)\
	(st, free_func)
#define SKM_sk_shift(type, st) \
	((type * (*)(STACK_OF(type) *))sk_shift)(st)
#define SKM_sk_pop(type, st) \
	((type * (*)(STACK_OF(type) *))sk_pop)(st)
#define SKM_sk_sort(type, st) \
	((void (*)(STACK_OF(type) *))sk_sort)(st)

#define	SKM_ASN1_SET_OF_d2i(type, st, pp, length, d2i_func, free_func, ex_tag, ex_class) \
	((STACK_OF(type) * (*) (STACK_OF(type) **,unsigned char **, long , \
                                       type *(*)(type **, unsigned char **,long), \
                                       void (*)(type *), int ,int )) d2i_ASN1_SET) \
						(st,pp,length, d2i_func, free_func, ex_tag,ex_class)
#define	SKM_ASN1_SET_OF_i2d(type, st, pp, i2d_func, ex_tag, ex_class, is_set) \
	((int (*)(STACK_OF(type) *,unsigned char **, \
                           int (*)(type *,unsigned char **), int , int , int)) i2d_ASN1_SET) \
						(st,pp,i2d_func,ex_tag,ex_class,is_set)

#define	SKM_ASN1_seq_pack(type, st, i2d_func, buf, len) \
	((unsigned char *(*)(STACK_OF(type) *, \
                                    int (*)(type *,unsigned char **), unsigned char **,int *)) ASN1_seq_pack) \
				(st, i2d_func, buf, len)
#define	SKM_ASN1_seq_unpack(type, buf, len, d2i_func, free_func) \
	((STACK_OF(type) * (*)(unsigned char *,int, \
                                       type *(*)(type **,unsigned char **, long), \
                                       void (*)(type *)))ASN1_seq_unpack) \
					(buf,len,d2i_func, free_func)

#define SKM_PKCS12_decrypt_d2i(type, algor, d2i_func, free_func, pass, passlen, oct, seq) \
	((STACK_OF(type) * (*)(X509_ALGOR *, \
                                type *(*)(type **, unsigned char **, long), void (*)(type *), \
                                const char *, int, \
                                ASN1_STRING *, int))PKCS12_decrypt_d2i) \
				(algor,d2i_func,free_func,pass,passlen,oct,seq)

#else

#define STACK_OF(type) STACK
#define PREDECLARE_STACK_OF(type) /* nada */
#define DECLARE_STACK_OF(type)    /* nada */
#define IMPLEMENT_STACK_OF(type)  /* nada */

#define SKM_sk_new(type, cmp) \
	sk_new((int (*)(const char * const *, const char * const *))(cmp))
#define SKM_sk_new_null(type) \
	sk_new_null()
#define SKM_sk_free(type, st) \
	sk_free(st)
#define SKM_sk_num(type, st) \
	sk_num(st)
#define SKM_sk_value(type, st,i) \
	((type *)sk_value(st, i))
#define SKM_sk_set(type, st,i,val) \
	((type *)sk_set(st, i,(char *)val))
#define SKM_sk_zero(type, st) \
	sk_zero(st)
#define SKM_sk_push(type, st,val) \
	sk_push(st, (char *)val)
#define SKM_sk_unshift(type, st,val) \
	sk_unshift(st, val)
#define SKM_sk_find(type, st,val) \
	sk_find(st, (char *)val)
#define SKM_sk_delete(type, st,i) \
	((type *)sk_delete(st, i))
#define SKM_sk_delete_ptr(type, st,ptr) \
	((type *)sk_delete_ptr(st,(char *)ptr))
#define SKM_sk_insert(type, st,val,i) \
	sk_insert(st, (char *)val, i)
#define SKM_sk_set_cmp_func(type, st,cmp) \
	((int (*)(const type * const *,const type * const *)) \
	sk_set_cmp_func(st, (int (*)(const char * const *, const char * const *))(cmp)))
#define SKM_sk_dup(type, st) \
	sk_dup(st)
#define SKM_sk_pop_free(type, st,free_func) \
	sk_pop_free(st, (void (*)(void *))free_func)
#define SKM_sk_shift(type, st) \
	((type *)sk_shift(st))
#define SKM_sk_pop(type, st) \
	((type *)sk_pop(st))
#define SKM_sk_sort(type, st) \
	sk_sort(st)

#define	SKM_ASN1_SET_OF_d2i(type, st, pp, length, d2i_func, free_func, ex_tag, ex_class) \
	d2i_ASN1_SET(st,pp,length, (char *(*)())d2i_func, (void (*)(void *))free_func, ex_tag,ex_class)
#define	SKM_ASN1_SET_OF_i2d(type, st, pp, i2d_func, ex_tag, ex_class, is_set) \
	i2d_ASN1_SET(st,pp,i2d_func,ex_tag,ex_class,is_set)

#define	SKM_ASN1_seq_pack(type, st, i2d_func, buf, len) \
	ASN1_seq_pack(st, i2d_func, buf, len)
#define	SKM_ASN1_seq_unpack(type, buf, len, d2i_func, free_func) \
	ASN1_seq_unpack(buf,len,(char *(*)())d2i_func, (void(*)(void *))free_func)

#define SKM_PKCS12_decrypt_d2i(type, algor, d2i_func, free_func, pass, passlen, oct, seq) \
	((STACK *)PKCS12_decrypt_d2i(algor,(char *(*)())d2i_func, (void(*)(void *))free_func,pass,passlen,oct,seq))

#endif

/* This block of defines is updated by util/mkstack.pl, please do not touch! */
#define sk_ACCESS_DESCRIPTION_new(st) SKM_sk_new(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_new_null() SKM_sk_new_null(ACCESS_DESCRIPTION)
#define sk_ACCESS_DESCRIPTION_free(st) SKM_sk_free(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_num(st) SKM_sk_num(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_value(st, i) SKM_sk_value(ACCESS_DESCRIPTION, (st), (i))
#define sk_ACCESS_DESCRIPTION_set(st, i, val) SKM_sk_set(ACCESS_DESCRIPTION, (st), (i), (val))
#define sk_ACCESS_DESCRIPTION_zero(st) SKM_sk_zero(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_push(st, val) SKM_sk_push(ACCESS_DESCRIPTION, (st), (val))
#define sk_ACCESS_DESCRIPTION_unshift(st, val) SKM_sk_unshift(ACCESS_DESCRIPTION, (st), (val))
#define sk_ACCESS_DESCRIPTION_find(st, val) SKM_sk_find(ACCESS_DESCRIPTION, (st), (val))
#define sk_ACCESS_DESCRIPTION_delete(st, i) SKM_sk_delete(ACCESS_DESCRIPTION, (st), (i))
#define sk_ACCESS_DESCRIPTION_delete_ptr(st, ptr) SKM_sk_delete_ptr(ACCESS_DESCRIPTION, (st), (ptr))
#define sk_ACCESS_DESCRIPTION_insert(st, val, i) SKM_sk_insert(ACCESS_DESCRIPTION, (st), (val), (i))
#define sk_ACCESS_DESCRIPTION_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ACCESS_DESCRIPTION, (st), (cmp))
#define sk_ACCESS_DESCRIPTION_dup(st) SKM_sk_dup(ACCESS_DESCRIPTION, st)
#define sk_ACCESS_DESCRIPTION_pop_free(st, free_func) SKM_sk_pop_free(ACCESS_DESCRIPTION, (st), (free_func))
#define sk_ACCESS_DESCRIPTION_shift(st) SKM_sk_shift(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_pop(st) SKM_sk_pop(ACCESS_DESCRIPTION, (st))
#define sk_ACCESS_DESCRIPTION_sort(st) SKM_sk_sort(ACCESS_DESCRIPTION, (st))

#define sk_ASN1_GENERALSTRING_new(st) SKM_sk_new(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_new_null() SKM_sk_new_null(ASN1_GENERALSTRING)
#define sk_ASN1_GENERALSTRING_free(st) SKM_sk_free(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_num(st) SKM_sk_num(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_value(st, i) SKM_sk_value(ASN1_GENERALSTRING, (st), (i))
#define sk_ASN1_GENERALSTRING_set(st, i, val) SKM_sk_set(ASN1_GENERALSTRING, (st), (i), (val))
#define sk_ASN1_GENERALSTRING_zero(st) SKM_sk_zero(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_push(st, val) SKM_sk_push(ASN1_GENERALSTRING, (st), (val))
#define sk_ASN1_GENERALSTRING_unshift(st, val) SKM_sk_unshift(ASN1_GENERALSTRING, (st), (val))
#define sk_ASN1_GENERALSTRING_find(st, val) SKM_sk_find(ASN1_GENERALSTRING, (st), (val))
#define sk_ASN1_GENERALSTRING_delete(st, i) SKM_sk_delete(ASN1_GENERALSTRING, (st), (i))
#define sk_ASN1_GENERALSTRING_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_GENERALSTRING, (st), (ptr))
#define sk_ASN1_GENERALSTRING_insert(st, val, i) SKM_sk_insert(ASN1_GENERALSTRING, (st), (val), (i))
#define sk_ASN1_GENERALSTRING_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_GENERALSTRING, (st), (cmp))
#define sk_ASN1_GENERALSTRING_dup(st) SKM_sk_dup(ASN1_GENERALSTRING, st)
#define sk_ASN1_GENERALSTRING_pop_free(st, free_func) SKM_sk_pop_free(ASN1_GENERALSTRING, (st), (free_func))
#define sk_ASN1_GENERALSTRING_shift(st) SKM_sk_shift(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_pop(st) SKM_sk_pop(ASN1_GENERALSTRING, (st))
#define sk_ASN1_GENERALSTRING_sort(st) SKM_sk_sort(ASN1_GENERALSTRING, (st))

#define sk_ASN1_INTEGER_new(st) SKM_sk_new(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_new_null() SKM_sk_new_null(ASN1_INTEGER)
#define sk_ASN1_INTEGER_free(st) SKM_sk_free(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_num(st) SKM_sk_num(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_value(st, i) SKM_sk_value(ASN1_INTEGER, (st), (i))
#define sk_ASN1_INTEGER_set(st, i, val) SKM_sk_set(ASN1_INTEGER, (st), (i), (val))
#define sk_ASN1_INTEGER_zero(st) SKM_sk_zero(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_push(st, val) SKM_sk_push(ASN1_INTEGER, (st), (val))
#define sk_ASN1_INTEGER_unshift(st, val) SKM_sk_unshift(ASN1_INTEGER, (st), (val))
#define sk_ASN1_INTEGER_find(st, val) SKM_sk_find(ASN1_INTEGER, (st), (val))
#define sk_ASN1_INTEGER_delete(st, i) SKM_sk_delete(ASN1_INTEGER, (st), (i))
#define sk_ASN1_INTEGER_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_INTEGER, (st), (ptr))
#define sk_ASN1_INTEGER_insert(st, val, i) SKM_sk_insert(ASN1_INTEGER, (st), (val), (i))
#define sk_ASN1_INTEGER_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_INTEGER, (st), (cmp))
#define sk_ASN1_INTEGER_dup(st) SKM_sk_dup(ASN1_INTEGER, st)
#define sk_ASN1_INTEGER_pop_free(st, free_func) SKM_sk_pop_free(ASN1_INTEGER, (st), (free_func))
#define sk_ASN1_INTEGER_shift(st) SKM_sk_shift(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_pop(st) SKM_sk_pop(ASN1_INTEGER, (st))
#define sk_ASN1_INTEGER_sort(st) SKM_sk_sort(ASN1_INTEGER, (st))

#define sk_ASN1_OBJECT_new(st) SKM_sk_new(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_new_null() SKM_sk_new_null(ASN1_OBJECT)
#define sk_ASN1_OBJECT_free(st) SKM_sk_free(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_num(st) SKM_sk_num(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_value(st, i) SKM_sk_value(ASN1_OBJECT, (st), (i))
#define sk_ASN1_OBJECT_set(st, i, val) SKM_sk_set(ASN1_OBJECT, (st), (i), (val))
#define sk_ASN1_OBJECT_zero(st) SKM_sk_zero(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_push(st, val) SKM_sk_push(ASN1_OBJECT, (st), (val))
#define sk_ASN1_OBJECT_unshift(st, val) SKM_sk_unshift(ASN1_OBJECT, (st), (val))
#define sk_ASN1_OBJECT_find(st, val) SKM_sk_find(ASN1_OBJECT, (st), (val))
#define sk_ASN1_OBJECT_delete(st, i) SKM_sk_delete(ASN1_OBJECT, (st), (i))
#define sk_ASN1_OBJECT_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_OBJECT, (st), (ptr))
#define sk_ASN1_OBJECT_insert(st, val, i) SKM_sk_insert(ASN1_OBJECT, (st), (val), (i))
#define sk_ASN1_OBJECT_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_OBJECT, (st), (cmp))
#define sk_ASN1_OBJECT_dup(st) SKM_sk_dup(ASN1_OBJECT, st)
#define sk_ASN1_OBJECT_pop_free(st, free_func) SKM_sk_pop_free(ASN1_OBJECT, (st), (free_func))
#define sk_ASN1_OBJECT_shift(st) SKM_sk_shift(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_pop(st) SKM_sk_pop(ASN1_OBJECT, (st))
#define sk_ASN1_OBJECT_sort(st) SKM_sk_sort(ASN1_OBJECT, (st))

#define sk_ASN1_STRING_TABLE_new(st) SKM_sk_new(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_new_null() SKM_sk_new_null(ASN1_STRING_TABLE)
#define sk_ASN1_STRING_TABLE_free(st) SKM_sk_free(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_num(st) SKM_sk_num(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_value(st, i) SKM_sk_value(ASN1_STRING_TABLE, (st), (i))
#define sk_ASN1_STRING_TABLE_set(st, i, val) SKM_sk_set(ASN1_STRING_TABLE, (st), (i), (val))
#define sk_ASN1_STRING_TABLE_zero(st) SKM_sk_zero(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_push(st, val) SKM_sk_push(ASN1_STRING_TABLE, (st), (val))
#define sk_ASN1_STRING_TABLE_unshift(st, val) SKM_sk_unshift(ASN1_STRING_TABLE, (st), (val))
#define sk_ASN1_STRING_TABLE_find(st, val) SKM_sk_find(ASN1_STRING_TABLE, (st), (val))
#define sk_ASN1_STRING_TABLE_delete(st, i) SKM_sk_delete(ASN1_STRING_TABLE, (st), (i))
#define sk_ASN1_STRING_TABLE_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_STRING_TABLE, (st), (ptr))
#define sk_ASN1_STRING_TABLE_insert(st, val, i) SKM_sk_insert(ASN1_STRING_TABLE, (st), (val), (i))
#define sk_ASN1_STRING_TABLE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_STRING_TABLE, (st), (cmp))
#define sk_ASN1_STRING_TABLE_dup(st) SKM_sk_dup(ASN1_STRING_TABLE, st)
#define sk_ASN1_STRING_TABLE_pop_free(st, free_func) SKM_sk_pop_free(ASN1_STRING_TABLE, (st), (free_func))
#define sk_ASN1_STRING_TABLE_shift(st) SKM_sk_shift(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_pop(st) SKM_sk_pop(ASN1_STRING_TABLE, (st))
#define sk_ASN1_STRING_TABLE_sort(st) SKM_sk_sort(ASN1_STRING_TABLE, (st))

#define sk_ASN1_TYPE_new(st) SKM_sk_new(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_new_null() SKM_sk_new_null(ASN1_TYPE)
#define sk_ASN1_TYPE_free(st) SKM_sk_free(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_num(st) SKM_sk_num(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_value(st, i) SKM_sk_value(ASN1_TYPE, (st), (i))
#define sk_ASN1_TYPE_set(st, i, val) SKM_sk_set(ASN1_TYPE, (st), (i), (val))
#define sk_ASN1_TYPE_zero(st) SKM_sk_zero(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_push(st, val) SKM_sk_push(ASN1_TYPE, (st), (val))
#define sk_ASN1_TYPE_unshift(st, val) SKM_sk_unshift(ASN1_TYPE, (st), (val))
#define sk_ASN1_TYPE_find(st, val) SKM_sk_find(ASN1_TYPE, (st), (val))
#define sk_ASN1_TYPE_delete(st, i) SKM_sk_delete(ASN1_TYPE, (st), (i))
#define sk_ASN1_TYPE_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_TYPE, (st), (ptr))
#define sk_ASN1_TYPE_insert(st, val, i) SKM_sk_insert(ASN1_TYPE, (st), (val), (i))
#define sk_ASN1_TYPE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_TYPE, (st), (cmp))
#define sk_ASN1_TYPE_dup(st) SKM_sk_dup(ASN1_TYPE, st)
#define sk_ASN1_TYPE_pop_free(st, free_func) SKM_sk_pop_free(ASN1_TYPE, (st), (free_func))
#define sk_ASN1_TYPE_shift(st) SKM_sk_shift(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_pop(st) SKM_sk_pop(ASN1_TYPE, (st))
#define sk_ASN1_TYPE_sort(st) SKM_sk_sort(ASN1_TYPE, (st))

#define sk_ASN1_VALUE_new(st) SKM_sk_new(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_new_null() SKM_sk_new_null(ASN1_VALUE)
#define sk_ASN1_VALUE_free(st) SKM_sk_free(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_num(st) SKM_sk_num(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_value(st, i) SKM_sk_value(ASN1_VALUE, (st), (i))
#define sk_ASN1_VALUE_set(st, i, val) SKM_sk_set(ASN1_VALUE, (st), (i), (val))
#define sk_ASN1_VALUE_zero(st) SKM_sk_zero(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_push(st, val) SKM_sk_push(ASN1_VALUE, (st), (val))
#define sk_ASN1_VALUE_unshift(st, val) SKM_sk_unshift(ASN1_VALUE, (st), (val))
#define sk_ASN1_VALUE_find(st, val) SKM_sk_find(ASN1_VALUE, (st), (val))
#define sk_ASN1_VALUE_delete(st, i) SKM_sk_delete(ASN1_VALUE, (st), (i))
#define sk_ASN1_VALUE_delete_ptr(st, ptr) SKM_sk_delete_ptr(ASN1_VALUE, (st), (ptr))
#define sk_ASN1_VALUE_insert(st, val, i) SKM_sk_insert(ASN1_VALUE, (st), (val), (i))
#define sk_ASN1_VALUE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ASN1_VALUE, (st), (cmp))
#define sk_ASN1_VALUE_dup(st) SKM_sk_dup(ASN1_VALUE, st)
#define sk_ASN1_VALUE_pop_free(st, free_func) SKM_sk_pop_free(ASN1_VALUE, (st), (free_func))
#define sk_ASN1_VALUE_shift(st) SKM_sk_shift(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_pop(st) SKM_sk_pop(ASN1_VALUE, (st))
#define sk_ASN1_VALUE_sort(st) SKM_sk_sort(ASN1_VALUE, (st))

#define sk_BIO_new(st) SKM_sk_new(BIO, (st))
#define sk_BIO_new_null() SKM_sk_new_null(BIO)
#define sk_BIO_free(st) SKM_sk_free(BIO, (st))
#define sk_BIO_num(st) SKM_sk_num(BIO, (st))
#define sk_BIO_value(st, i) SKM_sk_value(BIO, (st), (i))
#define sk_BIO_set(st, i, val) SKM_sk_set(BIO, (st), (i), (val))
#define sk_BIO_zero(st) SKM_sk_zero(BIO, (st))
#define sk_BIO_push(st, val) SKM_sk_push(BIO, (st), (val))
#define sk_BIO_unshift(st, val) SKM_sk_unshift(BIO, (st), (val))
#define sk_BIO_find(st, val) SKM_sk_find(BIO, (st), (val))
#define sk_BIO_delete(st, i) SKM_sk_delete(BIO, (st), (i))
#define sk_BIO_delete_ptr(st, ptr) SKM_sk_delete_ptr(BIO, (st), (ptr))
#define sk_BIO_insert(st, val, i) SKM_sk_insert(BIO, (st), (val), (i))
#define sk_BIO_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(BIO, (st), (cmp))
#define sk_BIO_dup(st) SKM_sk_dup(BIO, st)
#define sk_BIO_pop_free(st, free_func) SKM_sk_pop_free(BIO, (st), (free_func))
#define sk_BIO_shift(st) SKM_sk_shift(BIO, (st))
#define sk_BIO_pop(st) SKM_sk_pop(BIO, (st))
#define sk_BIO_sort(st) SKM_sk_sort(BIO, (st))

#define sk_CONF_IMODULE_new(st) SKM_sk_new(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_new_null() SKM_sk_new_null(CONF_IMODULE)
#define sk_CONF_IMODULE_free(st) SKM_sk_free(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_num(st) SKM_sk_num(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_value(st, i) SKM_sk_value(CONF_IMODULE, (st), (i))
#define sk_CONF_IMODULE_set(st, i, val) SKM_sk_set(CONF_IMODULE, (st), (i), (val))
#define sk_CONF_IMODULE_zero(st) SKM_sk_zero(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_push(st, val) SKM_sk_push(CONF_IMODULE, (st), (val))
#define sk_CONF_IMODULE_unshift(st, val) SKM_sk_unshift(CONF_IMODULE, (st), (val))
#define sk_CONF_IMODULE_find(st, val) SKM_sk_find(CONF_IMODULE, (st), (val))
#define sk_CONF_IMODULE_delete(st, i) SKM_sk_delete(CONF_IMODULE, (st), (i))
#define sk_CONF_IMODULE_delete_ptr(st, ptr) SKM_sk_delete_ptr(CONF_IMODULE, (st), (ptr))
#define sk_CONF_IMODULE_insert(st, val, i) SKM_sk_insert(CONF_IMODULE, (st), (val), (i))
#define sk_CONF_IMODULE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(CONF_IMODULE, (st), (cmp))
#define sk_CONF_IMODULE_dup(st) SKM_sk_dup(CONF_IMODULE, st)
#define sk_CONF_IMODULE_pop_free(st, free_func) SKM_sk_pop_free(CONF_IMODULE, (st), (free_func))
#define sk_CONF_IMODULE_shift(st) SKM_sk_shift(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_pop(st) SKM_sk_pop(CONF_IMODULE, (st))
#define sk_CONF_IMODULE_sort(st) SKM_sk_sort(CONF_IMODULE, (st))

#define sk_CONF_MODULE_new(st) SKM_sk_new(CONF_MODULE, (st))
#define sk_CONF_MODULE_new_null() SKM_sk_new_null(CONF_MODULE)
#define sk_CONF_MODULE_free(st) SKM_sk_free(CONF_MODULE, (st))
#define sk_CONF_MODULE_num(st) SKM_sk_num(CONF_MODULE, (st))
#define sk_CONF_MODULE_value(st, i) SKM_sk_value(CONF_MODULE, (st), (i))
#define sk_CONF_MODULE_set(st, i, val) SKM_sk_set(CONF_MODULE, (st), (i), (val))
#define sk_CONF_MODULE_zero(st) SKM_sk_zero(CONF_MODULE, (st))
#define sk_CONF_MODULE_push(st, val) SKM_sk_push(CONF_MODULE, (st), (val))
#define sk_CONF_MODULE_unshift(st, val) SKM_sk_unshift(CONF_MODULE, (st), (val))
#define sk_CONF_MODULE_find(st, val) SKM_sk_find(CONF_MODULE, (st), (val))
#define sk_CONF_MODULE_delete(st, i) SKM_sk_delete(CONF_MODULE, (st), (i))
#define sk_CONF_MODULE_delete_ptr(st, ptr) SKM_sk_delete_ptr(CONF_MODULE, (st), (ptr))
#define sk_CONF_MODULE_insert(st, val, i) SKM_sk_insert(CONF_MODULE, (st), (val), (i))
#define sk_CONF_MODULE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(CONF_MODULE, (st), (cmp))
#define sk_CONF_MODULE_dup(st) SKM_sk_dup(CONF_MODULE, st)
#define sk_CONF_MODULE_pop_free(st, free_func) SKM_sk_pop_free(CONF_MODULE, (st), (free_func))
#define sk_CONF_MODULE_shift(st) SKM_sk_shift(CONF_MODULE, (st))
#define sk_CONF_MODULE_pop(st) SKM_sk_pop(CONF_MODULE, (st))
#define sk_CONF_MODULE_sort(st) SKM_sk_sort(CONF_MODULE, (st))

#define sk_CONF_VALUE_new(st) SKM_sk_new(CONF_VALUE, (st))
#define sk_CONF_VALUE_new_null() SKM_sk_new_null(CONF_VALUE)
#define sk_CONF_VALUE_free(st) SKM_sk_free(CONF_VALUE, (st))
#define sk_CONF_VALUE_num(st) SKM_sk_num(CONF_VALUE, (st))
#define sk_CONF_VALUE_value(st, i) SKM_sk_value(CONF_VALUE, (st), (i))
#define sk_CONF_VALUE_set(st, i, val) SKM_sk_set(CONF_VALUE, (st), (i), (val))
#define sk_CONF_VALUE_zero(st) SKM_sk_zero(CONF_VALUE, (st))
#define sk_CONF_VALUE_push(st, val) SKM_sk_push(CONF_VALUE, (st), (val))
#define sk_CONF_VALUE_unshift(st, val) SKM_sk_unshift(CONF_VALUE, (st), (val))
#define sk_CONF_VALUE_find(st, val) SKM_sk_find(CONF_VALUE, (st), (val))
#define sk_CONF_VALUE_delete(st, i) SKM_sk_delete(CONF_VALUE, (st), (i))
#define sk_CONF_VALUE_delete_ptr(st, ptr) SKM_sk_delete_ptr(CONF_VALUE, (st), (ptr))
#define sk_CONF_VALUE_insert(st, val, i) SKM_sk_insert(CONF_VALUE, (st), (val), (i))
#define sk_CONF_VALUE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(CONF_VALUE, (st), (cmp))
#define sk_CONF_VALUE_dup(st) SKM_sk_dup(CONF_VALUE, st)
#define sk_CONF_VALUE_pop_free(st, free_func) SKM_sk_pop_free(CONF_VALUE, (st), (free_func))
#define sk_CONF_VALUE_shift(st) SKM_sk_shift(CONF_VALUE, (st))
#define sk_CONF_VALUE_pop(st) SKM_sk_pop(CONF_VALUE, (st))
#define sk_CONF_VALUE_sort(st) SKM_sk_sort(CONF_VALUE, (st))

#define sk_CRYPTO_EX_DATA_FUNCS_new(st) SKM_sk_new(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_new_null() SKM_sk_new_null(CRYPTO_EX_DATA_FUNCS)
#define sk_CRYPTO_EX_DATA_FUNCS_free(st) SKM_sk_free(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_num(st) SKM_sk_num(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_value(st, i) SKM_sk_value(CRYPTO_EX_DATA_FUNCS, (st), (i))
#define sk_CRYPTO_EX_DATA_FUNCS_set(st, i, val) SKM_sk_set(CRYPTO_EX_DATA_FUNCS, (st), (i), (val))
#define sk_CRYPTO_EX_DATA_FUNCS_zero(st) SKM_sk_zero(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_push(st, val) SKM_sk_push(CRYPTO_EX_DATA_FUNCS, (st), (val))
#define sk_CRYPTO_EX_DATA_FUNCS_unshift(st, val) SKM_sk_unshift(CRYPTO_EX_DATA_FUNCS, (st), (val))
#define sk_CRYPTO_EX_DATA_FUNCS_find(st, val) SKM_sk_find(CRYPTO_EX_DATA_FUNCS, (st), (val))
#define sk_CRYPTO_EX_DATA_FUNCS_delete(st, i) SKM_sk_delete(CRYPTO_EX_DATA_FUNCS, (st), (i))
#define sk_CRYPTO_EX_DATA_FUNCS_delete_ptr(st, ptr) SKM_sk_delete_ptr(CRYPTO_EX_DATA_FUNCS, (st), (ptr))
#define sk_CRYPTO_EX_DATA_FUNCS_insert(st, val, i) SKM_sk_insert(CRYPTO_EX_DATA_FUNCS, (st), (val), (i))
#define sk_CRYPTO_EX_DATA_FUNCS_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(CRYPTO_EX_DATA_FUNCS, (st), (cmp))
#define sk_CRYPTO_EX_DATA_FUNCS_dup(st) SKM_sk_dup(CRYPTO_EX_DATA_FUNCS, st)
#define sk_CRYPTO_EX_DATA_FUNCS_pop_free(st, free_func) SKM_sk_pop_free(CRYPTO_EX_DATA_FUNCS, (st), (free_func))
#define sk_CRYPTO_EX_DATA_FUNCS_shift(st) SKM_sk_shift(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_pop(st) SKM_sk_pop(CRYPTO_EX_DATA_FUNCS, (st))
#define sk_CRYPTO_EX_DATA_FUNCS_sort(st) SKM_sk_sort(CRYPTO_EX_DATA_FUNCS, (st))

#define sk_CRYPTO_dynlock_new(st) SKM_sk_new(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_new_null() SKM_sk_new_null(CRYPTO_dynlock)
#define sk_CRYPTO_dynlock_free(st) SKM_sk_free(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_num(st) SKM_sk_num(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_value(st, i) SKM_sk_value(CRYPTO_dynlock, (st), (i))
#define sk_CRYPTO_dynlock_set(st, i, val) SKM_sk_set(CRYPTO_dynlock, (st), (i), (val))
#define sk_CRYPTO_dynlock_zero(st) SKM_sk_zero(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_push(st, val) SKM_sk_push(CRYPTO_dynlock, (st), (val))
#define sk_CRYPTO_dynlock_unshift(st, val) SKM_sk_unshift(CRYPTO_dynlock, (st), (val))
#define sk_CRYPTO_dynlock_find(st, val) SKM_sk_find(CRYPTO_dynlock, (st), (val))
#define sk_CRYPTO_dynlock_delete(st, i) SKM_sk_delete(CRYPTO_dynlock, (st), (i))
#define sk_CRYPTO_dynlock_delete_ptr(st, ptr) SKM_sk_delete_ptr(CRYPTO_dynlock, (st), (ptr))
#define sk_CRYPTO_dynlock_insert(st, val, i) SKM_sk_insert(CRYPTO_dynlock, (st), (val), (i))
#define sk_CRYPTO_dynlock_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(CRYPTO_dynlock, (st), (cmp))
#define sk_CRYPTO_dynlock_dup(st) SKM_sk_dup(CRYPTO_dynlock, st)
#define sk_CRYPTO_dynlock_pop_free(st, free_func) SKM_sk_pop_free(CRYPTO_dynlock, (st), (free_func))
#define sk_CRYPTO_dynlock_shift(st) SKM_sk_shift(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_pop(st) SKM_sk_pop(CRYPTO_dynlock, (st))
#define sk_CRYPTO_dynlock_sort(st) SKM_sk_sort(CRYPTO_dynlock, (st))

#define sk_DIST_POINT_new(st) SKM_sk_new(DIST_POINT, (st))
#define sk_DIST_POINT_new_null() SKM_sk_new_null(DIST_POINT)
#define sk_DIST_POINT_free(st) SKM_sk_free(DIST_POINT, (st))
#define sk_DIST_POINT_num(st) SKM_sk_num(DIST_POINT, (st))
#define sk_DIST_POINT_value(st, i) SKM_sk_value(DIST_POINT, (st), (i))
#define sk_DIST_POINT_set(st, i, val) SKM_sk_set(DIST_POINT, (st), (i), (val))
#define sk_DIST_POINT_zero(st) SKM_sk_zero(DIST_POINT, (st))
#define sk_DIST_POINT_push(st, val) SKM_sk_push(DIST_POINT, (st), (val))
#define sk_DIST_POINT_unshift(st, val) SKM_sk_unshift(DIST_POINT, (st), (val))
#define sk_DIST_POINT_find(st, val) SKM_sk_find(DIST_POINT, (st), (val))
#define sk_DIST_POINT_delete(st, i) SKM_sk_delete(DIST_POINT, (st), (i))
#define sk_DIST_POINT_delete_ptr(st, ptr) SKM_sk_delete_ptr(DIST_POINT, (st), (ptr))
#define sk_DIST_POINT_insert(st, val, i) SKM_sk_insert(DIST_POINT, (st), (val), (i))
#define sk_DIST_POINT_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(DIST_POINT, (st), (cmp))
#define sk_DIST_POINT_dup(st) SKM_sk_dup(DIST_POINT, st)
#define sk_DIST_POINT_pop_free(st, free_func) SKM_sk_pop_free(DIST_POINT, (st), (free_func))
#define sk_DIST_POINT_shift(st) SKM_sk_shift(DIST_POINT, (st))
#define sk_DIST_POINT_pop(st) SKM_sk_pop(DIST_POINT, (st))
#define sk_DIST_POINT_sort(st) SKM_sk_sort(DIST_POINT, (st))

#define sk_ENGINE_new(st) SKM_sk_new(ENGINE, (st))
#define sk_ENGINE_new_null() SKM_sk_new_null(ENGINE)
#define sk_ENGINE_free(st) SKM_sk_free(ENGINE, (st))
#define sk_ENGINE_num(st) SKM_sk_num(ENGINE, (st))
#define sk_ENGINE_value(st, i) SKM_sk_value(ENGINE, (st), (i))
#define sk_ENGINE_set(st, i, val) SKM_sk_set(ENGINE, (st), (i), (val))
#define sk_ENGINE_zero(st) SKM_sk_zero(ENGINE, (st))
#define sk_ENGINE_push(st, val) SKM_sk_push(ENGINE, (st), (val))
#define sk_ENGINE_unshift(st, val) SKM_sk_unshift(ENGINE, (st), (val))
#define sk_ENGINE_find(st, val) SKM_sk_find(ENGINE, (st), (val))
#define sk_ENGINE_delete(st, i) SKM_sk_delete(ENGINE, (st), (i))
#define sk_ENGINE_delete_ptr(st, ptr) SKM_sk_delete_ptr(ENGINE, (st), (ptr))
#define sk_ENGINE_insert(st, val, i) SKM_sk_insert(ENGINE, (st), (val), (i))
#define sk_ENGINE_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ENGINE, (st), (cmp))
#define sk_ENGINE_dup(st) SKM_sk_dup(ENGINE, st)
#define sk_ENGINE_pop_free(st, free_func) SKM_sk_pop_free(ENGINE, (st), (free_func))
#define sk_ENGINE_shift(st) SKM_sk_shift(ENGINE, (st))
#define sk_ENGINE_pop(st) SKM_sk_pop(ENGINE, (st))
#define sk_ENGINE_sort(st) SKM_sk_sort(ENGINE, (st))

#define sk_ENGINE_CLEANUP_ITEM_new(st) SKM_sk_new(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_new_null() SKM_sk_new_null(ENGINE_CLEANUP_ITEM)
#define sk_ENGINE_CLEANUP_ITEM_free(st) SKM_sk_free(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_num(st) SKM_sk_num(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_value(st, i) SKM_sk_value(ENGINE_CLEANUP_ITEM, (st), (i))
#define sk_ENGINE_CLEANUP_ITEM_set(st, i, val) SKM_sk_set(ENGINE_CLEANUP_ITEM, (st), (i), (val))
#define sk_ENGINE_CLEANUP_ITEM_zero(st) SKM_sk_zero(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_push(st, val) SKM_sk_push(ENGINE_CLEANUP_ITEM, (st), (val))
#define sk_ENGINE_CLEANUP_ITEM_unshift(st, val) SKM_sk_unshift(ENGINE_CLEANUP_ITEM, (st), (val))
#define sk_ENGINE_CLEANUP_ITEM_find(st, val) SKM_sk_find(ENGINE_CLEANUP_ITEM, (st), (val))
#define sk_ENGINE_CLEANUP_ITEM_delete(st, i) SKM_sk_delete(ENGINE_CLEANUP_ITEM, (st), (i))
#define sk_ENGINE_CLEANUP_ITEM_delete_ptr(st, ptr) SKM_sk_delete_ptr(ENGINE_CLEANUP_ITEM, (st), (ptr))
#define sk_ENGINE_CLEANUP_ITEM_insert(st, val, i) SKM_sk_insert(ENGINE_CLEANUP_ITEM, (st), (val), (i))
#define sk_ENGINE_CLEANUP_ITEM_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(ENGINE_CLEANUP_ITEM, (st), (cmp))
#define sk_ENGINE_CLEANUP_ITEM_dup(st) SKM_sk_dup(ENGINE_CLEANUP_ITEM, st)
#define sk_ENGINE_CLEANUP_ITEM_pop_free(st, free_func) SKM_sk_pop_free(ENGINE_CLEANUP_ITEM, (st), (free_func))
#define sk_ENGINE_CLEANUP_ITEM_shift(st) SKM_sk_shift(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_pop(st) SKM_sk_pop(ENGINE_CLEANUP_ITEM, (st))
#define sk_ENGINE_CLEANUP_ITEM_sort(st) SKM_sk_sort(ENGINE_CLEANUP_ITEM, (st))

#define sk_GENERAL_NAME_new(st) SKM_sk_new(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_new_null() SKM_sk_new_null(GENERAL_NAME)
#define sk_GENERAL_NAME_free(st) SKM_sk_free(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_num(st) SKM_sk_num(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_value(st, i) SKM_sk_value(GENERAL_NAME, (st), (i))
#define sk_GENERAL_NAME_set(st, i, val) SKM_sk_set(GENERAL_NAME, (st), (i), (val))
#define sk_GENERAL_NAME_zero(st) SKM_sk_zero(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_push(st, val) SKM_sk_push(GENERAL_NAME, (st), (val))
#define sk_GENERAL_NAME_unshift(st, val) SKM_sk_unshift(GENERAL_NAME, (st), (val))
#define sk_GENERAL_NAME_find(st, val) SKM_sk_find(GENERAL_NAME, (st), (val))
#define sk_GENERAL_NAME_delete(st, i) SKM_sk_delete(GENERAL_NAME, (st), (i))
#define sk_GENERAL_NAME_delete_ptr(st, ptr) SKM_sk_delete_ptr(GENERAL_NAME, (st), (ptr))
#define sk_GENERAL_NAME_insert(st, val, i) SKM_sk_insert(GENERAL_NAME, (st), (val), (i))
#define sk_GENERAL_NAME_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(GENERAL_NAME, (st), (cmp))
#define sk_GENERAL_NAME_dup(st) SKM_sk_dup(GENERAL_NAME, st)
#define sk_GENERAL_NAME_pop_free(st, free_func) SKM_sk_pop_free(GENERAL_NAME, (st), (free_func))
#define sk_GENERAL_NAME_shift(st) SKM_sk_shift(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_pop(st) SKM_sk_pop(GENERAL_NAME, (st))
#define sk_GENERAL_NAME_sort(st) SKM_sk_sort(GENERAL_NAME, (st))

#define sk_KRB5_APREQBODY_new(st) SKM_sk_new(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_new_null() SKM_sk_new_null(KRB5_APREQBODY)
#define sk_KRB5_APREQBODY_free(st) SKM_sk_free(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_num(st) SKM_sk_num(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_value(st, i) SKM_sk_value(KRB5_APREQBODY, (st), (i))
#define sk_KRB5_APREQBODY_set(st, i, val) SKM_sk_set(KRB5_APREQBODY, (st), (i), (val))
#define sk_KRB5_APREQBODY_zero(st) SKM_sk_zero(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_push(st, val) SKM_sk_push(KRB5_APREQBODY, (st), (val))
#define sk_KRB5_APREQBODY_unshift(st, val) SKM_sk_unshift(KRB5_APREQBODY, (st), (val))
#define sk_KRB5_APREQBODY_find(st, val) SKM_sk_find(KRB5_APREQBODY, (st), (val))
#define sk_KRB5_APREQBODY_delete(st, i) SKM_sk_delete(KRB5_APREQBODY, (st), (i))
#define sk_KRB5_APREQBODY_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_APREQBODY, (st), (ptr))
#define sk_KRB5_APREQBODY_insert(st, val, i) SKM_sk_insert(KRB5_APREQBODY, (st), (val), (i))
#define sk_KRB5_APREQBODY_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_APREQBODY, (st), (cmp))
#define sk_KRB5_APREQBODY_dup(st) SKM_sk_dup(KRB5_APREQBODY, st)
#define sk_KRB5_APREQBODY_pop_free(st, free_func) SKM_sk_pop_free(KRB5_APREQBODY, (st), (free_func))
#define sk_KRB5_APREQBODY_shift(st) SKM_sk_shift(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_pop(st) SKM_sk_pop(KRB5_APREQBODY, (st))
#define sk_KRB5_APREQBODY_sort(st) SKM_sk_sort(KRB5_APREQBODY, (st))

#define sk_KRB5_AUTHDATA_new(st) SKM_sk_new(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_new_null() SKM_sk_new_null(KRB5_AUTHDATA)
#define sk_KRB5_AUTHDATA_free(st) SKM_sk_free(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_num(st) SKM_sk_num(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_value(st, i) SKM_sk_value(KRB5_AUTHDATA, (st), (i))
#define sk_KRB5_AUTHDATA_set(st, i, val) SKM_sk_set(KRB5_AUTHDATA, (st), (i), (val))
#define sk_KRB5_AUTHDATA_zero(st) SKM_sk_zero(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_push(st, val) SKM_sk_push(KRB5_AUTHDATA, (st), (val))
#define sk_KRB5_AUTHDATA_unshift(st, val) SKM_sk_unshift(KRB5_AUTHDATA, (st), (val))
#define sk_KRB5_AUTHDATA_find(st, val) SKM_sk_find(KRB5_AUTHDATA, (st), (val))
#define sk_KRB5_AUTHDATA_delete(st, i) SKM_sk_delete(KRB5_AUTHDATA, (st), (i))
#define sk_KRB5_AUTHDATA_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_AUTHDATA, (st), (ptr))
#define sk_KRB5_AUTHDATA_insert(st, val, i) SKM_sk_insert(KRB5_AUTHDATA, (st), (val), (i))
#define sk_KRB5_AUTHDATA_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_AUTHDATA, (st), (cmp))
#define sk_KRB5_AUTHDATA_dup(st) SKM_sk_dup(KRB5_AUTHDATA, st)
#define sk_KRB5_AUTHDATA_pop_free(st, free_func) SKM_sk_pop_free(KRB5_AUTHDATA, (st), (free_func))
#define sk_KRB5_AUTHDATA_shift(st) SKM_sk_shift(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_pop(st) SKM_sk_pop(KRB5_AUTHDATA, (st))
#define sk_KRB5_AUTHDATA_sort(st) SKM_sk_sort(KRB5_AUTHDATA, (st))

#define sk_KRB5_AUTHENTBODY_new(st) SKM_sk_new(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_new_null() SKM_sk_new_null(KRB5_AUTHENTBODY)
#define sk_KRB5_AUTHENTBODY_free(st) SKM_sk_free(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_num(st) SKM_sk_num(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_value(st, i) SKM_sk_value(KRB5_AUTHENTBODY, (st), (i))
#define sk_KRB5_AUTHENTBODY_set(st, i, val) SKM_sk_set(KRB5_AUTHENTBODY, (st), (i), (val))
#define sk_KRB5_AUTHENTBODY_zero(st) SKM_sk_zero(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_push(st, val) SKM_sk_push(KRB5_AUTHENTBODY, (st), (val))
#define sk_KRB5_AUTHENTBODY_unshift(st, val) SKM_sk_unshift(KRB5_AUTHENTBODY, (st), (val))
#define sk_KRB5_AUTHENTBODY_find(st, val) SKM_sk_find(KRB5_AUTHENTBODY, (st), (val))
#define sk_KRB5_AUTHENTBODY_delete(st, i) SKM_sk_delete(KRB5_AUTHENTBODY, (st), (i))
#define sk_KRB5_AUTHENTBODY_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_AUTHENTBODY, (st), (ptr))
#define sk_KRB5_AUTHENTBODY_insert(st, val, i) SKM_sk_insert(KRB5_AUTHENTBODY, (st), (val), (i))
#define sk_KRB5_AUTHENTBODY_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_AUTHENTBODY, (st), (cmp))
#define sk_KRB5_AUTHENTBODY_dup(st) SKM_sk_dup(KRB5_AUTHENTBODY, st)
#define sk_KRB5_AUTHENTBODY_pop_free(st, free_func) SKM_sk_pop_free(KRB5_AUTHENTBODY, (st), (free_func))
#define sk_KRB5_AUTHENTBODY_shift(st) SKM_sk_shift(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_pop(st) SKM_sk_pop(KRB5_AUTHENTBODY, (st))
#define sk_KRB5_AUTHENTBODY_sort(st) SKM_sk_sort(KRB5_AUTHENTBODY, (st))

#define sk_KRB5_CHECKSUM_new(st) SKM_sk_new(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_new_null() SKM_sk_new_null(KRB5_CHECKSUM)
#define sk_KRB5_CHECKSUM_free(st) SKM_sk_free(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_num(st) SKM_sk_num(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_value(st, i) SKM_sk_value(KRB5_CHECKSUM, (st), (i))
#define sk_KRB5_CHECKSUM_set(st, i, val) SKM_sk_set(KRB5_CHECKSUM, (st), (i), (val))
#define sk_KRB5_CHECKSUM_zero(st) SKM_sk_zero(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_push(st, val) SKM_sk_push(KRB5_CHECKSUM, (st), (val))
#define sk_KRB5_CHECKSUM_unshift(st, val) SKM_sk_unshift(KRB5_CHECKSUM, (st), (val))
#define sk_KRB5_CHECKSUM_find(st, val) SKM_sk_find(KRB5_CHECKSUM, (st), (val))
#define sk_KRB5_CHECKSUM_delete(st, i) SKM_sk_delete(KRB5_CHECKSUM, (st), (i))
#define sk_KRB5_CHECKSUM_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_CHECKSUM, (st), (ptr))
#define sk_KRB5_CHECKSUM_insert(st, val, i) SKM_sk_insert(KRB5_CHECKSUM, (st), (val), (i))
#define sk_KRB5_CHECKSUM_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_CHECKSUM, (st), (cmp))
#define sk_KRB5_CHECKSUM_dup(st) SKM_sk_dup(KRB5_CHECKSUM, st)
#define sk_KRB5_CHECKSUM_pop_free(st, free_func) SKM_sk_pop_free(KRB5_CHECKSUM, (st), (free_func))
#define sk_KRB5_CHECKSUM_shift(st) SKM_sk_shift(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_pop(st) SKM_sk_pop(KRB5_CHECKSUM, (st))
#define sk_KRB5_CHECKSUM_sort(st) SKM_sk_sort(KRB5_CHECKSUM, (st))

#define sk_KRB5_ENCDATA_new(st) SKM_sk_new(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_new_null() SKM_sk_new_null(KRB5_ENCDATA)
#define sk_KRB5_ENCDATA_free(st) SKM_sk_free(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_num(st) SKM_sk_num(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_value(st, i) SKM_sk_value(KRB5_ENCDATA, (st), (i))
#define sk_KRB5_ENCDATA_set(st, i, val) SKM_sk_set(KRB5_ENCDATA, (st), (i), (val))
#define sk_KRB5_ENCDATA_zero(st) SKM_sk_zero(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_push(st, val) SKM_sk_push(KRB5_ENCDATA, (st), (val))
#define sk_KRB5_ENCDATA_unshift(st, val) SKM_sk_unshift(KRB5_ENCDATA, (st), (val))
#define sk_KRB5_ENCDATA_find(st, val) SKM_sk_find(KRB5_ENCDATA, (st), (val))
#define sk_KRB5_ENCDATA_delete(st, i) SKM_sk_delete(KRB5_ENCDATA, (st), (i))
#define sk_KRB5_ENCDATA_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_ENCDATA, (st), (ptr))
#define sk_KRB5_ENCDATA_insert(st, val, i) SKM_sk_insert(KRB5_ENCDATA, (st), (val), (i))
#define sk_KRB5_ENCDATA_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_ENCDATA, (st), (cmp))
#define sk_KRB5_ENCDATA_dup(st) SKM_sk_dup(KRB5_ENCDATA, st)
#define sk_KRB5_ENCDATA_pop_free(st, free_func) SKM_sk_pop_free(KRB5_ENCDATA, (st), (free_func))
#define sk_KRB5_ENCDATA_shift(st) SKM_sk_shift(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_pop(st) SKM_sk_pop(KRB5_ENCDATA, (st))
#define sk_KRB5_ENCDATA_sort(st) SKM_sk_sort(KRB5_ENCDATA, (st))

#define sk_KRB5_ENCKEY_new(st) SKM_sk_new(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_new_null() SKM_sk_new_null(KRB5_ENCKEY)
#define sk_KRB5_ENCKEY_free(st) SKM_sk_free(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_num(st) SKM_sk_num(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_value(st, i) SKM_sk_value(KRB5_ENCKEY, (st), (i))
#define sk_KRB5_ENCKEY_set(st, i, val) SKM_sk_set(KRB5_ENCKEY, (st), (i), (val))
#define sk_KRB5_ENCKEY_zero(st) SKM_sk_zero(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_push(st, val) SKM_sk_push(KRB5_ENCKEY, (st), (val))
#define sk_KRB5_ENCKEY_unshift(st, val) SKM_sk_unshift(KRB5_ENCKEY, (st), (val))
#define sk_KRB5_ENCKEY_find(st, val) SKM_sk_find(KRB5_ENCKEY, (st), (val))
#define sk_KRB5_ENCKEY_delete(st, i) SKM_sk_delete(KRB5_ENCKEY, (st), (i))
#define sk_KRB5_ENCKEY_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_ENCKEY, (st), (ptr))
#define sk_KRB5_ENCKEY_insert(st, val, i) SKM_sk_insert(KRB5_ENCKEY, (st), (val), (i))
#define sk_KRB5_ENCKEY_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_ENCKEY, (st), (cmp))
#define sk_KRB5_ENCKEY_dup(st) SKM_sk_dup(KRB5_ENCKEY, st)
#define sk_KRB5_ENCKEY_pop_free(st, free_func) SKM_sk_pop_free(KRB5_ENCKEY, (st), (free_func))
#define sk_KRB5_ENCKEY_shift(st) SKM_sk_shift(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_pop(st) SKM_sk_pop(KRB5_ENCKEY, (st))
#define sk_KRB5_ENCKEY_sort(st) SKM_sk_sort(KRB5_ENCKEY, (st))

#define sk_KRB5_PRINCNAME_new(st) SKM_sk_new(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_new_null() SKM_sk_new_null(KRB5_PRINCNAME)
#define sk_KRB5_PRINCNAME_free(st) SKM_sk_free(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_num(st) SKM_sk_num(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_value(st, i) SKM_sk_value(KRB5_PRINCNAME, (st), (i))
#define sk_KRB5_PRINCNAME_set(st, i, val) SKM_sk_set(KRB5_PRINCNAME, (st), (i), (val))
#define sk_KRB5_PRINCNAME_zero(st) SKM_sk_zero(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_push(st, val) SKM_sk_push(KRB5_PRINCNAME, (st), (val))
#define sk_KRB5_PRINCNAME_unshift(st, val) SKM_sk_unshift(KRB5_PRINCNAME, (st), (val))
#define sk_KRB5_PRINCNAME_find(st, val) SKM_sk_find(KRB5_PRINCNAME, (st), (val))
#define sk_KRB5_PRINCNAME_delete(st, i) SKM_sk_delete(KRB5_PRINCNAME, (st), (i))
#define sk_KRB5_PRINCNAME_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_PRINCNAME, (st), (ptr))
#define sk_KRB5_PRINCNAME_insert(st, val, i) SKM_sk_insert(KRB5_PRINCNAME, (st), (val), (i))
#define sk_KRB5_PRINCNAME_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_PRINCNAME, (st), (cmp))
#define sk_KRB5_PRINCNAME_dup(st) SKM_sk_dup(KRB5_PRINCNAME, st)
#define sk_KRB5_PRINCNAME_pop_free(st, free_func) SKM_sk_pop_free(KRB5_PRINCNAME, (st), (free_func))
#define sk_KRB5_PRINCNAME_shift(st) SKM_sk_shift(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_pop(st) SKM_sk_pop(KRB5_PRINCNAME, (st))
#define sk_KRB5_PRINCNAME_sort(st) SKM_sk_sort(KRB5_PRINCNAME, (st))

#define sk_KRB5_TKTBODY_new(st) SKM_sk_new(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_new_null() SKM_sk_new_null(KRB5_TKTBODY)
#define sk_KRB5_TKTBODY_free(st) SKM_sk_free(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_num(st) SKM_sk_num(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_value(st, i) SKM_sk_value(KRB5_TKTBODY, (st), (i))
#define sk_KRB5_TKTBODY_set(st, i, val) SKM_sk_set(KRB5_TKTBODY, (st), (i), (val))
#define sk_KRB5_TKTBODY_zero(st) SKM_sk_zero(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_push(st, val) SKM_sk_push(KRB5_TKTBODY, (st), (val))
#define sk_KRB5_TKTBODY_unshift(st, val) SKM_sk_unshift(KRB5_TKTBODY, (st), (val))
#define sk_KRB5_TKTBODY_find(st, val) SKM_sk_find(KRB5_TKTBODY, (st), (val))
#define sk_KRB5_TKTBODY_delete(st, i) SKM_sk_delete(KRB5_TKTBODY, (st), (i))
#define sk_KRB5_TKTBODY_delete_ptr(st, ptr) SKM_sk_delete_ptr(KRB5_TKTBODY, (st), (ptr))
#define sk_KRB5_TKTBODY_insert(st, val, i) SKM_sk_insert(KRB5_TKTBODY, (st), (val), (i))
#define sk_KRB5_TKTBODY_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(KRB5_TKTBODY, (st), (cmp))
#define sk_KRB5_TKTBODY_dup(st) SKM_sk_dup(KRB5_TKTBODY, st)
#define sk_KRB5_TKTBODY_pop_free(st, free_func) SKM_sk_pop_free(KRB5_TKTBODY, (st), (free_func))
#define sk_KRB5_TKTBODY_shift(st) SKM_sk_shift(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_pop(st) SKM_sk_pop(KRB5_TKTBODY, (st))
#define sk_KRB5_TKTBODY_sort(st) SKM_sk_sort(KRB5_TKTBODY, (st))

#define sk_MIME_HEADER_new(st) SKM_sk_new(MIME_HEADER, (st))
#define sk_MIME_HEADER_new_null() SKM_sk_new_null(MIME_HEADER)
#define sk_MIME_HEADER_free(st) SKM_sk_free(MIME_HEADER, (st))
#define sk_MIME_HEADER_num(st) SKM_sk_num(MIME_HEADER, (st))
#define sk_MIME_HEADER_value(st, i) SKM_sk_value(MIME_HEADER, (st), (i))
#define sk_MIME_HEADER_set(st, i, val) SKM_sk_set(MIME_HEADER, (st), (i), (val))
#define sk_MIME_HEADER_zero(st) SKM_sk_zero(MIME_HEADER, (st))
#define sk_MIME_HEADER_push(st, val) SKM_sk_push(MIME_HEADER, (st), (val))
#define sk_MIME_HEADER_unshift(st, val) SKM_sk_unshift(MIME_HEADER, (st), (val))
#define sk_MIME_HEADER_find(st, val) SKM_sk_find(MIME_HEADER, (st), (val))
#define sk_MIME_HEADER_delete(st, i) SKM_sk_delete(MIME_HEADER, (st), (i))
#define sk_MIME_HEADER_delete_ptr(st, ptr) SKM_sk_delete_ptr(MIME_HEADER, (st), (ptr))
#define sk_MIME_HEADER_insert(st, val, i) SKM_sk_insert(MIME_HEADER, (st), (val), (i))
#define sk_MIME_HEADER_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(MIME_HEADER, (st), (cmp))
#define sk_MIME_HEADER_dup(st) SKM_sk_dup(MIME_HEADER, st)
#define sk_MIME_HEADER_pop_free(st, free_func) SKM_sk_pop_free(MIME_HEADER, (st), (free_func))
#define sk_MIME_HEADER_shift(st) SKM_sk_shift(MIME_HEADER, (st))
#define sk_MIME_HEADER_pop(st) SKM_sk_pop(MIME_HEADER, (st))
#define sk_MIME_HEADER_sort(st) SKM_sk_sort(MIME_HEADER, (st))

#define sk_MIME_PARAM_new(st) SKM_sk_new(MIME_PARAM, (st))
#define sk_MIME_PARAM_new_null() SKM_sk_new_null(MIME_PARAM)
#define sk_MIME_PARAM_free(st) SKM_sk_free(MIME_PARAM, (st))
#define sk_MIME_PARAM_num(st) SKM_sk_num(MIME_PARAM, (st))
#define sk_MIME_PARAM_value(st, i) SKM_sk_value(MIME_PARAM, (st), (i))
#define sk_MIME_PARAM_set(st, i, val) SKM_sk_set(MIME_PARAM, (st), (i), (val))
#define sk_MIME_PARAM_zero(st) SKM_sk_zero(MIME_PARAM, (st))
#define sk_MIME_PARAM_push(st, val) SKM_sk_push(MIME_PARAM, (st), (val))
#define sk_MIME_PARAM_unshift(st, val) SKM_sk_unshift(MIME_PARAM, (st), (val))
#define sk_MIME_PARAM_find(st, val) SKM_sk_find(MIME_PARAM, (st), (val))
#define sk_MIME_PARAM_delete(st, i) SKM_sk_delete(MIME_PARAM, (st), (i))
#define sk_MIME_PARAM_delete_ptr(st, ptr) SKM_sk_delete_ptr(MIME_PARAM, (st), (ptr))
#define sk_MIME_PARAM_insert(st, val, i) SKM_sk_insert(MIME_PARAM, (st), (val), (i))
#define sk_MIME_PARAM_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(MIME_PARAM, (st), (cmp))
#define sk_MIME_PARAM_dup(st) SKM_sk_dup(MIME_PARAM, st)
#define sk_MIME_PARAM_pop_free(st, free_func) SKM_sk_pop_free(MIME_PARAM, (st), (free_func))
#define sk_MIME_PARAM_shift(st) SKM_sk_shift(MIME_PARAM, (st))
#define sk_MIME_PARAM_pop(st) SKM_sk_pop(MIME_PARAM, (st))
#define sk_MIME_PARAM_sort(st) SKM_sk_sort(MIME_PARAM, (st))

#define sk_NAME_FUNCS_new(st) SKM_sk_new(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_new_null() SKM_sk_new_null(NAME_FUNCS)
#define sk_NAME_FUNCS_free(st) SKM_sk_free(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_num(st) SKM_sk_num(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_value(st, i) SKM_sk_value(NAME_FUNCS, (st), (i))
#define sk_NAME_FUNCS_set(st, i, val) SKM_sk_set(NAME_FUNCS, (st), (i), (val))
#define sk_NAME_FUNCS_zero(st) SKM_sk_zero(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_push(st, val) SKM_sk_push(NAME_FUNCS, (st), (val))
#define sk_NAME_FUNCS_unshift(st, val) SKM_sk_unshift(NAME_FUNCS, (st), (val))
#define sk_NAME_FUNCS_find(st, val) SKM_sk_find(NAME_FUNCS, (st), (val))
#define sk_NAME_FUNCS_delete(st, i) SKM_sk_delete(NAME_FUNCS, (st), (i))
#define sk_NAME_FUNCS_delete_ptr(st, ptr) SKM_sk_delete_ptr(NAME_FUNCS, (st), (ptr))
#define sk_NAME_FUNCS_insert(st, val, i) SKM_sk_insert(NAME_FUNCS, (st), (val), (i))
#define sk_NAME_FUNCS_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(NAME_FUNCS, (st), (cmp))
#define sk_NAME_FUNCS_dup(st) SKM_sk_dup(NAME_FUNCS, st)
#define sk_NAME_FUNCS_pop_free(st, free_func) SKM_sk_pop_free(NAME_FUNCS, (st), (free_func))
#define sk_NAME_FUNCS_shift(st) SKM_sk_shift(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_pop(st) SKM_sk_pop(NAME_FUNCS, (st))
#define sk_NAME_FUNCS_sort(st) SKM_sk_sort(NAME_FUNCS, (st))

#define sk_OCSP_CERTID_new(st) SKM_sk_new(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_new_null() SKM_sk_new_null(OCSP_CERTID)
#define sk_OCSP_CERTID_free(st) SKM_sk_free(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_num(st) SKM_sk_num(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_value(st, i) SKM_sk_value(OCSP_CERTID, (st), (i))
#define sk_OCSP_CERTID_set(st, i, val) SKM_sk_set(OCSP_CERTID, (st), (i), (val))
#define sk_OCSP_CERTID_zero(st) SKM_sk_zero(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_push(st, val) SKM_sk_push(OCSP_CERTID, (st), (val))
#define sk_OCSP_CERTID_unshift(st, val) SKM_sk_unshift(OCSP_CERTID, (st), (val))
#define sk_OCSP_CERTID_find(st, val) SKM_sk_find(OCSP_CERTID, (st), (val))
#define sk_OCSP_CERTID_delete(st, i) SKM_sk_delete(OCSP_CERTID, (st), (i))
#define sk_OCSP_CERTID_delete_ptr(st, ptr) SKM_sk_delete_ptr(OCSP_CERTID, (st), (ptr))
#define sk_OCSP_CERTID_insert(st, val, i) SKM_sk_insert(OCSP_CERTID, (st), (val), (i))
#define sk_OCSP_CERTID_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(OCSP_CERTID, (st), (cmp))
#define sk_OCSP_CERTID_dup(st) SKM_sk_dup(OCSP_CERTID, st)
#define sk_OCSP_CERTID_pop_free(st, free_func) SKM_sk_pop_free(OCSP_CERTID, (st), (free_func))
#define sk_OCSP_CERTID_shift(st) SKM_sk_shift(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_pop(st) SKM_sk_pop(OCSP_CERTID, (st))
#define sk_OCSP_CERTID_sort(st) SKM_sk_sort(OCSP_CERTID, (st))

#define sk_OCSP_ONEREQ_new(st) SKM_sk_new(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_new_null() SKM_sk_new_null(OCSP_ONEREQ)
#define sk_OCSP_ONEREQ_free(st) SKM_sk_free(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_num(st) SKM_sk_num(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_value(st, i) SKM_sk_value(OCSP_ONEREQ, (st), (i))
#define sk_OCSP_ONEREQ_set(st, i, val) SKM_sk_set(OCSP_ONEREQ, (st), (i), (val))
#define sk_OCSP_ONEREQ_zero(st) SKM_sk_zero(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_push(st, val) SKM_sk_push(OCSP_ONEREQ, (st), (val))
#define sk_OCSP_ONEREQ_unshift(st, val) SKM_sk_unshift(OCSP_ONEREQ, (st), (val))
#define sk_OCSP_ONEREQ_find(st, val) SKM_sk_find(OCSP_ONEREQ, (st), (val))
#define sk_OCSP_ONEREQ_delete(st, i) SKM_sk_delete(OCSP_ONEREQ, (st), (i))
#define sk_OCSP_ONEREQ_delete_ptr(st, ptr) SKM_sk_delete_ptr(OCSP_ONEREQ, (st), (ptr))
#define sk_OCSP_ONEREQ_insert(st, val, i) SKM_sk_insert(OCSP_ONEREQ, (st), (val), (i))
#define sk_OCSP_ONEREQ_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(OCSP_ONEREQ, (st), (cmp))
#define sk_OCSP_ONEREQ_dup(st) SKM_sk_dup(OCSP_ONEREQ, st)
#define sk_OCSP_ONEREQ_pop_free(st, free_func) SKM_sk_pop_free(OCSP_ONEREQ, (st), (free_func))
#define sk_OCSP_ONEREQ_shift(st) SKM_sk_shift(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_pop(st) SKM_sk_pop(OCSP_ONEREQ, (st))
#define sk_OCSP_ONEREQ_sort(st) SKM_sk_sort(OCSP_ONEREQ, (st))

#define sk_OCSP_SINGLERESP_new(st) SKM_sk_new(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_new_null() SKM_sk_new_null(OCSP_SINGLERESP)
#define sk_OCSP_SINGLERESP_free(st) SKM_sk_free(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_num(st) SKM_sk_num(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_value(st, i) SKM_sk_value(OCSP_SINGLERESP, (st), (i))
#define sk_OCSP_SINGLERESP_set(st, i, val) SKM_sk_set(OCSP_SINGLERESP, (st), (i), (val))
#define sk_OCSP_SINGLERESP_zero(st) SKM_sk_zero(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_push(st, val) SKM_sk_push(OCSP_SINGLERESP, (st), (val))
#define sk_OCSP_SINGLERESP_unshift(st, val) SKM_sk_unshift(OCSP_SINGLERESP, (st), (val))
#define sk_OCSP_SINGLERESP_find(st, val) SKM_sk_find(OCSP_SINGLERESP, (st), (val))
#define sk_OCSP_SINGLERESP_delete(st, i) SKM_sk_delete(OCSP_SINGLERESP, (st), (i))
#define sk_OCSP_SINGLERESP_delete_ptr(st, ptr) SKM_sk_delete_ptr(OCSP_SINGLERESP, (st), (ptr))
#define sk_OCSP_SINGLERESP_insert(st, val, i) SKM_sk_insert(OCSP_SINGLERESP, (st), (val), (i))
#define sk_OCSP_SINGLERESP_set_cmp_func(st, cmp) SKM_sk_set_cmp_func(OCSP_SINGLERESP, (st), (cmp))
#define sk_OCSP_SINGLERESP_dup(st) SKM_sk_dup(OCSP_SINGLERESP, st)
#define sk_OCSP_SINGLERESP_pop_free(st, free_func) SKM_sk_pop_free(OCSP_SINGLERESP, (st), (free_func))
#define sk_OCSP_SINGLERESP_shift(st) SKM_sk_shift(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_pop(st) SKM_sk_pop(OCSP_SINGLERESP, (st))
#define sk_OCSP_SINGLERESP_sort(st) SKM_sk_sort(OCSP_SINGLERESP, (st))

#define sk_PKCS12_SAFEBAG_new(st) SKM_sk_new(PKCS12_SAFEBAG, (st))
#define sk_PKCS12_SAFEBAG_new_null() SKM_sk_new_null(PKCS12_SAFEBAG)
#define sk_PKCS12_SAFEBAG_free(st) SKM_sk_free(PKCS12_SAFEBAG, (st))
#define sk_PKCS12_SAFEBAG_num(st) SKM_sk_num(PKCS12_SAFEBAG, (st))
#define sk_PKCS12_SAFEBAG_value(st, i) SKM_sk_value(PKCS12_SAFEBAG, (st), (i))
#define sk_PKCS12_SAFEBAG_set(st, i, val) SKM_sk_set(PKCS12_SAFEBAG, (st), (i))

#endif //mcli add