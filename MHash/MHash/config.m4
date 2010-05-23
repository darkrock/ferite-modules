AC_CHECK_LIB(mhash, mhash_init, mhash=yes, echo "Cannot find libmhash: see http://mhash.sourceforge.net/",)

if test "$mhash" = yes; then
  MHash_LIBS="-lmhash"
  MHash_CFLAGS=""
  AC_SUBST(MHash_LIBS)
  AC_SUBST(MHash_CFLAGS)
  modules="$modules MHash"
  echo "Building MHash support..."
fi
