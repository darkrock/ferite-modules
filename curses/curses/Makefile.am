## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = curses*.h curses*.c curses*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @curses_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @curses_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = curses.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = curses.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = curses.la

curses_la_SOURCES    = curses_core.c curses_misc.c curses_header.h curses_Curses.c curses_Curses_Window.c curses_Curses_Pad.c
curses_la_LDFLAGS    = -no-undefined -module -avoid-version
curses_la_LIBADD     =

curses_core.c: @MODULE_SRC_PREFIX@/curses/curses.fec 
	@BUILDER@ -m curses @MODULE_SRC_PREFIX@/curses/curses.fec

curses_misc.c: curses_core.c
curses_header.h: curses_core.c
curses_Curses.c: curses_core.c
curses_Curses_Window.c: curses_core.c
curses_Curses_Pad.c: curses_core.c
