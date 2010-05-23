#include "embfer.h"

char *__embfer_output_function = NULL;
FeriteHash *__embfer_callback_hash = NULL;

EmbferSource *__embfer_create_source( const char *source )
{
	EmbferSource *s = fmalloc_ngc( sizeof( EmbferSource ) );
	s->location = 0;
	s->source = strdup( source );
	s->size = strlen( source );
	return s;
}

void __embfer_delete_source( EmbferSource *source )
{
	free( source->source );
	ffree_ngc( source );
}

#define NOT_PEN( l, r ) ((l + 1) < r)
void __embfer_walk_until_embfer_tag( EmbferSource *source, FeriteBuffer *output )
{
	int gotContent = FE_FALSE;
	
	for( ; source->location < source->size; source->location++ )
	{
		char c = source->source[source->location];
		if( c == '<' && NOT_PEN(source->location, source->size) )
		{
			char d = source->source[source->location + 1];
			if( d == '?' )
				break;
		}
		if( !gotContent )
		{
			ferite_buffer_add_str( FE_NoScript, output, __embfer_output_function );
			ferite_buffer_add_str( FE_NoScript, output, "(\"" );
			gotContent = FE_TRUE;
		}
		if( c == '\\' || c == '"' || c == '$' )
			ferite_buffer_add_char( FE_NoScript, output, '\\' );
		ferite_buffer_add_char( FE_NoScript, output, c );
	}
	if( gotContent )
		ferite_buffer_add_str( FE_NoScript, output, "\");" );
}

/*
 The anatomy of a embfer tag:
 <?type contents?>
 */

void __embfer_tag_code_handler( FeriteBuffer *output, const char *type, const char *contents )
{
	ferite_buffer_add_str( FE_NoScript, output, (char*)contents );
}
void __embfer_tag_expr_handler( FeriteBuffer *output, const char *type, const char *contents )
{
	ferite_buffer_add_str( FE_NoScript, output, __embfer_output_function );
	ferite_buffer_add_str( FE_NoScript, output, "(\"\" + (" );
	ferite_buffer_add_str( FE_NoScript, output, (char*)contents );
	ferite_buffer_add_str( FE_NoScript, output, ") );" );
}
void __embfer_tag_comment_handler( FeriteBuffer *output, const char *type, const char *contents )
{
	ferite_buffer_add_str( FE_NoScript, output, "/* " );
	 ferite_buffer_add_str( FE_NoScript, output, (char*)contents );
	 ferite_buffer_add_str( FE_NoScript, output, " */" );
}

void __embfer_walk_until_end_of_embfer_tag( EmbferSource *source, FeriteBuffer *output )
{
	EmbferCallback cb = NULL;
	FeriteBuffer *type = ferite_buffer_new( FE_NoScript, 128 );
	FeriteBuffer *contents = ferite_buffer_new( FE_NoScript, 1024 );
	FeriteString *flatType = NULL, *flatContents = NULL;
	int seenType = FE_FALSE;

	for( source->location += 2; source->location < source->size; source->location++ )
	{
		char c = source->source[source->location];
		if( c == '?' && NOT_PEN(source->location, source->size) )
		{
			char d = source->source[source->location + 1];
			if( d == '>' )
			{
				source->location += 2;
				break;
			}
		}
		else if( !seenType && (c == ' ' || c == '\r' || c == '\t' || c == '\n' ))
		{
			seenType = FE_TRUE;
			continue;
		}
		
		if( seenType )
			ferite_buffer_add_char( FE_NoScript, contents, c );
		else
			ferite_buffer_add_char( FE_NoScript, type, c );
	}
	
	flatType = ferite_buffer_to_str( FE_NoScript, type );
	flatContents = ferite_buffer_to_str( FE_NoScript, contents );
	cb = ferite_hash_get( NULL, __embfer_callback_hash, flatType->data );
	if( cb != NULL )
		(cb)( output, flatType->data, flatContents->data );
	else
	{
		ferite_buffer_add_str( FE_NoScript, output, __embfer_output_function );
		ferite_buffer_add_str( FE_NoScript, output, "('" );
		ferite_buffer_add_str( FE_NoScript, output, (char*)contents );
		ferite_buffer_add_str( FE_NoScript, output, "<?" );
		ferite_buffer_add_str( FE_NoScript, output, flatType->data );
		ferite_buffer_add_str( FE_NoScript, output, " " );
		ferite_buffer_add_str( FE_NoScript, output, flatContents->data );
		ferite_buffer_add_str( FE_NoScript, output, " ?>" );
		ferite_buffer_add_str( FE_NoScript, output, "');" );
	}
	ferite_buffer_delete( FE_NoScript, type );
	ferite_buffer_delete( FE_NoScript, contents );
	ferite_str_destroy( FE_NoScript, flatType );
	ferite_str_destroy( FE_NoScript, flatContents );
}

void embfer_init()
{
	if( __embfer_callback_hash == NULL )
	{
		__embfer_callback_hash = ferite_create_hash( NULL, 32 );
		embfer_register_tag_handler( "",  (EmbferCallback)__embfer_tag_code_handler );
		embfer_register_tag_handler( "=", (EmbferCallback)__embfer_tag_expr_handler );
		embfer_register_tag_handler( "#", (EmbferCallback)__embfer_tag_comment_handler );
	}
}
void embfer_deinit()
{
	if( __embfer_callback_hash != NULL )
		ferite_delete_hash( NULL, __embfer_callback_hash, NULL );
	__embfer_callback_hash = NULL;
}

void embfer_set_output_fnc( const char *name )
{
	if( __embfer_output_function != NULL )
		free( __embfer_output_function );
	__embfer_output_function = strdup( name );
}

#define EMBFER_SOURCE_REMAINING( SOURCE ) (SOURCE->location < SOURCE->size)
FeriteString *embfer_convert_from_string( const char *source )
{
	EmbferSource *s = __embfer_create_source( source );
	FeriteBuffer *output = ferite_buffer_new( FE_NoScript, strlen( source ) );
	FeriteString *flat_output = NULL;
	
	while( EMBFER_SOURCE_REMAINING(s) ) {
		__embfer_walk_until_embfer_tag( s, output );
		if( !EMBFER_SOURCE_REMAINING(s) )
			break;
		__embfer_walk_until_end_of_embfer_tag( s, output );
	}
	__embfer_delete_source( s );
	flat_output = ferite_buffer_to_str( FE_NoScript, output );
	ferite_buffer_delete( FE_NoScript, output );
	return flat_output;
}

FeriteString *embfer_convert_from_file( const char *path )
{
	FeriteBuffer *buf = NULL;
	FeriteString *source = NULL;
	FeriteString *ret = NULL;
	char rbuf[1024];
	char *rbufp = rbuf;
	FILE *f = fopen( path, "r" );
	
	if( f != NULL )
	{
		buf = ferite_buffer_new( FE_NoScript, 16000 );
		while( !feof( f ) )
		{
			memset( rbufp, '\0', 1024 );
			fgets( rbufp, 1024, f );
			ferite_buffer_add_str( FE_NoScript, buf, rbufp );
		}
		source = ferite_buffer_to_str( FE_NoScript, buf );
		ret = embfer_convert_from_string( source->data );
		
		ferite_buffer_delete( FE_NoScript, buf );
		ferite_str_destroy( FE_NoScript, source );
	}
	return ret;
}

void embfer_register_tag_handler( char *type, EmbferCallback cb )
{
	if( cb != NULL )
		ferite_hash_add( NULL, __embfer_callback_hash, type, cb );
}

