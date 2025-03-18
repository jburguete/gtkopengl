gtkopengl
=========

:uk:[english](README.md) :es:[español](README.es.md)

Demo programs to draw with GTK and OpenGL

AUTHORS
-------

* Javier Burguete Tolosa
  [jburguete@eead.csic.es](mailto:jburguete@eead.csic.es).

DEMOS
-----

* demo0: Draw one triangle in a FreeGLUT/GLFW/SDL/GtkWindow window.

* demo1: Draw one triangle in a FreeGLUT/GLFW/SDL/GtkWindow window and a
  GtkWindow to close.

* demo2: Draw two triangles, a transparent PNG file and a text in a
  FreeGLUT/GLFW/SDL/GtkWindow window and a GtkWindow to close.

BUILDING THE EXECUTABLES
------------------------

REQUIRED LIBRARIES AND UTILITIES
________________________________

Mandatory:
* [gcc](https://gcc.gnu.org) or [clang](http://clang.llvm.org) to compile the
  source code.
* [make](http://www.gnu.org/software/make) to build the executable file.
* [autoconf](http://www.gnu.org/software/autoconf) to generate the Makefile in
  different operating systems.
* [automake](http://www.gnu.org/software/automake) to check the operating
  system.
* [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config) to find the
  libraries to compile.
* [glib](https://developer.gnome.org/glib) extended utilities of C to work with
  data, lists, mapped files, regular expressions, using multicores in shared
  memory machines, ...
* [png](http://libpng.sourceforge.net) to work with PNG files.
* [gtk](http://www.gtk.org) to work with interactive windows.
* [glew](http://glew.sourceforge.net) high level OpenGL functions.

The following optional libraries can be used as alternative to the GtkGLArea
widget of the GTK library to interact with OpenGL to draw graphs.
* [freeglut](http://freeglut.sourceforge.net)
* [sdl2](https://www.libsdl.org)
* [glfw](http://www.glfw.org)

OPERATIVE SYSTEMS
_________________

You can install all required utilities and libraries using the instructions of
[install-unix](https://github.com/jburguete/install-unix).

This software has been built and tested in the following operative systems:
* Arch Linux
* Debian 12 (Linux)
* Devuan Linux 5
* Dragonfly BSD 6.4.0
* Fedora Linux 40
* FreeBSD 14.1
* Gentoo Linux
* Linux Mint DE 6
* MacOS Ventura + Homebrew
* Manjaro Linux
* Microsoft Windows 11 + MSYS2
* NetBSD 10.0
* OpenBSD 7.6
* OpenInidiana Hipster
* OpenSUSE Linux 15.5
* Ubuntu Linux 

On Microsoft Windows systems you have to install
[MSYS2](http://sourceforge.net/projects/msys2) and the required
libraries and utilities. You can follow detailed instructions in
[install-unix](https://github.com/jburguete/install-unix/blob/master/tutorial.pdf)
tutorial.

On NetBSD 10.0, to use the last GCC version, you have to do first on the
building terminal:
> $ export PATH="/usr/pkg/gcc9/bin:$PATH"

To do permanent this change the following line can be added to the ".profile"
file in the user root directory:
> PATH="/usr/pkg/gcc9/bin:$PATH"

On OpenBSD 7.6 you have to do first on the building terminal:
> $ export AUTOCONF\_VERSION=2.69 AUTOMAKE\_VERSION=1.16

BUILDING INSTRUCTIONS
_____________________

1. Load the last program version:
> $ git clone https://github.com/jburguete/gtkopengl

2. Enter into the selected demo directory
> $ cd gtkopengl/demoX

3. Prepare for autoconf
> $ aclocal

4. Execute autoconf
> $ autoconf

5. Execute automake
> $ automake --add-missing

6. Configure for the selected GTK version (3 or 4)
> $ ./configure --with-gtk=4

7. Build
> $ make

EXECUTE THE DEMO
----------------

1. For OpenGL in FreeGLUT
> $ ./gtk-freeglut

2. For OpenGL in GLFW
> $ ./gtk-glfw

3. For OpenGL in SDL
> $ ./gtk-sdl

4. For OpenGL in a GtkGLArea widget in GTK
> $ ./gtk-glarea
