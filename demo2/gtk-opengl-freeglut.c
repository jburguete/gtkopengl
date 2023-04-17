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
GdkGLContext *gl_context;
int glut_window;

// Init FreeGLUT
static void
freeglut_init (int *argn, char **argc)
{
  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (window_width, window_height);
  glut_window = glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, window_width, window_height);
}

// FreeGLUT idle function
static void
freeglut_idle ()
{
  GMainContext *context = g_main_context_default ();
  if (gl_context)
    gdk_gl_context_make_current (gl_context);
  while (g_main_context_pending (context))
    g_main_context_iteration (context, 0);
  glutSetWindow (glut_window);
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
  g_signal_connect (gtk_window, "destroy", glutLeaveMainLoop, NULL);

  // Main loop
  freeglut_loop ();
  draw_free ();
  return 0;
}
