

dnl Process this file with autoconf to create configure.

AC_INIT
AC_CONFIG_SRCDIR([dbi/dbi.fec])
AM_INIT_AUTOMAKE(dbi-ferite-module, 1.1.1)
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

sinclude(dbi/config.m4)

MODULE_SRC_PREFIX="\${top_srcdir}"
AC_SUBST(MODULE_SRC_PREFIX)

AM_PATH_FERITE(1.1.1,,
   AC_MSG_ERROR(You should have ferite 1.1.1 installed for this to work.)
)

if ! test -z "$prefix"; then
   prefix="$FERITE_PREFIX";
   echo "Setting prefix to $FERITE_PREFIX";
fi

AC_ARG_WITH(libdbi-libdir,[ --with-libdbi-libdir=<location of libdbi libs> ],
               LIBDBI="$withval",LIBDBI="$prefix/lib/")

AC_ARG_WITH(libdbi-incdir,[ --with-libdbi-incdir=<location of libdbi includes> ],
               LIBDBIINC="$withval",LIBDBIINC="$prefix/include/")

if test -f "$LIBDBI/libdbi.so"; then
     LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBDBI
fi

CFLAGS="$CFLAGS -I$prefix/include -I$LIBDBIINC"
CPPFLAGS="$CPPFLAGS -I$prefix/include -I$LIBDBIINC"
LDFLAGS="$LDFLAGS -L$prefix/lib"

AC_CHECK_LIB(dbi,dbi_initialize,,  
     AC_MSG_ERROR(libdbi not found in LIBRARY_PATH or "$LIBDBI".  Set with --with-libdbi-libdir=)  ) 

AC_CHECK_HEADER(dbi/dbi.h,,
     AC_MSG_ERROR(libDBI headers not found. Use --with-libdbi-incdir=) )

dbi_CFLAGS="$dbi_CFLAGS $FERITE_CFLAGS -I$LIBDBIINC"
dbi_LIBS="$dbi_LIBS $FERITE_LIBS $L$LIBDBI"
AC_SUBST(dbi_LIBS)
AC_SUBST(dbi_CFLAGS)

#dnl we do this because the threading libs are already passed through ferite-config
pthread_defs=""
AC_SUBST(pthread_defs)

AC_CONFIG_FILES([
Makefile
dbi/Makefile
examples/Makefile
])
AC_CONFIG_COMMANDS([default],[[
]],[[]])
 
AC_OUTPUT
