AC_PREREQ([2.69])
AC_INIT([gtkopengl], [0.0.0], [https://github.com/jburguete/gtkopengl])

# Checks for operative systems
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_MSG_NOTICE([Build: $build])
AC_MSG_NOTICE([Host: $host])
AC_MSG_NOTICE([Target: $target])

# Checks for programs
AC_PROG_CC([egcc gcc clang])
AC_MSG_NOTICE([Compiler: $CC])
AC_PROG_MAKE_SET
AC_LANG([C])
PKG_PROG_PKG_CONFIG

# Checks for libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([PNG], [libpng])
PKG_CHECK_MODULES([FREETYPE], [freetype2])
PKG_CHECK_MODULES([GLEW], [glew])
PKG_CHECK_MODULES([GLFW], [glfw3])
PKG_CHECK_MODULES([SDL], [sdl2])
AC_ARG_WITH([gtk],
	AS_HELP_STRING([--with-gtk],
		[Build using a version of GTK (3 (--with-gtk=3 or 4 (--with-gtk=4))]))
if test "x$with_gtk" = "x4"; then
	PKG_CHECK_MODULES([GTK], [gtk4], [gtk4=1], [gtk4=0])
  if test $gtk4 = 0; then
	  PKG_CHECK_MODULES([GTK], [gtk+-3.0])
  fi
elif test "x$with_gtk" = "x3"; then
	PKG_CHECK_MODULES([GTK], [gtk+-3.0])
fi

# Final output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
