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
elif test `uname -s` = "Darwin"; then
	AC_MSG_NOTICE([MacOS])
	AC_MSG_NOTICE([Remember:])
	AC_MSG_NOTICE("echo 'export PATH=''/usr/local/opt/llvm@13/bin:$PATH''' >> ./zshrc")
	LDFLAGS="$LDFLAGS -L/System/Library/Frameworks -framework GLUT -framework OpenGL"
fi
AC_SUBST(HAIKU, [$haiku])

# Checks for programs
AC_PROG_CC([gcc-12 gcc12 gcc-11 gcc11 gcc-10 gcc10 egcc gcc clang])
AC_MSG_NOTICE([Compiler: $CC])
AC_PROG_MAKE_SET
AC_LANG([C])
PKG_PROG_PKG_CONFIG

# Checks for libraries
PKG_CHECK_MODULES([GLIB], [glib-2.0])
AC_SEARCH_LIBS([glViewport], [GL opengl32], 
	AC_MSG_NOTICE([OpenGL OK]),
	AC_MSG_ERROR([OpenGL not found]))
if test $haiku = 1; then
	AC_MSG_NOTICE([Haiku])
	AC_SUBST(GLEW_LIBS, "-lGLEW -lGLU")
else
	PKG_CHECK_MODULES([GLEW], [glew])
	PKG_CHECK_MODULES([GLUT], [freeglut], ,
		[PKG_CHECK_MODULES([GLUT], [glut])])
fi
PKG_CHECK_MODULES([GLFW], [glfw3])
PKG_CHECK_MODULES([SDL], [sdl2])
PKG_CHECK_MODULES([GTK3], [gtk+-3.0])
PKG_CHECK_MODULES([GTK4], [gtk4], AC_SUBST([gtk4], [1]),
	[AC_SUBST([gtk4], [0])])

# Final output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
