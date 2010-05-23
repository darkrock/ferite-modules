## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in apache*.h apache*.c apache*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @apache_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. -I@apachedir@/include @apache_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = apache.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = apache.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = apache.la

apache_la_SOURCES    = apache_core.c apache_misc.c apache_Request.c apache_Metrics.c apache_header.h util_qp.c util_multipart.c multipart.c multipart.h utility.h util_qparser.c
apache_la_LDFLAGS    = -module -avoid-version
apache_la_LIBADD     =

apache_core.c: @MODULE_SRC_PREFIX@/apache/apache.fec
	@BUILDER@ -m apache @MODULE_SRC_PREFIX@/apache/apache.fec
