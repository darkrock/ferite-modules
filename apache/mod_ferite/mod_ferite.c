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
#include "mod_ferite.h"
#include <time.h>

static void mod_ferite_mark_checkpoint( FeriteScript *script, char *pnt ) {
	FeriteVariable *timeline = ferite_script_fetch_data( script, "TimeLine" );
	double *lasttime = ferite_script_fetch_data( script, "LastTime" );
	char *buf = fcalloc( strlen(pnt) + 128, sizeof(char) );
	struct timeval the_time;
	double t = 0;

	gettimeofday(&the_time, NULL);
	t = (double)the_time.tv_sec + (((double)the_time.tv_usec)/1000000);

	sprintf( buf, "%f,\"%s\" (d: %f)", t, pnt, (t - *lasttime) );
	*lasttime = t;

	printf("%s\n", buf);
	fprintf(stdout, "Out: %s\n", buf);
	fprintf(stderr, "Error: %s\n", buf);
	
	ferite_uarray_push( script, VAUA(timeline), fe_new_str_static("Checkpoint", buf, strlen(buf), FE_CHARSET_DEFAULT) );
	ffree( buf );
}
static int mod_ferite_hook_handler( request_rec *r)
{
	FeriteScript *script = NULL;
	char *args[2], path[PATH_MAX], *p;
	ModFeriteConfig *cfg;

	FE_APACHE_FNC_START;

	if (strcmp(r->handler, "ferite-handler")) 
		return DECLINED;

	cfg = (ModFeriteConfig *) ap_get_module_config(r->per_dir_config, &ferite_module);
	if( cfg && cfg->cache ) {
		ferite_cache_toggle(FE_TRUE);
	}

	if (r->finfo.filetype == 0) {
		return HTTP_NOT_FOUND;
	}

	if (r->header_only) return OK;

	if( !(r->method_number == M_GET ||  r->method_number == M_POST || r->assbackwards) )
	  return DECLINED;

	/* get current working dir */
	p = strrchr( r->filename, '/' );
	apr_snprintf(path,PATH_MAX,"%.*s", (int)(p - r->filename), r->filename);
	args[0] = path;
	args[1] = NULL;

	script = NULL;
#ifdef ENABLE_EMBFER
	if( strcmp( ".fsp", r->filename + (strlen( r->filename ) - 4) ) == 0 )
	{
		FeriteString *str = NULL;
		
		embfer_init(); /* We can call this as much as we want really */
		embfer_set_output_fnc( "Request.current().print" );
		str = embfer_convert_from_file( r->filename );
		if( str != NULL )
		{
			script = mod_ferite_new_script( r );
			mod_ferite_mark_checkpoint( script, "[embfer] Script Created" );
			mod_ferite_mark_checkpoint( script, "[embfer] Start Compliation" );
			script = ferite_compile_string_with_script_and_path( script, str->data, args );
			mod_ferite_mark_checkpoint( script, "[embfer] End Compliation" );
			
			ferite_str_destroy( FE_NoScript, str );
		}
		embfer_deinit(); /* We clear up afterwards */
	}
#endif

	ferite_clear_module_search_list();
	ferite_add_library_search_path( fstrdup(XPLAT_LIBRARY_DIR) );

	if( script == NULL )
	{
		script = mod_ferite_new_script( r );
		mod_ferite_mark_checkpoint( script, "Script Created" );
		mod_ferite_mark_checkpoint( script, "Start Compliation" );
		script = ferite_script_compile_with_script_and_path( script, r->filename, args );
		if( ! ferite_has_compile_error( script ) ) {
			mod_ferite_mark_checkpoint( script, "End Compliation" );
		}
	}
	
	if( script != NULL )
	{
		FeriteVariable *timeline = NULL;
		double *lastime = NULL;
		
		if( ferite_has_compile_error( script ) )
			return mod_ferite_handle_error( script, r, cfg->debug  );
		
		mod_ferite_mark_checkpoint( script, "Creating Global Variables" );
		mod_ferite_create_global_variables(script, cfg);
		mod_ferite_mark_checkpoint( script, "Executing Script" );
		ferite_script_execute( script );
		
		if( ferite_has_runtime_error( script ) )
		  return mod_ferite_handle_error( script, r, cfg->debug );

		timeline = ferite_script_fetch_data( script, "TimeLine" );
		if( timeline )
			ferite_variable_destroy( script, timeline );
		lastime = ferite_script_fetch_data( script, "LastTime");
		if( lastime ) 
			ffree(lastime);
		ferite_script_delete( script );

		return OK;
	}

	FE_APACHE_FNC_FINISH;
	return HTTP_NOT_FOUND;
}

static FeriteScript *mod_ferite_new_script( request_rec *r ) 
{
	FeriteScript *script = NULL;
	FeriteVariable *timeline = NULL;
	double *lasttime = NULL;
	
	FE_APACHE_FNC_START;

	script = ferite_new_script(); 
	timeline = ferite_create_uarray_variable( script, "timeline array", 32, FE_STATIC );
	lasttime = fmalloc(sizeof(double));
	*lasttime = 0.0;
	
	ferite_script_attach_data( script, MF_ATTACH_LINE, (void *)r, NULL );
	ferite_script_attach_data( script, "TimeLine", (void *)timeline, NULL );
	ferite_script_attach_data( script, "LastTime", (void *)lasttime, NULL );
	
	FE_APACHE_FNC_FINISH;
	return script;
}

static int mod_ferite_module_init(apr_pool_t *pconf, apr_pool_t *plog,
							   apr_pool_t *ptemp, server_rec *s)
{
	char **argv = NULL;
	int argc = 0;
	
	FE_APACHE_FNC_START;

#ifdef USE_LIBGC
	argv = malloc(sizeof(char*) * 2);
	argv[0] = "--fe-use-jedi";
	argv[1] = NULL;
	argc = 1;
#endif
	
#ifdef DBUG
	ferite_show_debug = 1;
#endif
	ferite_init( argc, argv );
	ferite_set_library_native_path( fstrdup(NATIVE_LIBRARY_DIR) );
	ferite_module_add_preload( "apache" );

#ifdef ENABLE_EMBFER
	ferite_module_add_preload( "embfer" );
#endif

#ifdef USE_LIBGC
	free(argv);
#endif

	FE_APACHE_FNC_FINISH;
	return OK;
}

static int mod_ferite_sys_deinit(void *data)
{
	FE_APACHE_FNC_START;
	ferite_deinit();
	FE_APACHE_FNC_FINISH;
	return OK;
}

static int mod_ferite_create_global_variables( FeriteScript *script, ModFeriteConfig *cfg )
{
	FE_APACHE_FNC_START;
	
	if(!apr_is_empty_table(cfg->vars))
		apr_table_do(mod_ferite_create_variable, (void *)script, cfg->vars, NULL);

	if( cfg->working_dir )
		mod_ferite_create_variable( script, "FeriteWorkingDirectory", cfg->working_dir );

	FE_APACHE_FNC_FINISH;
	return 0;
}

static int mod_ferite_handle_error( FeriteScript *script, request_rec *r, long debug )
{
	char *errmsg = NULL, *real_errmsg = NULL;

	FE_APACHE_FNC_START;
	
	errmsg = ferite_get_error_log( script );
	if( debug )
	{
		real_errmsg = ferite_replace_string( errmsg, "\n", "<br>\n" );
		ap_rprintf(r,"<html><body><b>Compilation failed:</b> %s</body></html>", real_errmsg);
		ffree( errmsg );
		ffree( real_errmsg );
		ferite_script_delete( script );

		FE_APACHE_FNC_FINISH;
		return OK;
	}
	else
	{
		ap_log_rerror(APLOG_MARK, APLOG_NOERRNO|APLOG_ERR, 0, r, "mod_ferite: %s.", errmsg );
		ffree( errmsg );
		ferite_script_delete( script );
		
		FE_APACHE_FNC_FINISH;
		return HTTP_INTERNAL_SERVER_ERROR;
	}
	FE_APACHE_FNC_FINISH;
}

static int mod_ferite_config_handler(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp,
								   server_rec *s)
{
	FE_APACHE_FNC_START;
	
	ap_add_version_component(p, MFVERSION);
	
	FE_APACHE_FNC_FINISH;
	return OK;
}

static void *mod_ferite_create_dir_config(apr_pool_t *p, char *dir)
{
	ModFeriteConfig *fc = (ModFeriteConfig *)apr_pcalloc(p, sizeof(ModFeriteConfig));
	
	FE_APACHE_FNC_START;
	
	fc->vars = apr_table_make(p, 0);
	fc->debug = 0;
	fc->cache = 0;
	fc->working_dir = (char *)apr_pstrdup(p, "/tmp");
	
	FE_APACHE_FNC_FINISH;
	return (void*)fc;
}

static void *mod_ferite_create_server_config(apr_pool_t *p, server_rec *dir)
{
	FE_APACHE_FNC_START;
	FE_APACHE_FNC_FINISH;
	return apr_table_make(p, 0);
}

static void *mod_ferite_merge_dir_config(apr_pool_t *p, void *basev, void *overridesv)
{
	ModFeriteConfig *newb, *base = (ModFeriteConfig *)basev, *override = (ModFeriteConfig *)overridesv;
	
	FE_APACHE_FNC_START;
	
	newb = (ModFeriteConfig *)apr_pcalloc(p, sizeof(ModFeriteConfig));

	newb->vars =  apr_table_overlay(p,override->vars, base->vars);
	newb->debug = (override->debug) ? override->debug : 0;
	newb->cache = (override->cache) ? override->cache : 0;
	newb->working_dir = (strcmp(override->working_dir, "/tmp")) ? override->working_dir : base->working_dir;

	FE_APACHE_FNC_FINISH;
	
	return newb;
}

static void *mod_ferite_merge_server_config(apr_pool_t *p, void *basev, void *overridesv)
{
	FE_APACHE_FNC_START;
	FE_APACHE_FNC_FINISH;
	return apr_table_overlay(p,overridesv, basev);
}

static const char *mod_ferite_set_debug(cmd_parms *cmd, void *pcfg, const char *value)
{
	ModFeriteConfig *cfg = pcfg;
	char *notused;
	long level;
	
	FE_APACHE_FNC_START;
	
	level = strtol(value, &notused, 10);
	if(level)
		cfg->debug = level;
	
	FE_APACHE_FNC_FINISH;
	return NULL;
}

static const char *mod_ferite_add_working_directory(cmd_parms *cmd, void *pcfg, const char *value)
{
	ModFeriteConfig *cfg = pcfg;
	
	FE_APACHE_FNC_START;
	if( value != NULL )
		cfg->working_dir = (char *)apr_pstrdup(cmd->pool, value);
	
	FE_APACHE_FNC_FINISH;
	return NULL;
}

static const char *mod_ferite_set_cache(cmd_parms *cmd, void *pcfg, int mode)
{
	ModFeriteConfig *cfg = pcfg;

	FE_APACHE_FNC_START;
	if( mode )
		cfg->cache = mode;
	
	FE_APACHE_FNC_FINISH;
	return NULL;
}

static const char *mod_ferite_add_variable(cmd_parms *cmd, void *pcfg, const char *name, const char *value)
{
	ModFeriteConfig *cfg = pcfg;
	
	FE_APACHE_FNC_START;
	apr_table_add(cfg->vars, name, value);
	FE_APACHE_FNC_FINISH;
	
	return NULL;
}

static int mod_ferite_create_variable(void *ptr, const char *name, const char *value )
{
	FeriteVariable *v;
	FeriteScript *script = ptr;
	
	FE_APACHE_FNC_START;
	v = ferite_create_string_variable_from_ptr( script, (char *)name, (char *)value, 0, FE_CHARSET_DEFAULT, FE_ALLOC );
	ferite_register_ns_variable( script, script->mainns, (char*)name, v );
	FE_APACHE_FNC_FINISH;
	
	return 0;
}

void mod_ferite_register_hooks(apr_pool_t *p)
{
	FE_APACHE_FNC_START;
	ap_hook_open_logs(mod_ferite_module_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(mod_ferite_hook_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_post_config(mod_ferite_config_handler, NULL, NULL, APR_HOOK_MIDDLE);
	FE_APACHE_FNC_FINISH;
}

static const command_rec mod_ferite_cmds[] =
{
	AP_INIT_FLAG("FeriteCache", mod_ferite_set_cache, NULL, OR_OPTIONS, "FeriteCache (On|Off) "),
	AP_INIT_TAKE1("FeriteDebug", mod_ferite_set_debug, NULL, OR_OPTIONS, "FeriteDebug <level> "),
	AP_INIT_TAKE2("FeriteSetVar", mod_ferite_add_variable, NULL, OR_OPTIONS, "FeriteSetVar <name> <value>"),
	AP_INIT_TAKE1("FeriteWorkingDirectory", mod_ferite_add_working_directory, NULL, OR_OPTIONS, "FeriteWorkingDirectory <value>"),
	{NULL}
};

module AP_MODULE_DECLARE_DATA ferite_module =
{
	STANDARD20_MODULE_STUFF,
	  mod_ferite_create_dir_config,	 /* dir config creater */
	  mod_ferite_merge_dir_config,	  /* dir merger --- default is to override */
	  mod_ferite_create_server_config,  /* server config */
	  mod_ferite_merge_server_config,   /* merge server config */
	  mod_ferite_cmds,				  /* command table */
	  mod_ferite_register_hooks		 /* register_hooks */
};
