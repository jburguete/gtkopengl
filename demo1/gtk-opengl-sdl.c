/**
 * \file gtk-opengl-sdl.c
 * \brief Source file with functions and variables to draw a triangle with
 *   OpenGL and SDL.
 * \author Javier Burguete Tolosa.
 * \date 2022-2025.
 * \license BSD-2-Clause.
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <glib.h>
#include <epoxy/gl.h>
#include <SDL.h>
#include <gtk/gtk.h>
#include "config.h"
#include "draw.h"

#if GTK_MAJOR_VERSION > 3
#define TITLE "GTK4"
#else
#define TITLE "GTK3"
#endif
///< windows title.

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context = NULL;
SDL_Window *sdl_window = NULL;  ///< SDL window.
SDL_GLContext sdl_context = NULL;       ///< SDL OpenGL context.

/**
 * SDL render function.
 */
static void
sdl_render ()
{

#if DEBUG
  fprintf (stderr, "sdl_render: start\n");
#endif

  SDL_GL_MakeCurrent (sdl_window, sdl_context);
  draw_render ();
  SDL_GL_SwapWindow (sdl_window);

#if DEBUG
  fprintf (stderr, "sdl_render: end\n");
#endif

}

/**
 * SDL resize function.
 */
static void
sdl_resize (int w,              ///< new width.
            int h)              ///< new height.
{
  unsigned int resize = 0;

#if DEBUG
  fprintf (stderr, "sdl_resize: start\n");
#endif

  if (w < MINIMUM_WIDTH)
    {
      window_width = MINIMUM_WIDTH;
      resize = 1;
    }
  else
    window_width = w;
  if (h < MINIMUM_HEIGHT)
    {
      window_height = MINIMUM_HEIGHT;
      resize = 1;
    }
  else
    window_height = h;
  if (resize)
    SDL_SetWindowSize (sdl_window, window_width, window_height);
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "sdl_resize: end\n");
#endif

}

/**
 * Init SDL.
 */
static int
sdl_init ()
{

#if DEBUG
  fprintf (stderr, "sdl_init: start\n");
#endif

  if (SDL_Init (SDL_INIT_VIDEO))
    {
      printf ("%s: %s\n", "Unable to init SDL", SDL_GetError ());
      goto end;
    }
  sdl_window
    = SDL_CreateWindow (TITLE "-SDL",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        MINIMUM_WIDTH, MINIMUM_HEIGHT,
                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (!sdl_window)
    {
      printf ("%s: %s\n", "Unable to create SDL window", SDL_GetError ());
      goto end;
    }
  SDL_SetWindowSize (sdl_window, window_width, window_height);
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetSwapInterval (1);
  sdl_context = SDL_GL_CreateContext (sdl_window);
  if (!sdl_context)
    {
      printf ("%s: %s\n", "Unable to create SDL context", SDL_GetError ());
      goto end;
    }

#if DEBUG
  fprintf (stderr, "sdl_init: end on success\n");
#endif

  return 1;

end:

#if DEBUG
  fprintf (stderr, "sdl_init: end on error\n");
#endif

  return 0;
}

/**
 * SDL loop.
 */
static void
sdl_loop ()
{
  SDL_Event event[1];
  GMainContext *context;

#if DEBUG
  fprintf (stderr, "sdl_loop: start\n");
#endif

  context = g_main_context_default ();
  while (1)
    {
      if (gl_context)
        gdk_gl_context_make_current (gl_context);
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      while (SDL_PollEvent (event))
        {
          switch (event->type)
            {
            case SDL_QUIT:
              goto end;
            case SDL_WINDOWEVENT:
              switch (event->window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                  sdl_resize (event->window.data1, event->window.data2);
                }
            }
          sdl_render ();
        }
    }

end:

#if DEBUG
  fprintf (stderr, "sdl_loop: end\n");
#endif

  return;
}

/**
 * SDL free.
 */
static void
sdl_free ()
{

#if DEBUG
  fprintf (stderr, "sdl_free: start\n");
#endif

  draw_free ();
  SDL_GL_DeleteContext (sdl_context);
  SDL_Quit ();

#if DEBUG
  fprintf (stderr, "sdl_free: end\n");
#endif

}

// Quit loop
static void
sdl_loop_quit ()
{
  SDL_Event event[1];

#if DEBUG
  fprintf (stderr, "sdl_quit: start\n");
#endif

  event->type = SDL_QUIT;
  SDL_PushEvent (event);

#if DEBUG
  fprintf (stderr, "sdl_quit: end\n");
#endif

}

/**
 * Main function.
 *
 * \return exit status.
 */
int
main (int argn __attribute__((unused)), ///< number of command-line arguments. 
      char **argc __attribute__((unused)))
  ///< array of command-line arguments.
{
  GtkButton *button_close;

  // Render window
  if (!sdl_init ())
    return 4;
  if (!draw_init ())
    return 3;

  // GTK main window
#if GTK_MAJOR_VERSION > 3
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_window_set_title (gtk_window, TITLE);
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_window_set_child (gtk_window, GTK_WIDGET (button_close));
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_window_destroy, gtk_window);
  gtk_window_present (gtk_window);
#else
  gtk_init (&argn, &argc);
  gtk_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (gtk_window, TITLE);
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_container_add (GTK_CONTAINER (gtk_window), GTK_WIDGET (button_close));
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_widget_destroy, gtk_window);
  gtk_widget_show_all (GTK_WIDGET (gtk_window));
#endif
  gl_context = gdk_gl_context_get_current ();
  if (!gl_context)
    printf ("GdkGLContext: NULL\n");
  g_signal_connect (gtk_window, "destroy", sdl_loop_quit, NULL);

  // Main loop
  sdl_loop ();

  // Free resources
  sdl_free ();
  return 0;
}
