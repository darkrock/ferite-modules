
#include "ferite.h"
#include "embfer_header.h"

#define EMB_MAX_CLOSURES 32

int emb_closure_count;
EmbferClosureCallback *emb_closurehold[EMB_MAX_CLOSURES];

void __embferclass_tag_handler( FeriteBuffer *output, const char *type, const char *contents )
{
    int i = 0;
    for( i = 0; i < emb_closure_count; i++ )
    {
       if( strcmp( type, emb_closurehold[i]->tag ) == 0)
       {
		   EmbferClosureCallback *cb = emb_closurehold[i];
		   FeriteScript *script = script;
		   FeriteVariable *ret, **params;
		   FeriteString *value;
		   FeriteFunction *f = ferite_object_get_function(script,cb->object,"invoke");
		   
		   if( f != NULL )
		   {
			   params = ferite_create_parameter_list( script, 3 );
			   params[0] = ferite_create_string_variable_from_ptr( script, "", type, 0, FE_CHARSET_DEFAULT, FE_STATIC );
			   MARK_VARIABLE_AS_DISPOSABLE(params[0]);
			   params[1] = ferite_create_string_variable_from_ptr( script, "", contents, 0, FE_CHARSET_DEFAULT, FE_STATIC );
			   MARK_VARIABLE_AS_DISPOSABLE(params[1]);
			   
			   ret = ferite_call_function( script, cb->object, NULL, f, params );
			   
			   ferite_delete_parameter_list(script,params);
			   
			   if( ret != NULL )
			   {
				   value = ferite_variable_to_str(script,ret,FE_FALSE);
				   ferite_variable_destroy(script,ret);
				   
				   ferite_buffer_add_fstr(script, output,value);
				   ferite_str_destroy(script,value);
			   }
		   }
		   break;
       }        
    }         
}
