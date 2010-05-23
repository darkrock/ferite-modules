dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([crypt/crypt.fec])
AM_INIT_AUTOMAKE(crypt-ferite-module, 1.1.1)
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

sinclude(crypt/config.m4)


AC_ARG_WITH(mhash,
        [  --with-mhash=DIR         use mhash in <DIR>],
           CPPFLAGS="$CPPFLAGS -I$withval/include"
	   CFLAGS="$CFLAGS -I$withval/include"
	   LD_LIBRAY_PATH="$withval/lib:$LD_LIBRARY_PATH")

AC_ARG_WITH(mcrypt,
        [  --with-mcrypt=DIR         use mcrypt in <DIR>],
           CPPFLAGS="$CPPFLAGS -I$withval/include"
	   CFLAGS="$CFLAGS -I$withval/include"
	   LD_LIBRAY_PATH="$withval/lib:$LD_LIBRARY_PATH")


AC_CHECK_HEADER(mhash.h,,
     AC_MSG_ERROR(header not found) 
)
AC_CHECK_HEADER(mcrypt.h,,
     AC_MSG_ERROR(header not found) 
)

crypt_CFLAGS="$crypt_CFLAGS $CPPFLAGS $FERITE_CFLAGS"
crypt_LIBS="$crypt_LIBS $FERITE_LIBS"


AC_CHECK_LIB(mhash,main,,  
     AC_MSG_ERROR(library not found),  ) 

AC_CHECK_LIB(mcrypt,main,,  
     AC_MSG_ERROR(library not found),  ) 


MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

#dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

AC_CONFIG_FILES([
Makefile
crypt/Makefile
test/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
