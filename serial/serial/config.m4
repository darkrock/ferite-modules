AC_CHECK_FUNC(tcsetattr, termios=yes)
if test "$termios" = yes; then
  AC_SUBST(serial_LIBS)
  AC_SUBST(serial_CFLAGS)
  modules="$modules serial"
  echo "Building serial module..."
fi
