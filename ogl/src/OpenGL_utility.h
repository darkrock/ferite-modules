/*
 * $Date$
 * $Author$
 */
#ifndef _FEOGL_H_
#define _FEOGL_H_

#include <ferite.h>

extern FeriteVariable *g_Vertex_ptr;
extern FeriteVariable *g_Normal_ptr;
extern FeriteVariable *g_Color_ptr;
extern FeriteVariable *g_Index_ptr;
extern FeriteVariable *g_TexCoord_ptr;
extern FeriteVariable *g_EdgeFlag_ptr;
extern FeriteVariable *g_current_sel_buffer;
extern FeriteVariable *g_current_feed_buffer;    

extern int ary2cint( FeriteVariable *arg, int cary[], int maxlen);
extern int ary2cflt( FeriteVariable *arg, float cary[], int maxlen);
extern int ary2cdbl( FeriteVariable *arg, double cary[], int maxlen);
extern void mary2ary( FeriteScript *script, FeriteVariable *src, FeriteVariable *ary );
extern void ary2cmat4x4( FeriteVariable *ary, double cary[] );
int glformat_size(GLenum format);
int gltype_size(GLenum type);
FeriteVariable *allocate_buffer_with_string( int size );

#endif

