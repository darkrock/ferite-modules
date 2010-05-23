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

#include <stdlib.h>
#include <string.h>
#include "utility.h"

mlist *mlist_new( )
{
    mlist *ll = calloc(1,sizeof(mlist));
    if(!ll)
      return NULL;
    return ll;
}
int mlist_append(mlist *ll, void *data)
{
    mlist_element *e = malloc(sizeof(mlist_element));
    if(!e)
      return 1;
    e->value = data;
    e->next = NULL;
    if(ll->last)
      ll->last->next = e;
    else
      ll->root = e;
    ll->last = e;
    ll->count++;
    return 0;
}
int mlist_prepend(mlist *ll, void *data)
{
    mlist_element *e = malloc(sizeof(mlist_element));
    if(!e)
      return 1;
    e->value = data;
    e->next = ll->root;
    ll->root = e;
    if(!ll->last)
      ll->last = e;
    ll->count++;
    return 0;
}
void mlist_each_begin(mlist *ll)
{
    ll->current = ll->root;
}
void *mlist_each(mlist *ll)
{
    void *ptr;
    if(!ll->current)
      return NULL;
    ptr = ll->current->value;
    ll->current = ll->current->next;
    return ptr;
}
int mlist_delete(mlist *ll)
{
    mlist_element *e = ll->root;
    mlist_element *next;
    while(e)
    {
        next = e->next;
        free(e);
        e = next;
    }
    return 0;
}
char *memstr(char *haystack, char *needle, int size)
{
    char *p;
    char needlesize = strlen(needle);
    for (p = haystack; p <= (haystack-needlesize+size); p++)
    {
        if(*p == *needle)
          if (memcmp(p, needle, needlesize) == 0)
            return p; /* found */
    }
    return NULL;
}

