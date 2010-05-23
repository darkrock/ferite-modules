## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = xslt*.h xslt*.c xslt*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = $(libferite) -L${libdir} @xslt_LIBS@
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @xslt_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = xslt.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = xslt.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = xslt.la

xslt_la_SOURCES    = xslt_core.c xslt_misc.c xslt_header.h xslt_XSLT.c xslt_XSLT_Transform.c 
xslt_la_LDFLAGS    = -no-undefined -module -avoid-version
xslt_la_LIBADD     =

xslt_core.c: @MODULE_SRC_PREFIX@/xslt/xslt.fec  
	@BUILDER@ -m xslt @MODULE_SRC_PREFIX@/xslt/xslt.fec

xslt_misc.c xslt_header.h : xslt_core.c
xslt_header.h : xslt_core.c
