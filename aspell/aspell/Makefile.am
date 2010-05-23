## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = aspell*.h aspell*.c aspell*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @aspell_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @aspell_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = aspell.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = aspell.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = aspell.la

aspell_la_SOURCES    = aspell_Aspell.c  aspell_core.c  aspell_header.h  aspell_misc.c  utility.c  utility.h
aspell_la_LDFLAGS    = -no-undefined -module -avoid-version
aspell_la_LIBADD     = -laspell

aspell_core.c: @MODULE_SRC_PREFIX@/aspell/aspell.fec 
	@BUILDER@ -m aspell @MODULE_SRC_PREFIX@/aspell/aspell.fec

aspell_misc.c: aspell_core.c
aspell_Aspell.c: aspell_core.c
aspell_header.h: aspell_core.c
