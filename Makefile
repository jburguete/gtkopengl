.PHONY: strip backup clean

FLAGS = -march=native -Os -Wall -Wextra
DEP = draw.h image.h text.h Makefile
SRC = draw.c image.c text.c 
FONT = `find /usr -type f -iname freesans.ttf`
LIBS1 = glew glib-2.0 libpng freetype2 glfw3
CFLAGS1 = `pkg-config --cflags $(LIBS1)` $(FLAGS)
LDFLAGS1 = `pkg-config --libs $(LIBS1)`
ifeq ($(gtk), 4)
LIBS2 = glew glib-2.0 libpng freetype2 gtk4
CFLAGS2 = `pkg-config --cflags $(LIBS2)` $(FLAGS) -DGTK4=1
else
LIBS2 = glew glib-2.0 libpng freetype2 gtk+-3.0
CFLAGS2 = `pkg-config --cflags $(LIBS2)` $(FLAGS)
endif
LDFLAGS2 = `pkg-config --libs $(LIBS2)`
LIBS3 = glib-2.0 glew glfw3 sdl2 gtk4
CFLAGS3 = `pkg-config --cflags $(LIBS3)` $(FLAGS)
LDFLAGS3 = `pkg-config --libs $(LIBS3)` -lglut
CC = gcc -g -flto
ALL = glfw gtk gtk2 gtkopengl 

all: $(ALL)

glfw: glfw.c $(SRC) $(DEP)
	$(CC) $(CFLAGS1) glfw.c $(SRC) -o glfw -DFONT=\"$(FONT)\" $(LDFLAGS1)

gtk: gtk.c $(SRC) $(DEP)
	$(CC) $(CFLAGS2) gtk.c $(SRC) -o gtk -DFONT=\"$(FONT)\" $(LDFLAGS2) 

gtk2: gtk2.c Makefile
	$(CC) $(CFLAGS2) gtk2.c -o gtk2 -DFONT=\"$(FONT)\" $(LDFLAGS2) 

gtkopengl: gtkopengl.c Makefile
	$(CC) $(CFLAGS3) gtkopengl.c -o gtkopengl $(LDFLAGS3)

strip:
	make
	strip $(ALL)

backup:
	tar cJf gtkopengl.tar.xz *.c *.h *.png Makefile

clean:
	rm $(ALL)
