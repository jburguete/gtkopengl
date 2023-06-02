/**
 * \file draw.h
 * \brief Header file with functions and variables for initializing, rendering,
 *   and freeing related resources to draw a triangle with OpenGL.
 * \copyright Copyright 2022-2023, Javier Burguete Tolosa. All rights reserved.
 */
#ifndef DRAW__H
#define DRAW__H 1

#define MINIMUM_WIDTH 320       ///< minimum window width.
#define MINIMUM_HEIGHT 240      ///< minimum window height.
extern unsigned int window_width, window_height;

int draw_init ();
void draw_render ();
void draw_free ();

#endif
