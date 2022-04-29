#ifndef DRAW__H
#define DRAW__H 1

// Window minimum size
#define MINIMUM_WIDTH 320
#define MINIMUM_HEIGHT 240

extern unsigned int window_width;
extern unsigned int window_height;
extern Image *logo;

unsigned int draw_init ();
void draw_render ();
void draw_free ();

#endif
