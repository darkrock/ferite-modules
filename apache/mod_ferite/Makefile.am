#   the default target
all-local: mod_ferite.so

#   compile the DSO file
mod_ferite.so: mod_ferite.c 
	@APXS@ -c @APXS_DEF@ @APXS_INC@ @APXS_LIB@ mod_ferite.c 

#   install the DSO file into the Apache installation
#   aswell as the configuration file
install-exec-local: all
	mkdir -p $(DESTDIR)`@APXS@ -q LIBEXECDIR`
	cp .libs/mod_ferite.so $(DESTDIR)`@APXS@ -q LIBEXECDIR`
	mkdir -p $(DESTDIR)`@APXS@ -q SYSCONFDIR`
	if ! test -e $(DESTDIR)`@APXS@ -q SYSCONFDIR`"/ferite.conf" ; then \
	   echo "Installing module configuration"; \
	   cat ferite.conf | sed "s#[$$](mod_ferite\.prefix)#"`@APXS@ -q LIBEXECDIR`"#g" > $(DESTDIR)`@APXS@ -q SYSCONFDIR`"/ferite.conf"; \
	fi;

#   cleanup
clean-local:
	-rm -rf mod_ferite.o mod_ferite.so mod_ferite.la \
	mod_ferite.lo mod_ferite.slo .libs 


#   reload the module by installing and restarting Apache
reload: install restart

#   the general Apache start/restart/stop procedures
start:
	@APACHECTL@ start

restart:
	@APACHECTL@ restart

stop:
	@APACHECTL@ stop

debug: stop
	gdb `@APXS@ -q SBINDIR`/`@APXS@ -q TARGET` -q -x gdb 


EXTRA_DIST = gdb ferite.conf mod_ferite.c mod_ferite.h
