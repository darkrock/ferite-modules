/*
 * Copyright (C) 2002 Christian M. Stamgren, Stephan Engström, Chris Ross
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * o Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * o Neither the name of the mod_ferite software nor the names of its contributors may
 *   be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "utility.h"

static FeriteString *get_get_data( FeriteScript *script, request_rec *r );
static FeriteString *get_post_data( FeriteScript *script, request_rec *r );
static FeriteVariable *popoluate_ferite_array_with_query_data(FeriteScript *script, FeriteVariable **query, request_rec *r, char *data );


FeriteString *apache_get_query_data( FeriteScript *script, request_rec *r )
{    
    FeriteString *ptr = NULL;
    if( r->method_number == M_GET || r->assbackwards )
	    ptr = get_get_data( script, r );
    else if( r->method_number == M_POST )
	    ptr = get_post_data( script, r );
    
    return ptr;
}

FeriteVariable *parse_query_data( FeriteScript *script, struct apache_ctx *ctx ) 
{
	FeriteVariable *query = NULL;
	char *ct = NULL, *boundary = NULL;
	
	query = ferite_create_uarray_variable( script, "query", FE_ARRAY_DEFAULT_SIZE, FE_STATIC );
	if( ctx->r->headers_in != NULL )
		ct = (char *)apr_table_get(ctx->r->headers_in, "Content-type");
	if(ct && ctx->r->method_number == M_POST && strncasecmp(ct, "multipart/form-data", 19) == 0)
	{
		boundary = strstr(ct,"boundary=");
		boundary += 9;
		multipart_fill_ferite_uarray( script, query, ctx->query, boundary );
	} 
	else if( ctx->r->method_number == M_POST )
	{
		popoluate_ferite_array_with_query_data( script, &query, ctx->r, ctx->query->data );
	} 
	if( ctx->r->args ) 
		popoluate_ferite_array_with_query_data( script, &query, ctx->r, ctx->r->args );	
		
	return query;	
}


static FeriteVariable *popoluate_ferite_array_with_query_data(FeriteScript *script, FeriteVariable **query, request_rec *r, char *data )
{
	char *p = NULL, *ptr = NULL, *pair = NULL, *key = NULL;
	int i;
	FeriteVariable *v, *realQuery = *query;

	p = ptr = fstrdup( data );
	while( (pair = strsep(&p, "&")) )
	{
		for(i=0;pair[i];i++)
			if(pair[i] == '+')
				pair[i] = ' ';
		if( (key = strsep(&pair,"=")) && pair )
		{
			ap_unescape_url(key);
			ap_unescape_url(pair);
			v = fe_new_str( "query-value", pair, 0, FE_CHARSET_DEFAULT );
			ferite_uarray_add( script, VAUA(realQuery), v, key, FE_ARRAY_ADD_AT_END );
		}
	}
	ffree( ptr );
}

static FeriteString *get_get_data( FeriteScript *script, request_rec *r ) 
{
	FeriteString *ptr = NULL;
        if( r->args == NULL )
		ptr = ferite_str_new( script, "", 0, FE_CHARSET_DEFAULT );
        else
		ptr = ferite_str_new( script, r->args, 0, FE_CHARSET_DEFAULT );
	
	return ptr;
}

static FeriteString *get_post_data( FeriteScript *script, request_rec *r )
{
	FeriteString *ptr;
	FeriteBuffer *buf; 
	char str[HUGE_STRING_LEN];
	int read;

	if( ap_should_client_block(r) )
        {
		buf = ferite_buffer_new( script, 0 );
		while((read = ap_get_client_block(r, str, HUGE_STRING_LEN)) > 0)
		{
			ferite_buffer_add( script, buf, str, read );
		}
		ptr = ferite_buffer_to_str( script, buf );
		ferite_buffer_delete( script, buf );
	}
	else
	{
		ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r, "mod_ferite: Post without data." );
		return ferite_str_new( script, "", 0, FE_CHARSET_DEFAULT ); 
	}	
	return ptr;
}
