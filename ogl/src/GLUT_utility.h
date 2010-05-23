/*
 * $Date$
 * $Author$
 */
#ifndef _FEGLUT_H_
#define _FEGLUT_H_

#include <ferite.h>

typedef struct _glut_menu_pair {
    long menu;
    long parameter;
} GlutMenuPair;

extern FeriteHash *g_menucallback;
extern FeriteHash *g_menuitems;

extern FeriteObject *glut_IdleClosure;
extern FeriteObject *glut_TimerClosure;
extern FeriteObject *glut_MenuStateClosure;

void glut_IdleFuncCallback();
void glut_TimerFuncCallback( int value );
void glut_MenuStateFuncCallback( int state );

#define WINDOW_CALLBACK_INTERFACE( _funcname ) \
extern FeriteHash *_funcname; \
void glut_ ## _funcname ## Callback(); \
FE_NATIVE_FUNCTION( glut_ ## _funcname ## NamespaceFunction );

/* module-init */
#define WINDOW_CALLBACK_DEFINE(NS, FUNCTION) \
    fe_create_ns_fnc( script, NS, # FUNCTION, glut_ ## FUNCTION ## NamespaceFunction, "o" );

/* module-deinit */
#define WINDOW_CALLBACK_CLEANUP(FUNCTION) do { \
    if( FUNCTION != NULL ) { \
        ferite_delete_hash( script, FUNCTION, NULL ); \
    } } while(0) 

char *ferite_glut_int_to_string( int value );
void glut_CreateMenuCallback( int value );
void FeriteGLUTSetup( FeriteScript *script );

WINDOW_CALLBACK_INTERFACE(DisplayFunc);
WINDOW_CALLBACK_INTERFACE(ReshapeFunc);
WINDOW_CALLBACK_INTERFACE(KeyboardFunc);
WINDOW_CALLBACK_INTERFACE(MouseFunc);
WINDOW_CALLBACK_INTERFACE(MotionFunc);
WINDOW_CALLBACK_INTERFACE(PassiveMotionFunc);
WINDOW_CALLBACK_INTERFACE(EntryFunc);
WINDOW_CALLBACK_INTERFACE(VisibilityFunc);
WINDOW_CALLBACK_INTERFACE(SpecialFunc);
WINDOW_CALLBACK_INTERFACE(SpaceballMotionFunc);
WINDOW_CALLBACK_INTERFACE(SpaceballRotateFunc);
WINDOW_CALLBACK_INTERFACE(SpaceballButtonFunc);
WINDOW_CALLBACK_INTERFACE(ButtonBoxFunc);
WINDOW_CALLBACK_INTERFACE(DialsFunc);
WINDOW_CALLBACK_INTERFACE(TabletMotionFunc);
WINDOW_CALLBACK_INTERFACE(TabletButtonFunc);
WINDOW_CALLBACK_INTERFACE(OverlayDisplayFunc);
WINDOW_CALLBACK_INTERFACE(WindowStatusFunc); 

#endif
