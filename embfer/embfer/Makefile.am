## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = embfer*.h embfer*.c embfer*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -lferite
endif

LDFLAGS          = -L$(top_builddir)/libembfer -lembfer $(libferite) -L${libdir} @embfer_LIBS@
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. -I$(top_srcdir)/libembfer @embfer_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = embfer.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = embfer.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = embfer.la

embfer_la_SOURCES    = embfer_core.c embfer_misc.c embfer_Embfer.c embfer_header.h utils.c utils.h
embfer_la_LDFLAGS    = -no-undefined -module -avoid-version
embfer_la_LIBADD     = -L$(top_builddir)/libembfer -lembfer

embfer_core.c: @MODULE_SRC_PREFIX@/embfer/embfer.fec  
	@BUILDER@ -m embfer @MODULE_SRC_PREFIX@/embfer/embfer.fec

embfer_misc.c embfer_Ember.c embfer_header.h : embfer_core.c
embfer_Ember.c embfer_header.h : embfer_core.c
embfer_header.h : embfer_core.c
