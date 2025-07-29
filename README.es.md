gtkopengl
=========

:uk:[english](README.md) :es:[español](README.es.md)

Programas de demostración para dibujar con GTK y OpenGL

AUTORES
-------

* Javier Burguete Tolosa
  [jburguete@eead.csic.es](mailto:jburguete@eead.csic.es).

DEMOS
-----

* demo0: dibuja un triángulo en una ventana FreeGLUT/GLFW/SDL/GtkWindow.

* demo1: dibuja un triángulo en una ventana FreeGLUT/GLFW/SDL/GtkWindow y una
  ventana GtkWindow para cerrar.

* demo2: dibuja dos triángulos, un fichero PNG transparente y un texto en una
  ventana FreeGLUT/GLFW/SDL/GtkWindow y una ventana GtkWindow para cerrar.

CONSTRUIR LOS EJECUTABLES
-------------------------

ÚTILES Y BIBLIOTECAS REQUERIDOS
_______________________________

Obligatorio:
* [gcc](https://gcc.gnu.org) o [clang](http://clang.llvm.org) para compilar el
  código fuente.
* [make](http://www.gnu.org/software/make) para construir el ejecutable.
* [autoconf](http://www.gnu.org/software/autoconf) para generar el Makefile en
  diferentes sistemas operativos.
* [automake](http://www.gnu.org/software/automake) para chequear el sistema
  operativo.
* [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config) para
  encontrar las bibliotecas para compilar.
* [glib](https://developer.gnome.org/glib) utilidades extendidas de C para
  trabajar con datos, listas, ficheros mapeados, expresiones regulares, usar
  multiprocesador en máquinas de memoria compartida, ...
* [png](http://libpng.sourceforge.net) para trabajar con ficheros PNG.
* [gtk](http://www.gtk.org) para trabajar con ventanas interactivas.
* [glew](http://glew.sourceforge.net) funciones OpenGL de alto nivel.

Bibliotecas opcionales que pueden usarse como alternativa al widget GtkGLArea de
la biblioteca GTK para interactuar con OpenGL para dibujar gráficos:
* [freeglut](http://freeglut.sourceforge.net)
* [sdl2](https://www.libsdl.org)
* [glfw](http://www.glfw.org)

SISTEMAS OPERATIVOS
___________________

Pueden instalarse todas las bibliotecas y utilidades requeridas usando las
instrucciones de [install-unix](https://github.com/jburguete/install-unix).

Este programa ha sido construido y probado en los siguientes sistemas
operativos:
* Alpine Linux
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

En sistemas Microsoft Windows hay que instalar
[MSYS2](http://sourceforge.net/projects/msys2) además de las bibliotecas y
utilidades requeridas. Puede seguir instrucciones detalladas en el tutorial
[install-unix](https://github.com/jburguete/install-unix/blob/master/tutorial.pdf).

En NetBSD 10.0, para usar la última versión del compilador GCC, hay que hacer
primero en el terminal:
> $ export PATH="/usr/pkg/gcc9/bin:$PATH"

Si se quiere hacer permanente tambien se puede añadir en el fichero ".profile"
de la carpeta raíz del usuario la línea:
> PATH="/usr/pkg/gcc9/bin:$PATH"

En OpenBSD 7.6 hay que hacer antes en el terminal:
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

1. Pana OpenGL en FreeGLUT
> $ ./gtk-freeglut

2. Para OpenGL en GLFW
> $ ./gtk-glfw

3. Para OpenGL en SDL
> $ ./gtk-sdl

4. Para OpenGL en un widget GtkGLArea en GTK
> $ ./gtk-glarea
