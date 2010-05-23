#! /bin/sh

ACLOCAL_FLAGS=

# some monkey tricks to get the right flags
for parm in $@
do
  case "${parm}" in
    --with-ferite-prefix=*)
      dirFerite="${parm#*=}"
      dirFerite="${dirFerite%%/}/"
      if [ ! -f "${dirFerite}/bin/ferite-config" ]
      then
        echo "error: unable to locate ferite-config with prefix"
        echo "       dirFerite    == '${dirFerite}'"
        echo "       cmdOption    == '${1}'"
        exit 2
      elif [ ! -x "${dirFerite}/bin/ferite-config" ]
      then
        echo "error: unable to execute ferite-config with prefix"
        echo "       dirFerite    == '${dirFerite}'"
        echo "       cmdOption    == '${1}'"
        exit 2
      else
        # you must export ACLOCAL_FLAGS if ferite is installed in
        # a non-standard location. standard being /usr/bin
        export ACLOCAL_FLAGS="-I ${dirFerite}/share/aclocal"
      fi
      ;;
    *) : ;;
  esac
done


echo
echo 
echo "NOTE:"
echo "you will need libtool 1.3 or higher for this to work"
echo
echo

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

# libtool requires configure.ac to work
if [ ! -f configure.ac ]
then
  echo "error: configure.ac is missing"
  exit 2
else
  libtoolize -c -f
  if [ $? != 0 ]
  then
    # maybe libtool isn't installed so we should try
    # glibtool?
    glibtoolize -c -f || exit 2
  fi
fi

# run the autotools and autoconf
aclocal -I . $ACLOCAL_FLAGS || exit 2
autoheader                  || exit 2
automake -a -c              || exit 2
autoconf                    || exit 2

# now let us run the configure command
cd "$THEDIR"                || exit 2

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

$srcdir/configure $@

echo "Now type:"
echo
echo "make"
echo "make install"
echo
echo "have fun."
