/*
 * $Date$
 * $Author$
 */
#ifndef _FEOGLU_H_
#define _FEOGLU_H_

#include <ferite.h>
#ifdef MACOSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define NUM2DBL( VAR ) (( VAR->type == F_VAR_DOUBLE ) ? (VAF(VAR)) : (double)(VAI(VAR)) )
#define NUM2LNG( VAR ) (( VAR->type == F_VAR_DOUBLE ) ? (long)(VAF(VAR)) : VAI(VAR) )

#define INT2NUM(V) V
#define ferite_define_final( NS, NAME, VALUE ) do {  \
    FeriteVariable *fv = ferite_create_number_long_variable(script, NAME, VALUE, FE_STATIC); \
    MARK_VARIABLE_AS_FINALSET(fv); \
    ferite_register_ns_variable(script, NS, NAME, fv); \
} while(0)

#endif

