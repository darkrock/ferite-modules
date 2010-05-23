## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES =

CLEANFILES       = 

feincludes_DATA = embfer.h

feincludesdir = $(prefix)/include

EXTRA_DIST = $(feincludes_DATA)

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = $(libferite) -L${libdir} @embfer_LIBS@
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @embfer_CFLAGS@
DEFS             = @thread_defs@

lib_LTLIBRARIES      = libembfer.la

include_HEADERS      =

libembfer_la_SOURCES    = embfer.c  
libembfer_la_LDFLAGS    = -version-info 1:0:0 -export-dynamic
libembfer_la_LIBADD     =

