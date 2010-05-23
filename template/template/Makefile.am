## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in template*.h template*.c template*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @template_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @template_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = template.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = template.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = template.la

template_la_SOURCES    = template_core.c template_misc.c template_Template.c template_header.h  utility.c utility.h 
template_la_LDFLAGS    = -no-undefined -module -avoid-version
template_la_LIBADD     =

template_core.c: @MODULE_SRC_PREFIX@/template/template.fec
	@BUILDER@ -m template @MODULE_SRC_PREFIX@/template/template.fec

