## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in crypt*.h crypt*.c crypt*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @crypt_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @crypt_CFLAGS@ -I/usr/local/include/ferite
DEFS             = @pthread_defs@

scripts_DATA     = crypt.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = crypt.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = crypt.la

crypt_la_SOURCES    = crypt_core.c crypt_misc.c crypt_Crypt.c crypt_header.h  utility.c utility.h base.c base.h
crypt_la_LDFLAGS    = -no-undefined -module -avoid-version
crypt_la_LIBADD     = -lmcrypt -lmhash

crypt_core.c: @MODULE_SRC_PREFIX@/crypt/crypt.fec
	@BUILDER@ -m crypt @MODULE_SRC_PREFIX@/crypt/crypt.fec
