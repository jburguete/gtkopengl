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
PKG_CHECK_MODULES([GTK4], [gtk4])
PKG_CHECK_MODULES([GTK3], [gtk+-3.0])

# Check for font
AC_MSG_NOTICE([Searching font...])
font=`find /usr -type f -iname freemono.otf`
if test "x$font" = "x"; then
  font=`find /usr -type f -iname freemono.ttf`
	if test "x$font" = "x"; then
		AC_MSG_ERROR([FreeMono font not found])
	fi
fi
AC_MSG_NOTICE([Font: $font])
AC_SUBST(FONT, "-DFONT=\\\"$font\\\"")

# Final output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
