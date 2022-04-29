#include <stdio.h>
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <gtk/gtk.h>

#include "image.h"
#include "text.h"
#include "draw.h"

// Windows
GtkWindow *gtk_window;

// Init FreeGLUT
static void
freeglut_init (int *argn, char **argc)
{
  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (window_width, window_height);
  glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, window_width, window_height);
}

// FreeGLUT idle function
static void
freeglut_idle ()
{
  GMainContext *context = g_main_context_default ();
  while (g_main_context_pending (context))
    g_main_context_iteration (context, 0);
}

// FreeGLUT resize function
static void
freeglut_resize (int w, int h)
{
  window_width = MAX (w, MINIMUM_WIDTH);
  window_height = MAX (h, MINIMUM_HEIGHT);
  glutReshapeWindow (window_width, window_height);
  glViewport (0, 0, window_width, window_height);
}

// FreeGLUT render function
static void
freeglut_render ()
{
  draw_render ();
  glutSwapBuffers ();
}

// FreeGLUT loop
static void
freeglut_loop ()
{
  glutIdleFunc (freeglut_idle);
  glutReshapeFunc (freeglut_resize);
  glutDisplayFunc (freeglut_render);
  glutMainLoop ();
}

// Main function
int
main (int argn, char **argc)
{
  // GTK main window
#if GTK_MAJOR_VERSION > 3
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_widget_show (GTK_WIDGET (gtk_window));
#else
  gtk_init (&argn, &argc);
  gtk_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_show_all (GTK_WIDGET (gtk_window));
#endif
  g_signal_connect (gtk_window, "destroy", glutLeaveMainLoop, NULL);

  // Render window
  freeglut_init (&argn, argc);
  if (!draw_init ())
    return 3;
  draw_render ();
  freeglut_loop ();
  draw_free ();

  return 0;
}
