## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = serial*.h serial*.c serial*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @serial_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I. @serial_CFLAGS@
DEFS             = @thread_defs@

scripts_DATA     = serial.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = serial.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = serial.la

serial_la_SOURCES    = serial_core.c serial_misc.c serial_header.h serial_Serial.c serial_Serial_Port.c util.c
serial_la_LDFLAGS    = -no-undefined -module -avoid-version
serial_la_LIBADD     =

serial_core.c: @MODULE_SRC_PREFIX@/serial/serial.fec 
	@BUILDER@ -m serial @MODULE_SRC_PREFIX@/serial/serial.fec

serial_misc.c: serial_core.c
serial_header.h: serial_core.c
serial_Serial.c: serial_core.c
serial_Serial_Port.c: serial_core.c
util.c: serial_core.c
