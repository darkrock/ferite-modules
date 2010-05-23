/*
 * $Date$
 * $Author$
 */

#include "utility.h"

/* for OpenGL.fec */
FeriteVariable *g_Vertex_ptr;
FeriteVariable *g_Normal_ptr;
FeriteVariable *g_Color_ptr;
FeriteVariable *g_Index_ptr;
FeriteVariable *g_TexCoord_ptr;
FeriteVariable *g_EdgeFlag_ptr;
FeriteVariable *g_current_sel_buffer;
FeriteVariable *g_current_feed_buffer;    

extern int ary2cint( FeriteVariable *arg, int cary[], int maxlen)
{
    FeriteUnifiedArray* ary = VAUA(arg);
    int i;

    if (maxlen < 1)
	maxlen = ary->size;
    else
	maxlen = maxlen < ary->size ? maxlen : ary->size;
    for (i=0; i < maxlen; i++) {
	cary[i] = NUM2LNG(ary->array[i]);
    }
    return i;
}

extern int ary2cflt( FeriteVariable *arg, float cary[], int maxlen)
{
    FeriteUnifiedArray* ary = VAUA(arg);
    int i;
    
    if (maxlen < 1)
	maxlen = ary->size;
    else
	maxlen = maxlen < ary->size ? maxlen : ary->size;
    for (i=0; i < maxlen; i++) {
	cary[i] = (float)NUM2DBL(ary->array[i]);
    }
    return i;
}

extern int ary2cdbl( FeriteVariable *arg, double cary[], int maxlen)
{
    int i;
    FeriteUnifiedArray* ary = VAUA(arg);
    if (maxlen < 1)
	maxlen = ary->size;
    else
	maxlen = maxlen < ary->size ? maxlen : ary->size;
    for (i=0; i < maxlen; i++) {
	cary[i] = NUM2LNG(ary->array[i]);
    }
    return i;
}

extern void mary2ary( FeriteScript *script, FeriteVariable *src, FeriteVariable *ary )
{
    int i;
    FeriteUnifiedArray *tmp_ary = VAUA(src);
    for (i = 0; i < tmp_ary->size; i++) {
	if( tmp_ary->array[i]->type == F_VAR_UARRAY )
	    mary2ary( script, src, ary);
        else
            ferite_uarray_push(script, VAUA(ary), tmp_ary->array[i]);
    }
}

extern void ary2cmat4x4( FeriteVariable *ary, double cary[] )
{
    int i,j;
    FeriteUnifiedArray *ary_r, *ary_c;
    memset(cary, 0x0, sizeof(double[4*4]));
    ary_c = VAUA(ary);
    if( ary_c->array[0]->type != F_VAR_UARRAY )
	ary2cdbl( ary, cary, 16);
    else {
	for (i = 0; i < ary_c->size && i < 4; i++) {
	    ary_r = VAUA(ary_c->array[i]);
	    for(j = 0; j < ary_r->size && j < 4; j++)
		cary[i*4+j] = (GLdouble)NUM2DBL(ary_r->array[j]);
	}
    }
}

/*Need to find proper size for glReadPixels array*/
int glformat_size(GLenum format) {
    switch(format) {
	case GL_COLOR_INDEX:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_STENCIL_INDEX:
	case GL_DEPTH_COMPONENT:
	case GL_LUMINANCE:
            return 1;
            
	case GL_LUMINANCE_ALPHA:
            return 2;
            
	case GL_RGB:
#ifdef GL_BGR_EXT
	case GL_BGR_EXT:
#endif
            return 3;
            
	case GL_RGBA:
#ifdef GL_BGRA_EXT
	case GL_BGRA_EXT:
#endif
#ifdef GL_ABGR_EXT
	case GL_ABGR_EXT:
#endif
            return 4;
        case 1:
        case 2:
        case 3:
        case 4:
            return format;
	default:
            return -1;
    }
    return -1;
}

int gltype_size(GLenum type) {
    switch(type) {
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
            return 8;
            
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
            return 16;
            
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FLOAT:
            return 32;
            
	case GL_BITMAP:
            return 1;
            
	default:
            return -1;
    }
    return -1;
}

FeriteVariable *allocate_buffer_with_string( int size )
{
   FeriteScript *script = NULL;
   return fe_new_str("", NULL, size, FE_CHARSET_DEFAULT);
}
