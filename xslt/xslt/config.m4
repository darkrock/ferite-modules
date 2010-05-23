dnl xml module stuff

xslt_LIBS=""
xslt_CFLAGS=""

xslt_LIBS=`xslt-config --libs`
xslt_CFLAGS=`xslt-config --cflags`
echo "Building xslt support..."

AC_SUBST(xslt_LIBS)
AC_SUBST(xslt_CFLAGS)
