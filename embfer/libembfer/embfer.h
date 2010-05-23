#ifndef __UTIL_EMBER_H__
#define __UTIL_EMBER_H__

#include <ferite.h>

typedef struct __embfer_source {

    char *source;
    int   size;
    int   location;

} EmbferSource;

typedef void (*EmbferCallback)( FeriteBuffer *output, const char *type, const char *contents );

void          embfer_init();
void          embfer_deinit();
void          embfer_set_output_fnc( const char *name );
FeriteString *embfer_convert_from_string( const char *source );
FeriteString *embfer_convert_from_file( const char *path );
void          embfer_register_tag_handler( char *type, EmbferCallback cb );

#endif
