## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in dbi*.h dbi*.c dbi*~

CLEANFILES       = 


if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif


LDFLAGS          = -L${libdir} @dbi_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @dbi_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = dbi.fec DbiResult.fec dbi_sql.fe
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = dbi.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = dbi.la

dbi_la_SOURCES    = dbi_core.c dbi_misc.c dbi_Dbi.c dbi_DbiResult.c dbi_DbiObject.c dbi_header.h utility.c utility.h 
dbi_la_LDFLAGS    = -no-undefined -module -avoid-version
dbi_la_LIBADD     = -ldbi

dbi_core.c: @MODULE_SRC_PREFIX@/dbi/dbi.fec @MODULE_SRC_PREFIX@/dbi/DbiResult.fec
	@BUILDER@ -m dbi @MODULE_SRC_PREFIX@/dbi/dbi.fec
