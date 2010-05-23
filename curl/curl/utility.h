#include <curl/curl.h>
#include <ferite.h>

#define OPTIONS_SIZE            116
#define CURLOPT_RETURNTRANSFER  -1

typedef struct curl_object {
    CURL *handle;
    struct curl_httppost *httppost;
    struct curl_slist *httpheader;
    struct curl_slist *http200aliases;
    struct curl_slist *quote;
    struct curl_slist *postquote;
    struct curl_slist *prequote;
    /* misc */
    void *options[OPTIONS_SIZE];
    char error[CURL_ERROR_SIZE+1];
    FeriteVariable *content_buffer;
    FeriteVariable *header_buffer;
	FeriteStack    *string_stack;
	FeriteStack    *headers_stack;
} curl_object;

size_t ferite_curl_write_callback( int flags, char *ptr, size_t size, size_t nmemb, void *stream);
size_t ferite_curl_read_callback( char *ptr, size_t size, size_t nmemb, void *stream);
size_t ferite_curl_content_callback( char *ptr, size_t size, size_t nmemb, void *stream);
size_t ferite_curl_header_callback( char *ptr, size_t size, size_t nmemb, void *stream);
curl_object *ferite_curl_object_init( FeriteScript *script );
