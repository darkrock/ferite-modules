## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = gettext*.h gettext*.c gettext*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @gettext_LIBS@ $(libferite) 
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @gettext_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = gettext.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = gettext.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = gettext.la

gettext_la_SOURCES    = gettext_core.c gettext_misc.c gettext_Gettext.c gettext_header.h utility.c utility.h
gettext_la_LDFLAGS    = -no-undefined -module -avoid-version
gettext_la_LIBADD     =

gettext_core.c: @MODULE_SRC_PREFIX@/gettext/gettext.fec 
	@BUILDER@ -m gettext @MODULE_SRC_PREFIX@/gettext/gettext.fec

gettext_misc.c: gettext_core.c
gettext_Gettext.c: gettext_core.c
gettext_header.h: gettext_core.c
