/*
 * %CopyrightBegin%
 * 
 * Copyright Ericsson AB 2001-2009. All Rights Reserved.
 * 
 * The contents of this file are subject to the Erlang Public License,
 * Version 1.1, (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * Erlang Public License along with this software. If not, it can be
 * retrieved online at http://www.erlang.org/.
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 * 
 * %CopyrightEnd%
 *

 */
/*
 * ei_x_encode to encode in a self-expanding buffer
 */

#ifdef VXWORKS
#include <vxWorks.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef VRTX
#define __READY_EXTENSIONS__
#include <errno.h>
#endif

#include "eidef.h"
#include "ei_x_encode.h"
#include "ei_malloc.h"

int ei_x_extra = 100;

int ei_x_new(ei_x_buff* x)
{
    x->buff = ei_malloc(ei_x_extra);
    x->buffsz = ei_x_extra;
    x->index = 0;
    return x->buff != NULL ? 0 : -1;
}

int ei_x_new_with_version(ei_x_buff* x)
{
    if (ei_x_new(x) < 0)
	return -1;
    return ei_encode_version(x->buff, &x->index);
}

int ei_x_free(ei_x_buff* x)
{
    if (x->buff == NULL)
	return -1;
    ei_free(x->buff);
    x->buff = NULL;
    return 0;
}

int x_fix_buff(ei_x_buff* x, int szneeded)
{
    int sz = szneeded + ei_x_extra;
    if (sz > x->buffsz) {
	sz += ei_x_extra;	/* to avoid reallocating each and every time */
	x->buffsz = sz;
	x->buff = ei_realloc(x->buff, sz);
    }
    return x->buff != NULL;
}

int ei_x_append(ei_x_buff* x, const ei_x_buff* x2)
{
    return ei_x_append_buf(x, x2->buff, x2->index);
}

int ei_x_append_buf(ei_x_buff* x, const char* buf, int len)
{
    if (!x_fix_buff(x, x->index+len))
	return -1;
    memcpy(&x->buff[x->index], buf, len);
    x->index += len;
    return 0;
}

int ei_x_encode_string(ei_x_buff* x, const char* s)
{
    return ei_x_encode_string_len(x, s, strlen(s));
}

int ei_x_encode_string_len(ei_x_buff* x, const char* s, int len)
{
    int i = x->index;
    ei_encode_string_len(NULL, &i, s, len);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_string_len(x->buff, &x->index, s, len);
}

int ei_x_encode_binary(ei_x_buff* x, const void* p, int len)
{
    int i = x->index;
    ei_encode_binary(NULL, &i, p, len);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_binary(x->buff, &x->index, p, len);
}

int ei_x_encode_long(ei_x_buff* x, long n)
{
    int i = x->index;
    ei_encode_long(NULL, &i, n);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_long(x->buff, &x->index, n);
}

int ei_x_encode_ulong(ei_x_buff* x, unsigned long n)
{
    int i = x->index;
    ei_encode_ulong(NULL, &i, n);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_ulong(x->buff, &x->index, n);
}

int ei_x_encode_char(ei_x_buff* x, char p)
{
    int i = x->index;
    ei_encode_char(NULL, &i, p);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_char(x->buff, &x->index, p);
}

int ei_x_encode_boolean(ei_x_buff* x, int p)
{
    int i = x->index;
    ei_encode_boolean(NULL, &i, p);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_boolean(x->buff, &x->index, p);
}

int ei_x_encode_double(ei_x_buff* x, double dbl)
{
    int i = x->index;
    ei_encode_double(NULL, &i, dbl);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_double(x->buff, &x->index, dbl);
}

int ei_x_encode_list_header(ei_x_buff* x, long n)
{
    int i = x->index;
    ei_encode_list_header(NULL, &i, n);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_list_header(x->buff, &x->index, n);
}

int ei_x_encode_empty_list(ei_x_buff* x)
{
    int i = x->index;
    ei_encode_empty_list(NULL, &i);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_empty_list(x->buff, &x->index);
}

int ei_x_encode_version(ei_x_buff* x)
{
    int i = x->index;
    ei_encode_version(NULL, &i);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_version(x->buff, &x->index);
}

int ei_x_encode_tuple_header(ei_x_buff* x, long n)
{
    int i = x->index;
    ei_encode_tuple_header(NULL, &i, n);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_tuple_header(x->buff, &x->index, n);
}

int ei_x_encode_atom(ei_x_buff* x, const char* s)
{
    return ei_x_encode_atom_len_as(x, s, strlen(s), ERLANG_LATIN1, ERLANG_LATIN1);
}

int ei_x_encode_atom_len(ei_x_buff* x, const char* s, int len)
{
    return ei_x_encode_atom_len_as(x, s, len, ERLANG_LATIN1, ERLANG_LATIN1);
}

int ei_x_encode_atom_as(ei_x_buff* x, const char* s,
			enum erlang_char_encoding from_enc,
			enum erlang_char_encoding to_enc)
{
    return ei_x_encode_atom_len_as(x, s, strlen(s), from_enc, to_enc);
}

int ei_x_encode_atom_len_as(ei_x_buff* x, const char* s, int len,
			    enum erlang_char_encoding from_enc,
			    enum erlang_char_encoding to_enc)
{
    int i = x->index;
    ei_encode_atom_len_as(NULL, &i, s, len, from_enc, to_enc);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_atom_len_as(x->buff, &x->index, s, len, from_enc, to_enc);
}


int ei_x_encode_pid(ei_x_buff* x, const erlang_pid* pid)
{
    int i = x->index;
    ei_encode_pid(NULL, &i, pid);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_pid(x->buff, &x->index, pid);
}

int ei_x_encode_fun(ei_x_buff* x, const erlang_fun* fun)
{
    int i = x->index;
    ei_encode_fun(NULL, &i, fun);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_fun(x->buff, &x->index, fun);
}

int ei_x_encode_ref(ei_x_buff* x, const erlang_ref* ref)
{
    int i = x->index;
    ei_encode_ref(NULL, &i, ref);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_ref(x->buff, &x->index, ref);
}

int ei_x_encode_port(ei_x_buff* x, const erlang_port* port)
{
    int i = x->index;
    ei_encode_port(NULL, &i, port);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_port(x->buff, &x->index, port);
}

int ei_x_encode_trace(ei_x_buff* x, const erlang_trace* trace)
{
    int i = x->index;
    ei_encode_trace(NULL, &i, trace);
    if (!x_fix_buff(x, i))
	return -1;
    return ei_encode_trace(x->buff, &x->index, trace);
}
