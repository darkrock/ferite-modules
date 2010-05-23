dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([zlib/zlib.fec])
AM_INIT_AUTOMAKE(zlib-ferite-module, 1.1.0)
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

sinclude(zlib/config.m4)

MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

AM_PATH_FERITE(1.1.1,,
   AC_MSG_ERROR(You should have ferite 1.1.1 installed for this to work.)
)

if ! test -z "$prefix"; then
   prefix="$FERITE_PREFIX";
   echo "Setting prefix to $FERITE_PREFIX";
fi

CFLAGS="$CFLAGS -I$prefix/include"
CPPFLAGS="$CPPFLAGS -I$prefix/include"
LDFLAGS="$LDFLAGS -L$prefix/lib"

dnl Checks for zlib library.
AC_CHECK_HEADER(zlib.h,,
     AC_MSG_ERROR(zlib headers not found. Use --with-zlib-incdir=) )
dnl assume that if we find the header we will find the library
ZLIB_HOME=/usr
if test ! -f "${ZLIB_HOME}/include/zlib.h"
then
  ZLIB_HOME=/usr/local
  if test ! -f "${ZLIB_HOME}/include/zlib.h"
  then
    ZLIB_HOME=
    AC_MSG_RESULT(failed)
    AC_MSG_ERROR(bad zlib hack)
  fi
fi
ZLIB_OLD_LDFLAGS=$LDFLAGS
ZLIB_OLD_CPPFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
AC_LANG_SAVE
AC_LANG_C
AC_CHECK_LIB(z, inflateEnd, [zlib_cv_libz=yes], [zlib_cv_libz=no])
AC_CHECK_HEADER(zlib.h, [zlib_cv_zlib_h=yes], [zlib_cv_zlib_h=no])
AC_LANG_RESTORE
if test "$zlib_cv_libz" = "yes" -a "$zlib_cv_zlib_h" = "yes"
then
  #
  # If both library and header were found, use them
  #
  AC_CHECK_LIB(z, inflateEnd)
  AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
  AC_MSG_RESULT(ok)
else
  #
  # If either header or library was not found, revert and bomb
  #
  AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
  LDFLAGS="$ZLIB_OLD_LDFLAGS"
  CPPFLAGS="$ZLIB_OLD_CPPFLAGS"
  AC_MSG_RESULT(failed)
  AC_MSG_ERROR(specify a valid zlib installation with --with-zlib=DIR)
fi

dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

AC_CONFIG_FILES([
Makefile
zlib/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
