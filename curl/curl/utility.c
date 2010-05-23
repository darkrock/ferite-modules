/*
 * Written by Paul Querna <chip force-elite.com>
 *
 */

#include "utility.h"

size_t ferite_curl_content_callback( char *ptr, size_t size, size_t nmemb, void *stream) 
{
     return ferite_curl_write_callback( 1, ptr, size, nmemb, stream);
}

size_t ferite_curl_header_callback( char *ptr, size_t size, size_t nmemb, void *stream) 
{
     return ferite_curl_write_callback( 2, ptr, size, nmemb, stream);
}

size_t ferite_curl_read_callback( char *ptr, size_t size, size_t nmemb, void *stream )
{
	curl_object *crl = stream;
	int w_size = (int)(size * nmemb);
	int realsize = 0;
	FeriteString *b = VAS(crl->content_buffer);
	
	if( b->length ) {
		realsize = (b->length > w_size ? w_size : b->length);
		memcpy( ptr, b->data, realsize );
		memmove( b->data, b->data + realsize, b->length - realsize );
		b->length -= realsize;
		return realsize;
	}
	return 0;
}

size_t ferite_curl_write_callback( int flags, char *ptr, size_t size, size_t nmemb, void *stream)
{
    curl_object *crl = stream;
    int w_size;

    w_size = (int)(size * nmemb);

    if(w_size <= 0) {
        return 0;
    }

    if(flags == 1 ) {
        ferite_str_data_cat( FE_NoScript, VAS(crl->content_buffer), ptr, w_size);
    } 
    else {
        ferite_str_data_cat( FE_NoScript, VAS(crl->header_buffer), ptr, w_size);
    }

    return w_size;
}

curl_object *ferite_curl_object_init( FeriteScript *script ) 
{
    int res;
    curl_object *crl = fmalloc( sizeof( curl_object ) );

    crl->httppost = NULL;
    crl->httpheader = NULL;
    crl->http200aliases = NULL;
    crl->quote = NULL;
    crl->postquote = NULL;
    crl->prequote = NULL;

	crl->string_stack = ferite_create_stack(script,10);
	crl->headers_stack = ferite_create_stack(script,10);
    /* misc */
    void *options[OPTIONS_SIZE];
    memset(crl->options, 0, sizeof(crl->options));
    memset(crl->error, 0, sizeof(crl->error));

    if( ( crl->handle = curl_easy_init() ) == NULL ) {
        ferite_error( script, 0, "[Ferite::Curl] curl_easy_init failed\n");
        goto error;
    }

    res = curl_easy_setopt(crl->handle, CURLOPT_ERRORBUFFER, crl->error);    
    if (res != CURLE_OK) {
        ferite_error( script, 0, "[Ferite::Curl] Failed to Set Error Buffer\n");
        goto error;
    }

    res = curl_easy_setopt(crl->handle, CURLOPT_NOPROGRESS, (long)1);
    if (res != CURLE_OK) {
        ferite_error( script, 0, "[Ferite::Curl] Failed to Set CURLOPT_NOPROGRESS\n");
        goto error;
    }

    res = curl_easy_setopt(crl->handle, CURLOPT_VERBOSE, (long)0);
    if (res != CURLE_OK) {
        ferite_error( script, 0, "[Ferite::Curl]  Failed to Set CURLOPT_VERBOSE\n");
        goto error;
    }

    res = curl_easy_setopt(crl->handle, CURLOPT_NOSIGNAL, (long)0);
    if (res != CURLE_OK) {
        ferite_error( script, 0, "[Ferite::Curl]  Failed to Set CURLOPT_NOSIGNAL\n");
        goto error;
    }

    crl->content_buffer = NULL;
    crl->header_buffer = NULL;

    return crl;

error:
      ffree( crl );
      return NULL;
}
