## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = MHash*.h MHash*.c MHash*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @MHash_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @MHash_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = MHash.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = MHash.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = MHash.la

MHash_la_SOURCES    = MHash_core.c MHash_misc.c MHash_header.h MHash_MHash.c MHash_MHash_Hash.c MHash_MHash_KeyGen.c
MHash_la_LDFLAGS    = -no-undefined -module -avoid-version
MHash_la_LIBADD     =

MHash_core.c: @MODULE_SRC_PREFIX@/MHash/MHash.fec 
	@BUILDER@ -m MHash @MODULE_SRC_PREFIX@/MHash/MHash.fec

MHash_misc.c: MHash_core.c
MHash_header.h: MHash_core.c
MHash_MHash.c: MHash_core.c
MHash_MHash_Hash.c: MHash_core.c
MHash_MHash_KeyGen.c: MHash_core.c
