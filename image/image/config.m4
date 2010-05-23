AC_CHECK_PROG(IMLIBC, imlib2-config, imlib2-config)

image_LIBS=`$IMLIBC --libs`
image_CFLAGS=`$IMLIBC --cflags`
AC_SUBST(image_LIBS)
AC_SUBST(image_CFLAGS)

modules="$modules image"
