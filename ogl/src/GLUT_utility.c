/*
 * $Date$
 * $Author$
 */

#include "utility.h"

typedef struct _glut_menu_pair {
    long menu;
    long parameter;
} GlutMenuPair;

#define WINDOW_CALLBACK_HASH(_funcname) \
FeriteHash *_funcname = NULL;

/* callback define macro */
#define WINDOW_CALLBACK_IMPLEMENTATION(_funcname) \
FE_NATIVE_FUNCTION( glut_ ## _funcname ## NamespaceFunction ) \
{ \
    int win = glutGetWindow(); \
    char *winid = ferite_glut_int_to_string(win); \
    FeriteObject *closure = NULL; \
    if( _funcname == NULL ) { _funcname = ferite_create_hash( script, 8 ); } \
    closure = ferite_hash_get( script, _funcname, winid ); \
    if( closure != NULL ) { closure->refcount--; } \
    ferite_hash_add( script, _funcname, winid, VAO(params[0]) ); \
	if( VAO(params[0]) != NULL ) { VAO(params[0])->refcount++; } \
    glut ## _funcname(glut_ ## _funcname ## Callback); \
	FE_RETURN_VOID; \
}

WINDOW_CALLBACK_HASH(DisplayFunc);
WINDOW_CALLBACK_HASH(ReshapeFunc);
WINDOW_CALLBACK_HASH(KeyboardFunc);
WINDOW_CALLBACK_HASH(MouseFunc);
WINDOW_CALLBACK_HASH(MotionFunc);
WINDOW_CALLBACK_HASH(PassiveMotionFunc);
WINDOW_CALLBACK_HASH(EntryFunc);
WINDOW_CALLBACK_HASH(VisibilityFunc);
WINDOW_CALLBACK_HASH(SpecialFunc);
WINDOW_CALLBACK_HASH(SpaceballMotionFunc);
WINDOW_CALLBACK_HASH(SpaceballRotateFunc);
WINDOW_CALLBACK_HASH(SpaceballButtonFunc);
WINDOW_CALLBACK_HASH(ButtonBoxFunc);
WINDOW_CALLBACK_HASH(DialsFunc);
WINDOW_CALLBACK_HASH(TabletMotionFunc);
WINDOW_CALLBACK_HASH(TabletButtonFunc);
WINDOW_CALLBACK_HASH(OverlayDisplayFunc);
WINDOW_CALLBACK_HASH(WindowStatusFunc); 

FeriteScript   *g_script = NULL;

/* GLUT menu sub-API. */
FeriteHash *g_menucallback = NULL;
FeriteHash *g_menuitems = NULL;

char *ferite_glut_int_to_string( int value ) 
{
    static char buf[32];
    char *ret = buf;
    
    memset( ret, 0, 32 );
    sprintf( ret, "%d", value );
    return ret;
}

void glut_CreateMenuCallback( int value )
{
    GlutMenuPair    *gmp = (GlutMenuPair*)value;    
    FeriteObject    *closure = ferite_hash_get( g_script, g_menucallback, ferite_glut_int_to_string( gmp->menu ) );
    FeriteVariable **params = ferite_create_parameter_list_from_data( g_script, "l", gmp->parameter );
    FeriteFunction  *invoke = ferite_object_get_function_for_params( g_script, closure, "invoke", params );
    
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
}

void FeriteGLUTSetup( FeriteScript *script )
{
    g_script = script;
    g_menucallback = ferite_create_hash( script, 32 );
    g_menuitems = ferite_create_hash( script, 32 );
}

/* GLUT  sub-API. */
#define GET_INVOKE( HASH ) \
FeriteVariable **params = NULL; \
FeriteObject *closure = ferite_hash_get( g_script, HASH, ferite_glut_int_to_string(glutGetWindow())); \
FeriteFunction *invoke = ferite_object_get_function( g_script, closure, "invoke" ); \
if( invoke == NULL ) return;

#define CLEAN_INVOKE() \
if( ferite_has_runtime_error( g_script ) ) { \
    char *errmsg = ferite_get_error_log( g_script ); \
    fprintf( stderr, "[ferite: execution]\n%s", errmsg ); \
    (ferite_free)( errmsg, __FILE__, __LINE__, g_script ); \
    exit(1); \
}

void glut_DisplayFuncCallback(void)
{
    GET_INVOKE( DisplayFunc );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    CLEAN_INVOKE();
}

void glut_ReshapeFuncCallback( int width, int height )
{
    GET_INVOKE( ReshapeFunc );    
    params = ferite_create_parameter_list_from_data( g_script, "ll", width, height );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_KeyboardFuncCallback( unsigned char key, int x, int y ) 
{
    GET_INVOKE( KeyboardFunc );
    params = ferite_create_parameter_list_from_data( g_script, "lll", key, x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_MouseFuncCallback( int button, int state, int x, int y )
{
    GET_INVOKE( MouseFunc );
    params = ferite_create_parameter_list_from_data( g_script, "llll", button, state, x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_MotionFuncCallback( int x, int y )
{
    GET_INVOKE( MotionFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_PassiveMotionFuncCallback( int x, int y )
{
    GET_INVOKE( PassiveMotionFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_EntryFuncCallback( int state )
{
    GET_INVOKE( EntryFunc );
    params = ferite_create_parameter_list_from_data( g_script, "l", state );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

void glut_VisibilityFuncCallback( int state )
{
    GET_INVOKE( VisibilityFunc );
    params = ferite_create_parameter_list_from_data( g_script, "l", state );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}

FeriteObject *glut_IdleClosure;
FeriteObject *glut_TimerClosure;
FeriteObject *glut_MenuStateClosure;

void glut_IdleFuncCallback()
{
    if( glut_IdleClosure != NULL ) {
        FeriteFunction *invoke = ferite_object_get_function( g_script, glut_IdleClosure, "invoke" );
        if( invoke != NULL ) {
            ferite_variable_destroy( g_script, ferite_call_function( g_script, glut_IdleClosure, NULL, invoke, NULL ) );
            CLEAN_INVOKE();
        }
    }
}

void glut_TimerFuncCallback( int value )
{
    if( glut_TimerClosure != NULL ) {
        FeriteFunction *invoke = ferite_object_get_function( g_script, glut_TimerClosure, "invoke" );
        if( invoke != NULL ) {
            FeriteVariable **params = ferite_create_parameter_list_from_data( g_script, "l", value );
            ferite_variable_destroy( g_script, ferite_call_function( g_script, glut_TimerClosure, NULL, invoke, params ) );
            ferite_delete_parameter_list( g_script, params );
            CLEAN_INVOKE();
        }
    }
}

void glut_MenuStateFuncCallback( int state )
{
    if( glut_MenuStateClosure != NULL ) {
        FeriteFunction *invoke = ferite_object_get_function( g_script, glut_MenuStateClosure, "invoke" );
        if( invoke != NULL ) {
            FeriteVariable **params = ferite_create_parameter_list_from_data( g_script, "l", state );
            ferite_variable_destroy( g_script, ferite_call_function( g_script, glut_MenuStateClosure, NULL, invoke, params ) );
            ferite_delete_parameter_list( g_script, params );
        }
        CLEAN_INVOKE();
    }
}

#if (GLUT_API_VERSION >= 2)
void glut_SpecialFuncCallback( int key, int x, int y )
{
    GET_INVOKE( SpecialFunc );
    params = ferite_create_parameter_list_from_data( g_script, "lll", key, x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_SpaceballMotionFuncCallback( int x, int y, int z )
{
    GET_INVOKE( SpaceballMotionFunc );
    params = ferite_create_parameter_list_from_data( g_script, "lll", x, y, z );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_SpaceballRotateFuncCallback( int x, int y, int z )
{
    GET_INVOKE( SpaceballRotateFunc );
    params = ferite_create_parameter_list_from_data( g_script, "lll", x, y, z );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_SpaceballButtonFuncCallback( int button, int state )
{
    GET_INVOKE( SpaceballButtonFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", button, state );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_ButtonBoxFuncCallback( int button, int state )
{
    GET_INVOKE( ButtonBoxFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", button, state );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_DialsFuncCallback( int dial, int value )
{
    GET_INVOKE( DialsFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", dial, value );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_TabletMotionFuncCallback( int x, int y )
{
    GET_INVOKE( TabletMotionFunc );
    params = ferite_create_parameter_list_from_data( g_script, "ll", x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
void glut_TabletButtonFuncCallback( int button, int state, int x, int y )
{
    GET_INVOKE( TabletButtonFunc );
    params = ferite_create_parameter_list_from_data( g_script, "llll", button, state, x, y );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
#if (GLUT_API_VERSION >= 3)
void glut_OverlayDisplayFuncCallback()
{
    GET_INVOKE( OverlayDisplayFunc );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    CLEAN_INVOKE();
}
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
void glut_WindowStatusFuncCallback( int state )
{
    GET_INVOKE( WindowStatusFunc );
    params = ferite_create_parameter_list_from_data( g_script, "l", state );
    ferite_variable_destroy( g_script, ferite_call_function( g_script, closure, NULL, invoke, params ) );
    ferite_delete_parameter_list( g_script, params );
    CLEAN_INVOKE();
}
#endif
#endif
#endif

WINDOW_CALLBACK_IMPLEMENTATION(DisplayFunc);
WINDOW_CALLBACK_IMPLEMENTATION(ReshapeFunc);
WINDOW_CALLBACK_IMPLEMENTATION(KeyboardFunc);
WINDOW_CALLBACK_IMPLEMENTATION(MouseFunc);
WINDOW_CALLBACK_IMPLEMENTATION(MotionFunc);
WINDOW_CALLBACK_IMPLEMENTATION(PassiveMotionFunc);
WINDOW_CALLBACK_IMPLEMENTATION(EntryFunc);
WINDOW_CALLBACK_IMPLEMENTATION(VisibilityFunc);
WINDOW_CALLBACK_IMPLEMENTATION(SpecialFunc);
WINDOW_CALLBACK_IMPLEMENTATION(SpaceballMotionFunc);
WINDOW_CALLBACK_IMPLEMENTATION(SpaceballRotateFunc);
WINDOW_CALLBACK_IMPLEMENTATION(SpaceballButtonFunc);
WINDOW_CALLBACK_IMPLEMENTATION(ButtonBoxFunc);
WINDOW_CALLBACK_IMPLEMENTATION(DialsFunc);
WINDOW_CALLBACK_IMPLEMENTATION(TabletMotionFunc);
WINDOW_CALLBACK_IMPLEMENTATION(TabletButtonFunc);
WINDOW_CALLBACK_IMPLEMENTATION(OverlayDisplayFunc);
WINDOW_CALLBACK_IMPLEMENTATION(WindowStatusFunc); 
