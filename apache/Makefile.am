## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = apache mod_ferite scripts

CFLAGS_EXTRA = -I$(includedir) -I$(top_srcdir)

EXTRA_DIST = depcomp INSTALL LICENCE farm.yard
install-data-local:
	@$(NORMAL_INSTALL)
	$(prefix)/bin/feritedoc --regenerate ;
