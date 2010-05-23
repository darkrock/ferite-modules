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

#include <ferite.h>

char *url_encode( char *to_encode, int length )
{
    char *encoded;
    unsigned char hex[] = "0123456789ABCDEF";
    register int x, y;

    encoded = (char *) fmalloc_ngc(3 * length + 1); /* worst case */
    for (x = 0, y = 0; length--; x++, y++)
    {
        if (to_encode[x] == ' ')
        {
            encoded[y] = '+';
        }
        else if ((to_encode[x] < 'A' && to_encode[x] > '9') ||
                 (to_encode[x] > 'Z' && to_encode[x] < 'a' && to_encode[x] != '_') ||
                 (to_encode[x] > 'z') || (to_encode[x] < '0')) 
        {
            encoded[y++] = '%';
            encoded[y++] = hex[(unsigned char) to_encode[x] >> 4];
            encoded[y] = hex[(unsigned char) to_encode[x] & 15];
        }
        else
          encoded[y] = to_encode[x];
    }
    encoded[y] = '\0';
    return encoded;
}
