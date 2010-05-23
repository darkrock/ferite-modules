/*
 * Copyright (C) 2002 Stephan Engström
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

#include "multipart.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ferite.h>

int multipart_fill_ferite_uarray( FeriteScript *script, FeriteVariable *query, FeriteString *ptr, char *boundary )
{
    mlist *parts;
    struct multipart *mpart;
    struct mime_field *field;
    struct mime_param *param, *filename, *varname;
    FeriteVariable *v, *obj;
    FeriteClass *cls;
    char buf[200], *p;
    int i, size;

    parts = multipart_parse( ptr->data, ptr->length, boundary );
    mlist_each_begin( parts );
    while( ( mpart = mlist_each( parts )) )
    {
        mlist_each_begin( mpart->fields );
        while( (field = mlist_each( mpart->fields )) )
        {
            if( strncasecmp( field->name.data, "Content-Disposition", field->name.length ) == 0)
            {
                filename = NULL;
                varname = NULL;
                mlist_each_begin( field->params );
                while( ( param = mlist_each( field->params )) )
                {
                    if(strncmp(param->key.data, "filename", param->key.length) == 0)
                      filename = param;
                    if(strncmp(param->key.data, "name", param->key.length) == 0)
                      varname = param;
                }
                if( varname )
                {
                    snprintf( buf, 200, "%.*s", varname->value.length, varname->value.data );
                    if( filename )
                    {
                        cls = ferite_find_class( script, script->mainns, "MultiPart" );
                        obj = ferite_build_object( script, cls );
                        v = fe_new_str( "name", varname->value.data, varname->value.length, FE_CHARSET_DEFAULT );
                        ferite_object_set_var( script, VAO(obj), "name", v );
 
			p = filename->value.data;
			size = filename->value.length;

			for( i = size; i >= 0; i-- )
			{
			    if( filename->value.data[i] == '\\' || filename->value.data[i] == '/' || filename->value.data[i] == ':' )
			    {
				p = filename->value.data + i + 1;
				size--;
				break;
			    }
			}
			v = fe_new_str( "filename", p, size - i, FE_CHARSET_DEFAULT );
			ferite_object_set_var( script, VAO(obj), "filename", v );
                        v = fe_new_str( "filedata", mpart->data.data, mpart->data.length, FE_CHARSET_DEFAULT );
                        ferite_object_set_var( script, VAO(obj), "data", v );
                        ferite_uarray_add( script, VAUA(query), obj, buf, FE_ARRAY_ADD_AT_END );
                    }
                    else if ( mpart->data.length )
                    {
                        v = fe_new_str( "filedata", mpart->data.data, mpart->data.length, FE_CHARSET_DEFAULT );
                        ferite_uarray_add( script, VAUA(query), v, buf, FE_ARRAY_ADD_AT_END );
                    }
                }
            }
        }
    }
}

struct str *multipart_getline( char **data )
{
    char *p, *ldata = *data;
    struct str *s;
    if((p = strstr(ldata, "\r\n")) && p != ldata )
    {
        s = calloc( 1, sizeof(struct str) );
        s->data = ldata;
        s->length = p - ldata;
        *data = p + 2;
        return s;
    }
    if( p )
      *data = p + 2;
    return NULL;
}
struct mime_field *multipart_parseline( struct str *line )
{
    struct mime_field *field = calloc( 1, sizeof( struct mime_field ) );
    char *value, *data;
    struct mime_param *param = NULL, *p = NULL;
    int token;

    data = calloc( 1, line->length + 1 );
    strncpy( data, line->data, line->length );
    if( (value = strchr(data,':')) )
    {
        field->name.data = data;
        field->name.length = value - data;
        field->value.data = data = ++value;
        EATSPACE(data);
        while( !( *data =='\0' || *data == ' ' || *data == '\t' || *data == ';') )
          data++;
        field->value.length = data - field->value.data;
        EATSPACE(data);
        field->params = mlist_new();
        while( *data == ';' )
        {
            param = calloc( 1, sizeof( struct mime_param ) );
            data++;
            EATSPACE(data);
            param->key.data = data;
            while( *data != '=' )
              data++;
            param->key.length = data++ - param->key.data;
            token = ( *data == '"' ) ? 1 : 0 ;
            if( token )
              data++;
            param->value.data = data;
            while( *data  )
            {
                if( token && *data == '"' )
                  break;
                data++;
            }
            param->value.length = data - param->value.data;
            if( token )
              data++;
            EATSPACE(data);
            mlist_append( field->params, param );
        }
    }
    return field;
}
mlist *multipart_parse( char *data, int size, char *boundary )
{
    char *row;
    int boundary_length = strlen( boundary );
    struct str *line;
    struct mime_field *field;
    struct multipart *mpart;
    mlist *parts = mlist_new();

    while((row = memstr(data, boundary, size)))
    {
        if( row[boundary_length] == '-' && row[boundary_length+1] == '-' )
          break;
        mpart = calloc( 1, sizeof( struct multipart ) );
        mpart->fields = mlist_new();
        row += boundary_length + 2;
        while((line = multipart_getline( &row )))
        {
            mlist_append( mpart->fields, multipart_parseline( line ) );
            free(line);
        }
        mpart->data.data = row;
        size -= row - data;
        if((data = memstr(row, boundary, size)) == NULL)
        {
            printf(" Major fuckup\n");
            break;
        }
        data -= 4;
        size -= data - row;
        mpart->data.length = data - row;
        mlist_append( parts, mpart );
    }
    return parts;
}
void multipart_free( mlist *parts )
{
    struct multipart *mpart;
    struct mime_field *field;
    struct mime_param *param;

    mlist_each_begin( parts );
    while( (mpart = mlist_each( parts )) )
    {
        mlist_each_begin( mpart->fields );
        while( (field = mlist_each( mpart->fields )) )
        {
            mlist_each_begin( field->params );
            while( (param = mlist_each( field->params )) )
              free( param );
            mlist_delete( field->params );
            free( field );
        }
        mlist_delete( mpart->fields );
        free( mpart );
    }
    mlist_delete( parts );
}
void multipart_dump_parts( mlist *parts )
{
    struct multipart *mpart;
    struct mime_field *field;

    mlist_each_begin( parts );
    while( (mpart = mlist_each( parts )) )
    {
        printf("------------------------------\n");
        mlist_each_begin( mpart->fields );
        while( (field = mlist_each( mpart->fields )) )
          multipart_dump_field( field );
        printf("------------------------------\n");
        printf("Data: %.*s\n", mpart->data.length, mpart->data.data );
        printf("------------------------------\n");
    }
}
void multipart_dump_field( struct mime_field *f )
{
    struct mime_param *param;
    printf("fname: %.*s|\n",f->name.length, f->name.data );
    printf("fvalue: %.*s|\n",f->value.length, f->value.data );
    mlist_each_begin( f->params );
    while( (param = mlist_each( f->params )) )
    {
        printf("fparam-key: %.*s|\n",param->key.length, param->key.data );
        printf("fparam-val: %.*s|\n", param->value.length, param->value.data );
    }
}

