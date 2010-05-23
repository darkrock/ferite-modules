## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in zlib*.h zlib*.c zlib*~

CLEANFILES       =


if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif


LDFLAGS          = -L${libdir} @zlib_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @zlib_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = zlib.fec
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = zlib.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = zlib.la

zlib_la_SOURCES    = zlib_core.c zlib_misc.c zlib_ZLib_ZLib.c zlib_ZLib.c zlib_header.h utility.c
zlib_la_LDFLAGS    = -no-undefined -module -avoid-version
zlib_la_LIBADD     =

zlib_core.c: @MODULE_SRC_PREFIX@/zlib/zlib.fec
	@BUILDER@ -m zlib @MODULE_SRC_PREFIX@/zlib/zlib.fec
