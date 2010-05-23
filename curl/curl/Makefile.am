## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in curl*.h curl*.c curl*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @curl_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @curl_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = curl.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = curl.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = curl.la

curl_la_SOURCES    = curl_core.c curl_Curl.c curl_Curl_Curl.c curl_misc.c utility.c curl_header.h utility.h
curl_la_LDFLAGS    = -no-undefined -module -avoid-version
curl_la_LIBADD     =

curl_core.c: @MODULE_SRC_PREFIX@/curl/curl.fec
	@BUILDER@ -m curl @MODULE_SRC_PREFIX@/curl/curl.fec

