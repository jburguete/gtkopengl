/**
 * \file gtk-opengl-freeglut.c
 * \brief Source file with functions and variables to draw a triangle with
 *   OpenGL and FreeGLUT.
 * \author Javier Burguete Tolosa.
 * \date 2022-2023.
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
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gtk/gtk.h>
#include "config.h"
#include "draw.h"

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context;
int glut_window;
///< FreeGLUT window ID.

/**
 * Init FreeGLUT.
 */
static void
freeglut_init (int *argn,       ///< number of command line arguments.
               char **argc)     ///< array of command line arguments.
{

#if DEBUG
  fprintf (stderr, "freeglut_init: start\n");
#endif

  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (window_width, window_height);
  glut_window = glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "freeglut_init: end\n");
#endif

}

/**
 * FreeGLUT idle function.
 */
static void
freeglut_idle ()
{
  GMainContext *context;

#if DEBUG
  fprintf (stderr, "freeglut_idle: start\n");
#endif

  context = g_main_context_default ();
  if (gl_context)
    gdk_gl_context_make_current (gl_context);
  while (g_main_context_pending (context))
    g_main_context_iteration (context, 0);
  glutSetWindow (glut_window);

#if DEBUG
  fprintf (stderr, "freeglut_idle: end\n");
#endif

}

/**
 * FreeGLUT resize function.
 */
static void
freeglut_resize (int w,         ///< new window width.
                 int h)         ///< new window height.
{

#if DEBUG
  fprintf (stderr, "freeglut_resize: start\n");
#endif

  window_width = MAX (w, MINIMUM_WIDTH);
  window_height = MAX (h, MINIMUM_HEIGHT);
  glutReshapeWindow (window_width, window_height);
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "freeglut_resize: end\n");
#endif

}

/**
 * FreeGLUT render function.
 */
static void
freeglut_render ()
{

#if DEBUG
  fprintf (stderr, "freeglut_render: start\n");
#endif

  draw_render ();
  glutSwapBuffers ();

#if DEBUG
  fprintf (stderr, "freeglut_render: end\n");
#endif

}

/**
 * FreeGLUT loop.
 */
static void
freeglut_loop ()
{

#if DEBUG
  fprintf (stderr, "freeglut_loop: start\n");
#endif

  glutIdleFunc (freeglut_idle);
  glutReshapeFunc (freeglut_resize);
  glutDisplayFunc (freeglut_render);
  glutMainLoop ();
  glutDestroyWindow (glut_window);

#if DEBUG
  fprintf (stderr, "freeglut_loop: end\n");
#endif

}

/**
 * Main function.
 *
 * \return exit status.
 */
int
main (int argn,                 ///< number of command line arguments.
      char **argc)              ///< array of command line arguments.
{
  GtkButton *button_close;

  // Render window
  freeglut_init (&argn, argc);
  if (!draw_init ())
    return 3;

  // GTK main window
#if GTK_MAJOR_VERSION > 3
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_window_set_title (gtk_window, "GTK");
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_window_set_child (gtk_window, GTK_WIDGET (button_close));
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_window_destroy, gtk_window);
  printf ("gtk_widget_show\n");
  gtk_window_present (gtk_window);
#else
  gtk_init (&argn, &argc);
  gtk_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (gtk_window, "GTK");
  button_close = (GtkButton *) gtk_button_new_with_mnemonic ("_Close");
  gtk_container_add (GTK_CONTAINER (gtk_window), GTK_WIDGET (button_close));
  g_signal_connect_swapped (button_close, "clicked",
                            (GCallback) gtk_widget_destroy, gtk_window);
  gtk_widget_show_all (GTK_WIDGET (gtk_window));
#endif
  gl_context = gdk_gl_context_get_current ();
  if (!gl_context)
    printf ("GdkGLContext: NULL\n");

  g_signal_connect (gtk_window, "destroy", glutLeaveMainLoop, NULL);

  // Main loop
  freeglut_loop ();

  // Free resources
  draw_free ();
  return 0;
}
