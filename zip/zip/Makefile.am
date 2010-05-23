## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = zip*.h zip*.c zip*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = $(libferite) -L${libdir} @zip_LIBS@
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @zip_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = zip.fec
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = zip.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = zip.la

zip_la_SOURCES    = zip_core.c zip_misc.c zip_Zip.c zip_Zip_zipArchive.c zip_Zip_zipEntry.c zip_header.h  
zip_la_LDFLAGS    = -no-undefined -module -avoid-version
zip_la_LIBADD     =

zip_core.c: 
	@BUILDER@ -m zip @MODULE_SRC_PREFIX@/zip/zip.fec

zip_misc.c zip_Zip.c zip_Zip_zipArchive.c zip_Zip_zipEntry.c zip_header.h : zip_core.c
zip_Zip.c zip_Zip_zipArchive.c zip_Zip_zipEntry.c zip_header.h : zip_core.c
zip_Zip_zipArchive.c zip_Zip_zipEntry.c zip_header.h : zip_core.c
zip_Zip_zipEntry.c zip_header.h : zip_core.c
zip_header.h : zip_core.c
