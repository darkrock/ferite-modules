AC_CHECK_LIB(curses, initscr, curses=yes, echo "Cannot find the curses library",)

if test "$curses" = yes; then
  curses_LIBS="-lcurses"
  curses_CFLAGS=""
  AC_SUBST(curses_LIBS)
  AC_SUBST(curses_CFLAGS)
  modules="$modules curses"
  echo "Building curses support..."
fi
