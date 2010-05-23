/*
 * Copyright (C) 2002, Sveinung Haslestad 
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
 * o Neither the name of the ferite software nor the names of its contributors may
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
#include <stdio.h>

char *create_iv(int size, int seed ){
  int i;
  char *iv = malloc(size*sizeof(char));
  if( iv == NULL )
    return NULL;
  srand(seed);
  for(i=0;i<size;i++)
    iv[i]=rand();
  return iv;
}

char *create_key(char *pwd,int key_size) {
  char* key=calloc(1,key_size*sizeof(char));
  int len;
  
  KEYGEN keygen;
 
  //printf("KEYGEN_MCRYPT uses %d algoritms \n", mhash_keygen_uses_hash_algorithm(KEYGEN_MCRYPT));
  //printf(" does KEYGEN_MCRYPT use salt? %s\n",
  //mhash_keygen_uses_salt( KEYGEN_MCRYPT)?"yes":"no");
  keygen.count = 0;
  keygen.salt= NULL;
  keygen.hash_algorithm[0] = MHASH_MD5;
  keygen.salt_size = mhash_get_keygen_salt_size(MHASH_MD5);
  
  
  if( key == NULL )
    return NULL;

  mhash_keygen_ext( KEYGEN_MCRYPT ,keygen, key, key_size, pwd, strlen(pwd));
  
  return key;
}














