bin_PROGRAMS = embfer

embfer_SOURCES = main.c
		    
embfer_DEPENDENCIES = $(top_builddir)/libembfer/libembfer.la
embfer_LDFLAGS = 

INCLUDES = -I. -I$(top_srcdir) -I$(top_srcdir)/src -I..     \
-I$(top_srcdir)/libs/aphex/include -I$(top_srcdir)/include  \
-I$(includedir) -I$(prefix)/include

DEFS=@thread_defs@ @embfer_CFLAGS@

LDADD = $(top_builddir)/libembfer/libembfer.la -L$(libdir) -L$(prefix)/lib

EXTRA_DIST = 
