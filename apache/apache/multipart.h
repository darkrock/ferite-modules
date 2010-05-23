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

struct mlist_element
{
    void *value;
    struct mlist_element *next;
};
typedef struct mlist_element mlist_element;
struct mlist
{
    struct mlist_element *root;
    struct mlist_element *last;
    struct mlist_element *current;
    int count;
};
typedef struct mlist mlist;
struct str
{
    char *data;
    int length;
};
struct mime_param
{
    struct str key;
    struct str value;
};
struct mime_field
{
    struct str name;
    struct str value;
    mlist *params;
};
struct multipart
{
    mlist *fields;
    struct str data;
};

#define EATSPACE( str ) { while( *str == ' ' || *str == '\t' ) str++; }

char *memstr(char *haystack, char *needle, int size);

struct str *multipart_getline( char **data );
struct mime_field *multipart_parseline( struct str *line );
mlist *multipart_parse( char *data, int size, char *boundary );
void multipart_free( mlist *parts );
void multipart_dump_parts( mlist *parts );
void multipart_dump_field( struct mime_field *f );

mlist *mlist_new( );
int mlist_append(mlist *ll, void *data);
int mlist_prepend(mlist *ll, void *data);
void mlist_each_begin(mlist *ll);
void *mlist_each(mlist *ll);
int mlist_delete(mlist *ll);

