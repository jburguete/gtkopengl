# Header
AC_PREREQ([2.69])
AC_INIT([gtkopengl], [0.0.0], [https://github.com/jburguete/gtkopengl])

# Checks for operative systems
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_MSG_NOTICE([Build: $build])
AC_MSG_NOTICE([Host: $host])
AC_MSG_NOTICE([Target: $target])
haiku=0
if test `uname -s` = "Haiku"; then
	AC_MSG_NOTICE([Haiku])
	haiku=1
elif test `uname -s` = "NetBSD"; then
	AC_MSG_NOTICE([NetBSD])
	LDFLAGS="$LDFLAGS -L/usr/pkg/lib -L/usr/X11R7/lib"
	CPPFLAGS="$CPPFLAGS -I/usr/X11R7/include"
elif test `uname -s` = "OpenBSD"; then
	AC_MSG_NOTICE([OpenBSD])
	LDFLAGS="$LDFLAGS -L/usr/X11R6/lib"
	CPPFLAGS="$CPPFLAGS -I/usr/X11R6/include"
elif test `uname -s` = "Darwin"; then
	AC_MSG_NOTICE([MacOS])
	AC_MSG_NOTICE([Remember:])
	AC_MSG_NOTICE("echo 'export PATH=''/usr/local/opt/llvm/bin:$PATH''' >> ./zshrc")
	LDFLAGS="$LDFLAGS -L/System/Library/Frameworks -framework GLUT -framework OpenGL"
fi
AC_SUBST(HAIKU, [$haiku])

# Checks for programs
AC_PROG_CC([gcc-15 gcc15 gcc-14 gcc14 gcc-13 gcc13 gcc-12 gcc12 egcc gcc clang])
AC_MSG_NOTICE([Compiler: $CC])
AC_PROG_MAKE_SET
AC_LANG([C])
PKG_PROG_PKG_CONFIG

# Checks for libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([PNG], [libpng])
PKG_CHECK_MODULES([FREETYPE], [freetype2])
AC_SEARCH_LIBS([glViewport], [GL opengl32], 
	AC_MSG_NOTICE([OpenGL OK]),
	AC_MSG_ERROR([OpenGL not found]))
PKG_CHECK_MODULES([EPOXY], [epoxy])
PKG_CHECK_MODULES([GLFW], [glfw3], AC_SUBST([glfw], [1]), AC_SUBST([glfw], [0]))
PKG_CHECK_MODULES([SDL], [sdl2], AC_SUBST([sdl], [1]), AC_SUBST([sdl], [0]))
PKG_CHECK_MODULES([GTK3], [gtk+-3.0])
PKG_CHECK_MODULES([GTK4], [gtk4], AC_SUBST([gtk4], [1]),
	AC_SUBST([gtk4], [0]))

# Check for font
AC_MSG_NOTICE([Searching font...])
if test $haiku = 1; then
	font=/boot/system/data/fonts/ttfonts/NotoSansMono-Regular.ttf
else
	if test `uname -o` = "Msys"; then
		dir="/ucrt64"
	else
		dir="/usr"
	fi
	font=`find $dir -type f -iname freemono.otf -print -quit`
	if test "x$font" = "x"; then
		font=`find $dir -type f -iname freemono.ttf -print -quit`
		if test "x$font" = "x"; then
			AC_MSG_ERROR([FreeMono font not found])
		fi
	fi
fi
AC_MSG_NOTICE([Font: $font])
AC_SUBST(FONT, "-DFONT=\\\"$font\\\"")

# Final output
AC_CONFIG_FILES([demo0/Makefile demo1/Makefile demo2/Makefile])
AC_OUTPUT
