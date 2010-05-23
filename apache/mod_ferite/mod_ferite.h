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

#ifdef ENABLE_EMBFER
#include <embfer.h>
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_main.h"
#include "ap_mpm.h"
#include "ap_config.h"
#include "scoreboard.h"
#include "http_log.h"
#include "limits.h"
#include "apr.h"
#include "apr_strings.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef DBUG 
#define MF_DEBUG( d ) fprintf d;
#define FE_APACHE_FNC_START fprintf( stderr, "mod_ferite -> %s start\n", __FUNCTION__ )
#define FE_APACHE_FNC_FINISH fprintf( stderr, "mod_ferite -> %s finish\n", __FUNCTION__ )
#else
#define MF_DEBUG( d )
#define FE_APACHE_FNC_START
#define FE_APACHE_FNC_FINISH
#endif

#ifdef USE_LIBGC
#   warning Building anti-libgc support
#	define MFVERSION "mod_ferite/1.1.13c (ferite v" FERITE_VERSION "/libgc)"
#else
#	define MFVERSION "mod_ferite/1.1.13c (ferite v" FERITE_VERSION ")"
#endif

#define MF_ATTACH_LINE "__mod_ferite_request_rec__"

typedef struct mf_config {
    long debug;
    int cache;
    char *working_dir;
    apr_table_t *vars;
} ModFeriteConfig;

static void mod_ferite_register_hooks(apr_pool_t *);
static int mod_ferite_config_handler(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);
static void *mod_ferite_create_dir_config(apr_pool_t *p, char *dir);
static void *mod_ferite_merge_dir_config(apr_pool_t *p, void *basev, void *overridesv);
static const char *mod_ferite_set_debug(cmd_parms *cmd, void *pcfg, const char *value);
static const char *mod_ferite_add_variable(cmd_parms *cmd, void *pcfg, const char *name, const char *value);
static int mod_ferite_create_variable(void *ptr, const char *name, const char *value );
static const char *mod_ferite_add_working_directory(cmd_parms *cmd, void *pcfg, const char *value);
static int mod_ferite_handle_error( FeriteScript *script, request_rec *r, long debug );
static int mod_ferite_create_global_variables( FeriteScript *script, ModFeriteConfig *cfg );
static void *mod_ferite_create_server_config(apr_pool_t *p, server_rec *dir);
static void *mod_ferite_merge_server_config(apr_pool_t *p, void *basev, void *overridesv);
static int mod_ferite_sys_deinit(void *data);
static int mod_ferite_module_init(apr_pool_t *pconf, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s);
static FeriteScript *mod_ferite_new_script( request_rec *r );

static FeriteHash *mod_ferite_cache = NULL;

extern FeriteStack *ferite_module_search_list;
module AP_MODULE_DECLARE_DATA ferite_module;
