## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in uxml*.h uxml*.c uxml*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @uxml_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @uxml_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = uxml.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = uxml.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = uxml.la

uxml_la_SOURCES    = uxml_core.c uxml_misc.c uxml_uXML.c uxml_uXML_object.c uxml_header.h  util_uxml.c uxml.h 
uxml_la_LDFLAGS    = -no-undefined -module -avoid-version
uxml_la_LIBADD     =

uxml_core.c: @MODULE_SRC_PREFIX@/uxml/uxml.fec
	@BUILDER@ -m uxml @MODULE_SRC_PREFIX@/uxml/uxml.fec
