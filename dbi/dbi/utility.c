/*
 * Copyright (C) 2002-2007 Christian M. Stamgren, Stephan Engström, Chris Ross
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


DBIData *ferite_dbi_connect( FeriteScript *script, FeriteString *protocol,
	                     FeriteString *username, FeriteString *password,
	                     FeriteString *hostname, int port, 
                             FeriteString *database )
{
    
    DBIData *dbi = fmalloc( sizeof( DBIData ) );
    dbi->connected = 0;
    dbi->conn = NULL;
    dbi->version = dbi_version( );
    dbi->result_list = NULL;
    
    if( ( dbi->conn = dbi_conn_new( protocol->data ) ) == NULL ) {
		ferite_error(script, -1, "Failed to activate driver [%s] libdbi version: %s\n",
					   protocol->data, dbi->version ); 
					   ffree( dbi );
					   return NULL;
    }

    dbi_conn_set_option( dbi->conn, "host", hostname->data );
    dbi_conn_set_option( dbi->conn, "username", username->data );
    dbi_conn_set_option( dbi->conn, "password", password->data );
    dbi_conn_set_option( dbi->conn, "dbname", database->data );
    
    if( port != 0 ) 
          dbi_conn_set_option_numeric( dbi->conn, "port", port );

    if ( dbi_conn_connect( dbi->conn ) != 0 ) {
		const char *error_msg = NULL;
		dbi_conn_error( dbi->conn, &error_msg );
		ferite_error( script, 0, "Unable to connect to database: '%s'\n", error_msg );
		ffree( dbi );
		return NULL;
    }
    dbi->connected = 1;
    return dbi;
}

int ferite_add_result_list( DBIData *self_struct, RESData *obj)
{
    struct llist *tmp = fcalloc_ngc(1,sizeof(struct llist));
    if( tmp == NULL ) 
		return 1;
    
    tmp->result = obj->result;	
    tmp->next = self_struct->result_list;
    self_struct->result_list = tmp;
    return 0;
}	

int ferite_remove_from_result_list( RESData *obj )
{
    DBIData *data = obj->dbi;
    struct llist *previous = NULL, *curr = data->result_list;
    
    if( obj == NULL && obj->result == NULL ) 
	    return -1;
    
    while( curr ) {
	
	    if( curr->result  ) {
		    if( curr->result == obj->result) {
	
			    dbi_result_free( obj->result );
			    obj->result = NULL;
			    
			    if( previous == NULL ) 
				    data->result_list = curr->next;
			    else 
				    previous->next = curr->next;	
			    ffree_ngc( curr );
			    return 1;
		    }
	    } 
	    previous = curr;
	    curr = curr->next;
    }
    return 0;
}	



void ferite_deinit_result_list(struct llist *root) 
{
    struct llist *curr = root;
    
    while( curr ) {
		root = curr;
		curr = curr->next;
		if(  root->result ) {
			dbi_result_free( root->result );
			root->result = NULL;
		}
		if( root ) {
			ffree_ngc(root);
			root = NULL;
		}
    }
}


char *ferite_quote_internal( char *str ) 
{
    char *new = NULL, *orig = NULL, *dest = NULL, *end = NULL, c;
    int  len = strlen( str );
    new = fmalloc_ngc( ( len * 2 ) + 1 ); 
    
    for ( orig = str, end = (orig + len), dest = new; ( c = *orig ) || orig < end; orig++ ) {
		if (c == '\'' || c == '\\' || c == '"')  
			*dest++ = '\\';
		*dest++ = c;
    }
    *dest = '\0'; 
    return new;
}


FeriteVariable *ferite_return_result( FeriteScript *script, dbi_result result )     
{
    FeriteVariable *obj = NULL;
    FeriteClass *cls = NULL;
    
    if((cls = ferite_find_class( script, script->mainns, "DbiResult" )) == NULL) {
		ferite_error( script , 0, "Can't locate DbiResult" ); 
		FE_RETURN_NULL_OBJECT;
    }
	
    obj = ferite_new_object( script, cls, NULL); 
    ((RESData *)VAO(obj)->odata)->result = result;
    FE_RETURN_VAR( obj );	
}


FeriteVariable *ferite_dbi_get_entry( FeriteScript *script, dbi_result result, int col ) 
{	
    FeriteVariable *value = NULL;
    char *name = NULL, *stringcol = NULL; 
    long longcol;
    float floatcol;
    double doublecol;
    time_t loctime;
    struct  tm *time;
    unsigned long attribs;
    name = (char *) dbi_result_get_field_name( result, col );
    
    switch( dbi_result_get_field_type_idx( result, col ) ) {
	    
		case DBI_TYPE_INTEGER:
			
			attribs = dbi_result_get_field_attribs_idx(result, col);
			
			switch( attribs &  ~DBI_INTEGER_UNSIGNED ) {
				
				case DBI_INTEGER_SIZE1:
					longcol = ( attribs & DBI_INTEGER_UNSIGNED ) ? dbi_result_get_uchar( result, name) : dbi_result_get_char( result, name);
					break;
				case DBI_INTEGER_SIZE2:
					longcol = ( attribs & DBI_INTEGER_UNSIGNED ) ? dbi_result_get_ushort( result, name) : dbi_result_get_short( result, name);
					break;
				case DBI_INTEGER_SIZE3:
				case DBI_INTEGER_SIZE4:
					longcol = ( attribs & DBI_INTEGER_UNSIGNED ) ? dbi_result_get_uint( result, name) : dbi_result_get_int( result, name);
					break;
				case DBI_INTEGER_SIZE8:
					longcol = ( attribs & DBI_INTEGER_UNSIGNED ) ? dbi_result_get_ulonglong( result, name) : dbi_result_get_longlong( result, name);
					break;
				default:
					ferite_error( script, 0, "Unimplemented. Integer type." );
					break;
			}
				
			value = fe_new_lng( name,  longcol );
			break;
			
		case DBI_TYPE_DECIMAL:
			
			switch(dbi_result_get_field_attribs_idx(result, col)) { 
				case DBI_DECIMAL_SIZE4:
					floatcol = dbi_result_get_float( result, name);
					value = fe_new_dbl(name, floatcol);
					break;
				case DBI_DECIMAL_SIZE8:
					doublecol = dbi_result_get_double( result, name);
					value = fe_new_dbl(name, doublecol);
					break;
				default:
					ferite_error( script, 0, "Unimplemented. Decimal type.\n");
					break;
			}
			break;
					
		case DBI_TYPE_STRING:
			stringcol = (char *) dbi_result_get_string( result, name );
			value = fe_new_str( name, stringcol, 0, FE_CHARSET_DEFAULT ); 
			break;
		
		case DBI_TYPE_DATETIME:
			stringcol = fcalloc(21, sizeof( char ) );
			loctime = dbi_result_get_datetime( result, name);
			time =  localtime(&loctime); 
			
			switch( dbi_result_get_field_attribs_idx( result, col ) ) { 
				
				case DBI_TYPE_STRING:
					strftime(stringcol, 20, "%Y-%m-%d %H:%M:%S", time); 
					break;
					
				case DBI_DATETIME_DATE: 
					strftime(stringcol, 20, "%Y-%m-%d", time);
					break;
					
				case DBI_DATETIME_TIME:
					strftime(stringcol, 20, "%H:%M:%S", time);
					break;
					
				default:
					ferite_error( script, 0, "Unimplemented. String type.\n");
					break;
					
			} 
				
			value = fe_new_str( name, stringcol, 0, FE_CHARSET_DEFAULT );
			ffree(stringcol);
			break;
	
		case DBI_TYPE_BINARY:
			stringcol = (char *) dbi_result_get_binary( result, name);
			longcol = dbi_result_get_field_length( result, name );
			value = fe_new_str( name, stringcol, longcol, FE_CHARSET_DEFAULT );
			break;
			
		default:
			ferite_error( script, 0, "Unimplemented. Type.\n" );
			break;			
    }
    return ( value );
}
 
FeriteVariable *ferite_dbi_get_next_row( FeriteScript *script, RESData *res ) {
    FeriteVariable *value = NULL, *row = NULL;
    unsigned long i,cols;

    row = ferite_create_uarray_variable( script, "dbi_next_record", FE_ARRAY_DEFAULT_SIZE, FE_STATIC);
    if( res->result && dbi_result_next_row( res->result ) ) {
		cols = dbi_result_get_numfields( res->result );
		for( i = 1; i <= cols; i++ ) {
		    if( value = (FeriteVariable *) ferite_dbi_get_entry( script, res->result, i ) )
				ferite_uarray_add( script, VAUA( row ), value, value->vname, FE_ARRAY_ADD_AT_END ); 
		}
    }
	return row;
}

double ferite_dbi_timestamp() {
	struct timeval the_time;
	double t = 0;
	gettimeofday(&the_time, NULL);
	t = (double)the_time.tv_sec + (((double)the_time.tv_usec)/1000000);
	return t;
}

void ferite_dbi_recordQuery( FeriteScript *script, FeriteObject *self, double beforeQuery, char *query ) {
	double afterQuery = ferite_dbi_timestamp();
	FeriteFunction *recordFunction = ferite_object_get_function( script, self, "recordQuery" );
	FeriteVariable **plist = ferite_create_parameter_list( script, 4 );
	FeriteVariable *returnValue = NULL;
	
	plist[0] = ferite_create_number_double_variable( script, "beforeQuery", beforeQuery, FE_STATIC );
	MARK_VARIABLE_AS_DISPOSABLE(plist[0]);
	plist[1] = ferite_create_number_double_variable( script, "afterQuery", afterQuery, FE_STATIC );
	MARK_VARIABLE_AS_DISPOSABLE(plist[1]);
	plist[2] = ferite_create_string_variable_from_ptr( script, "query", query, strlen(query), FE_CHARSET_DEFAULT, FE_STATIC );
	MARK_VARIABLE_AS_DISPOSABLE(plist[2]);
	plist[3] = NULL;
	
	returnValue = ferite_call_function( script, self, NULL, recordFunction, plist );
	
	ferite_delete_parameter_list( script, plist );
	ferite_variable_destroy( script, returnValue );
}