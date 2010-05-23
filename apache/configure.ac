dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([apache/apache.fec])
AM_INIT_AUTOMAKE(mod_ferite, 1.1.18)
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

AC_ARG_WITH(apache-dir,[  --with-apache-dir=<location of Apacheroot> ],
	apachedir="$withval",apachedir="/usr" )
AC_ARG_WITH(apxs, [  --with-apxs=<location of apxs> ],
	apxs="$withval",apxs=`PATH=$PATH:$apachedir/bin; which apxs` )
AC_ARG_WITH(apachectl, [  --with-apachectl=<location of apachectl> ],
	apachectl="$withval", apachectl="$apachedir/bin/apachectl" )
AC_ARG_WITH(embfer, [  --with-embfer-location=<location of embfer-config> ],
	embfer="$withval", embfer=`which embfer-config` )
		
AC_ARG_ENABLE(debug,[  --enable-debug ],
               apache_debug="yes",apache_debug="no")
AC_SUBST(apachedir)
AC_SUBST(apxs)
AC_SUBST(apachectl)
AC_SUBST(embfer)

AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

AM_PATH_FERITE(1.1.18,,[
  echo "You should have ferite 1.1.18 installed for this to work.";
])

if ! test -z "$prefix"; then
   prefix="$FERITE_PREFIX";
   echo "Setting prefix to $FERITE_PREFIX";
fi

sinclude(apache/config.m4)

if test -f "$apachedir/bin/apr-config"; then 
	aprconfig="$apachedir/bin/apr-config";
else
	aprconfig="apr-1-config";
fi

APR_CFLAGS=`$aprconfig --includes --cppflags`
APR_LDFLAGS=`$aprconfig --link-ld`

if test -f "$apachedir/bin/apu-config"; then 
	apuconfig="$apachedir/bin/apu-config";
else
	apuconfig="apu-1-config";
fi

APU_CFLAGS=`$apuconfig --includes`
APU_LDFLAGS=`$apuconfig --link-ld`

AC_ARG_ENABLE(embfer, [  --enable-embfer],
		   en_embfer="yes", en_embfer="no" )
if test $en_embfer = "yes"; then
   EMBFER_CFLAGS=`$embfer --cflags` ;
   EMBFER_LDFLAGS=`$embfer --libs` ;
   APXS_DEF="$APXS_DEF -D ENABLE_EMBFER" ;
   echo "Embfer support will be compiled into mod_ferite" ;
else
   EMBFER_CFLAGS="" ;
   EMBFER_LDFLAGS="" ;
   APXS_DEF="" ;
   echo "Embfer support will not be compiled into mod_ferite" ;
fi

APXS_FLAGS="-I"`$apxs -q INCLUDEDIR`

apache_CFLAGS="$apache_CFLAGS $FERITE_CFLAGS $EMBFER_CFLAGS $APR_CFLAGS $APU_CFLAGS $APXS_FLAGS"
apache_LIBS="$apache_LIBS $FERITE_LIBS $EMBFER_LDFLAGS $APR_LDFLAGS $APU_LDFLAGS"

MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

#dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

#dnl these need to be auto confgiured
AP_PREFIX="$apachedir"

#dnl Check if apxs exists.
AC_MSG_CHECKING(for apxs)
if ! test -f "$apxs"; then  
   AC_MSG_RESULT(no)
   AC_MSG_ERROR("Can't find apxs (Apache extension tool). 
                   Specify ServerRoot using --with-apache-dir=<serverroot>
		or Specify the apxs path using --with-apxs=<apxs path>" )
else
   AC_MSG_RESULT(yes)
fi

#dnl this works, FE_* comes from the AM_PATH_FERITE() call
APXS_DEF="$APXS_DEF -DLIBRARY_DIR=\\\\\\\"$FE_LIBRARY_PATH\\\\\\\" -DNATIVE_LIBRARY_DIR=\\\\\\\"$FE_NATIVE_LIBRARY_PATH\\\\\\\" -DXPLAT_LIBRARY_DIR=\\\\\\\"$FE_XPLAT_LIBRARY_PATH\\\\\\\" "
APXS_INC="-I$FERITE_PREFIX/include $FERITE_CFLAGS $EMBFER_CFLAGS"
APXS_LIB="-L$FERITE_PREFIX/lib $FERITE_LIBS $EMBFER_LDFLAGS"

#dnl should we go in to debug mode.
if test $apache_debug = "yes"; then
   APXS_DEF="$APXS_DEF  -DDBUG"
fi

APXS="$apxs"
APACHECTL="$apachectl"
AP_PREFIX="$apachedir"

AC_SUBST(AP_PREFIX)
AC_SUBST(APXS)
AC_SUBST(APACHECTL)
AC_SUBST(APXS_DEF)
AC_SUBST(APXS_INC)
AC_SUBST(APXS_LIB)


AC_CONFIG_FILES([
Makefile
apache/Makefile
mod_ferite/Makefile
scripts/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
