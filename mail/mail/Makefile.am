## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in mail*.h mail*.c mail*~

CLEANFILES       = 

if NEED_FERITE_LIB
libferite = -L${top_builddir}/src -lferite
endif

LDFLAGS          = -L${libdir} @mail_LIBS@ $(libferite)
INCLUDES         = -I$(top_srcdir)/include -I$(prefix)/include -I$(prefix)/include/imap -I. @mail_CFLAGS@
DEFS             = @pthread_defs@

scripts_DATA     = mail.fec Mime.fec 
scriptsdir       = @FE_XPLAT_LIBRARY_PATH@

modxml_DATA      = mail.xml
modxmldir        = @FE_LIBRARY_PATH@/module-descriptions

EXTRA_DIST       = $(scripts_DATA) $(modxml_DATA)
pkgdir           = @FE_NATIVE_LIBRARY_PATH@
pkg_LTLIBRARIES  = mail.la

mail_la_SOURCES    = imap_utility.c  mail_Mail_AddressList.c mail_Mail_Mailobj.c mail_Mail_address.c mail_misc.c  mail_Encoding.c mail_Mail_Connection.c mail_Mail_Multipart.c mail_Mime.c mail_Mail.c mail_Mail_MailHeader.c mail_Mail_Part.c       mail_core.c


mail_la_LDFLAGS    = -no-undefined -module -avoid-version
mail_la_LIBADD     = -lc-client -lcrypt

mail_core.c: @MODULE_SRC_PREFIX@/mail/mail.fec
	@BUILDER@ -m mail @MODULE_SRC_PREFIX@/mail/mail.fec
