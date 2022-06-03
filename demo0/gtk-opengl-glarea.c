#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
#include <gtk/gtk.h>

#include "draw.h"

// Windows
GtkWindow *gtk_window;
GtkGLArea *gtk_draw = NULL;

// GTK resize
static void
glarea_resize (GtkGLArea * widget __attribute__((unused)), int w, int h)
{
  window_width = w;
  window_height = h;
  glViewport (0, 0, window_width, window_height);
#if GTK_MAJOR_VERSION < 4
  gtk_widget_queue_draw (GTK_WIDGET (widget));
#endif
}

// GTK realize
static void
glarea_realize ()
{
  gtk_gl_area_make_current (gtk_draw);
  if (!draw_init ())
    return;
}

// Main function
int
main (int argn __attribute__((unused)), char **argc __attribute__((unused)))
{
  GMainLoop *main_loop;
  main_loop = g_main_loop_new (NULL, 0);
#if GTK_MAJOR_VERSION > 3
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_window_set_title (gtk_window, "GtkGLArea");
  gtk_draw = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (gtk_draw), MINIMUM_WIDTH,
                               MINIMUM_HEIGHT);
  gtk_window_set_child (gtk_window, GTK_WIDGET (gtk_draw));
  g_signal_connect_swapped (gtk_window, "destroy",
                            (GCallback) g_main_loop_quit, main_loop);
  g_signal_connect (gtk_draw, "realize", (GCallback) glarea_realize, NULL);
  g_signal_connect (gtk_draw, "resize", (GCallback) glarea_resize, NULL);
  g_signal_connect (gtk_draw, "render", (GCallback) draw_render, NULL);
  gtk_widget_show (GTK_WIDGET (gtk_window));
#else
  gtk_init (&argn, &argc);
  gtk_window = (GtkWindow *) gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (gtk_window, "GtkGLArea");
  gtk_draw = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (gtk_draw), MINIMUM_WIDTH,
                               MINIMUM_HEIGHT);
  gtk_container_add (GTK_CONTAINER (gtk_window), GTK_WIDGET (gtk_draw));
  g_signal_connect_swapped (gtk_window, "destroy",
                            (GCallback) g_main_loop_quit, main_loop);
  g_signal_connect (gtk_draw, "realize", (GCallback) glarea_realize, NULL);
  g_signal_connect (gtk_draw, "resize", (GCallback) glarea_resize, NULL);
  g_signal_connect (gtk_draw, "render", (GCallback) draw_render, NULL);
  gtk_widget_show_all (GTK_WIDGET (gtk_window));
#endif
  g_main_loop_run (main_loop);
  draw_free ();
  g_main_loop_unref (main_loop);
  return 0;
}
