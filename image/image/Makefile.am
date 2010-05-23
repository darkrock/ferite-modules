## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in image*.h image*.c image*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L$(prefix)/lib -lferite
endif

LDFLAGS          = $(libferite) -L${libdir} -L/usr/X11R6/lib @image_LIBS@
INCLUDES         = $(libferite) -I$(top_srcdir)/include -I$(prefix)/include -I. @image_CFLAGS@ 
DEFS             = @thread_defs@ -DX_DISPLAY_MISSING

scripts_DATA     = image.fec ImageClass.fec Polygon.fec Font.fec Fontdir.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = image.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = image.la

image_la_SOURCES    = image_core.c image_misc.c image_header.h image_Image.c image_Point.c image_Polygon.c image_Color.c image_Font.c image_Fontdir.c utility.c utility.h image_Size.c image_Rect.c
image_la_LDFLAGS    = -no-undefined -module -avoid-version
image_la_LIBADD     = @image_LIBS@

image_core.c: @MODULE_SRC_PREFIX@/image/image.fec @MODULE_SRC_PREFIX@/image/ImageClass.fec @MODULE_SRC_PREFIX@/image/Polygon.fec @MODULE_SRC_PREFIX@/image/Font.fec @MODULE_SRC_PREFIX@/image/Fontdir.fec 
	@BUILDER@ -m image @MODULE_SRC_PREFIX@/image/image.fec
