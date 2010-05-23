dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([mail/mail.fec])
AM_INIT_AUTOMAKE(mail-ferite-module, 0.0.3)
AM_CONFIG_HEADER(config.h)

AC_C_BIGENDIAN	      

AC_PROG_CC
AM_PROG_CC_STDC
AC_C_CONST

AC_PATH_PROG(unamepath, uname)
if test "_$unamepath" = _; then
   system="unknown"
else
   AC_MSG_CHECKING(system type)
   system=`$unamepath -s`
   AC_MSG_RESULT($system)

   cygwin=`$unamepath -s | cut -c 1-5`
   if test "$cygwin" = "CYGWIN"; then
      AC_DEFINE([USING_CYGWIN], 1, [We Are Using CygWin])
          system="CYGWIN"
   fi
fi

AM_CONDITIONAL(NEED_FERITE_LIB, test "$system" = "Darwin")
AM_CONDITIONAL(NEED_FERITE_LIB, test "$system" = "CYGWIN")
AM_CONDITIONAL(NEED_FERITE_LIB, test "$system" = "SunOS")


AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

AM_PATH_FERITE(1.1.1,,[
  echo "You should have ferite 1.1.1 installed for this to work.";
])

if ! test -z "$prefix"; then
   prefix="$FERITE_PREFIX";
   echo "Setting prefix to $FERITE_PREFIX";
fi

CFLAGS="$CFLAGS -I$prefix/include -L$prefix/lib"
CPPFLAGS="$CPPFLAGS -I$prefix/include -L$prefix/lib"
LDFLAGS="$LDFLAGS -L$prefix/lib"

AC_CHECK_HEADER(c-client/c-client.h,,
     AC_MSG_ERROR(c-client header files where not found) )


AC_CHECK_LIB(c-client,create_imap_envelope,,  
     AC_MSG_ERROR(c-client library not found), ) 

sinclude(mail/config.m4)

mail_CFLAGS="$mail_CFLAGS $FERITE_CFLAGS"
mail_LIBS="$mail_LIBS $FERITE_LIBS"

MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

#dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

AC_CONFIG_FILES([
Makefile
mail/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
