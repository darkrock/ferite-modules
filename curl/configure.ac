dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([curl/curl.fec])
AM_INIT_AUTOMAKE(curl-ferite-module, 1.1.1)
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

sinclude(curl/config.m4)

# check for --with-curl
AC_MSG_CHECKING(for --with-curl-config)
AC_ARG_WITH(curl, [  --with-curl-config=PATH        Path to curl-config],
[
  if test -x "$withval"
  then
    AC_MSG_RESULT([$withval executable, good])
    CURL_CONFIG=$withval
  else
    echo
    AC_MSG_ERROR([$withval not found or not executable])
  fi
],
AC_MSG_RESULT(no))

# if no apxs found yet, check /usr/local/apache/sbin
# since it's the default Apache location
if test -z "$CURL_CONFIG"; then
  AC_MSG_CHECKING(for curl-config in /usr/local/bin)
  if test -x /usr/local/bin/curl-config; then
    CURL_CONFIG=/usr/local/bin/curl-config
    AC_MSG_RESULT([found, we'll use this. Use --with-curl-config=PATH to specify another.])    
  else
    AC_MSG_RESULT(no)
  fi
fi


if test -z "$CURL_CONFIG"; then
  AC_MSG_CHECKING(for curl-config in /usr/bin)
  if test -x /usr/bin/curl-config; then
    CURL_CONFIG=/usr/bin/curl-config
    AC_MSG_RESULT([found, we'll use this. Use --with-curl-config=PATH to specify another.])    
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$CURL_CONFIG"; then
  AC_PATH_PROG(CURL_CONFIG, curl-config)
  if test -x $CURL_CONFIG; then
    AC_MSG_RESULT([found $CURL_CONFIG, we'll use this. Use --with-curl-config to specify another.])
  else
    AC_MSG_RESULT(no)
  fi
fi

if test -z "$CURL_CONFIG"; then
  AC_MSG_ERROR([**** curl-config was not found. You must install libcurl to use this Package. cURL is at: http://curl.haxx.se])
fi

curl_CFLAGS="$curl_CFLAGS $FERITE_CFLAGS `$CURL_CONFIG --cflags`"
curl_LIBS="$curl_LIBS $FERITE_LIBS `$CURL_CONFIG --libs`"

MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

#dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

AC_CONFIG_FILES([
Makefile
curl/Makefile
test/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
