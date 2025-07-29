/**
 * \file gtk-opengl-glarea.c
 * \brief Source file with functions and variables to draw a triangle with
 *   OpenGL and GTK.
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
GtkWindow *gtk_window, *main_window;
GtkGLArea *gtk_draw;

/**
 * GTK resize function.
 */
static void
glarea_resize (GtkGLArea * widget __attribute__((unused)),
               ///< OpenGL widget.
               int w,           ///< new widget width.
               int h)           ///< new widget height.
{

#if DEBUG
  fprintf (stderr, "glarea_resize: start\n");
#endif

  window_width = w;
  window_height = h;
  glViewport (0, 0, window_width, window_height);
#if GTK_MAJOR_VERSION < 4
  gtk_widget_queue_draw (GTK_WIDGET (widget));
#endif

#if DEBUG
  fprintf (stderr, "glarea_resize: end\n");
#endif

}

/**
 * GTK realize function.
 */
static void
glarea_realize ()
{

#if DEBUG
  fprintf (stderr, "glarea_realize: start\n");
#endif

  gtk_gl_area_make_current (gtk_draw);
  draw_init ();

#if DEBUG
  fprintf (stderr, "glarea_realize: end\n");
#endif

}

/**
 * GTK unrealize function.
 */
static void
glarea_unrealize ()
{

#if DEBUG
  fprintf (stderr, "glarea_unrealize: start\n");
#endif

  gtk_gl_area_make_current (gtk_draw);
  draw_free ();

#if DEBUG
  fprintf (stderr, "glarea_unrealize: end\n");
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
  GMainLoop *main_loop;

  // Init main loop
  main_loop = g_main_loop_new (NULL, 0);

  // Render window
#if GTK_MAJOR_VERSION > 3
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_window_set_title (gtk_window, TITLE "-GtkGLArea");
  gtk_draw = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (gtk_draw), MINIMUM_WIDTH,
                               MINIMUM_HEIGHT);
  gtk_window_set_child (gtk_window, GTK_WIDGET (gtk_draw));
  g_signal_connect_swapped (gtk_window, "destroy",
                            (GCallback) g_main_loop_quit, main_loop);
  g_signal_connect (gtk_draw, "realize", (GCallback) glarea_realize, NULL);
  g_signal_connect (gtk_draw, "resize", (GCallback) glarea_resize, NULL);
  g_signal_connect (gtk_draw, "render", (GCallback) draw_render, NULL);
  g_signal_connect (gtk_draw, "unrealize", (GCallback) glarea_unrealize, NULL);
  gtk_window_present (gtk_window);
#else
  gtk_init (&argn, &argc);
  gtk_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (gtk_window, TITLE "-GtkGLArea");
  gtk_draw = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (gtk_draw), MINIMUM_WIDTH,
                               MINIMUM_HEIGHT);
  gtk_container_add (GTK_CONTAINER (gtk_window), GTK_WIDGET (gtk_draw));
  g_signal_connect_swapped (gtk_window, "destroy",
                            (GCallback) g_main_loop_quit, main_loop);
  g_signal_connect (gtk_draw, "realize", (GCallback) glarea_realize, NULL);
  g_signal_connect (gtk_draw, "resize", (GCallback) glarea_resize, NULL);
  g_signal_connect (gtk_draw, "render", (GCallback) draw_render, NULL);
  g_signal_connect (gtk_draw, "unrealize", (GCallback) glarea_unrealize, NULL);
  gtk_widget_show_all (GTK_WIDGET (gtk_window));
#endif

  // Main window
#if GTK_MAJOR_VERSION > 3
  main_window = (GtkWindow *) gtk_window_new ();
  gtk_window_set_title (main_window, TITLE);
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_window_set_child (main_window, GTK_WIDGET (button_close));
  g_signal_connect_swapped (main_window, "destroy",
                            (GCallback) gtk_window_destroy, gtk_window);
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_window_destroy, main_window);
  gtk_window_present (main_window);
#else
  main_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (main_window, TITLE);
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (button_close));
  g_signal_connect_swapped (main_window, "destroy",
                            (GCallback) gtk_widget_destroy, gtk_window);
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_widget_destroy, main_window);
  gtk_widget_show_all (GTK_WIDGET (main_window));
#endif

  // Main loop
  g_main_loop_run (main_loop);

  // Free resources
  g_main_loop_unref (main_loop);
  return 0;
}
