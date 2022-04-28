#include <stdio.h>
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <gtk/gtk.h>

#include "image.h"
#include "text.h"
#include "draw.h"

static void
draw_resize (GtkGLArea * gl_area __attribute__((unused)),
             ///< GtkGLArea widget.
             int width,         ///< Graphic window width.
             int height)        ///< Graphic window height.
{
  window_width = width;
  window_height = height;
  glViewport (0, 0, width, height);
}

static void
draw_realize (GtkGLArea * gl_area)
{
  GLenum e;
  gtk_gl_area_make_current (gl_area);
  if (gtk_gl_area_get_error (gl_area) != NULL)
    printf ("realizing error\n");
  e = glewInit ();
  if (e != GLEW_OK)
    printf ("Error: %s\n", glewGetErrorString (e));
  else
    printf ("GLEW version %s\n", glewGetString (GLEW_VERSION));
  logo = image_new ("logo.png");
  draw_init ();
}

static void
application_activate (GtkApplication * application,
                      gpointer data __attribute__((unused)))
{
  GtkApplicationWindow *window;
  GtkGLArea *graphic;
  GtkButton *button;
  GtkBox *box;
  window = (GtkApplicationWindow *) gtk_application_window_new (application);
  graphic = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (graphic), 640, 480);
  gtk_window_set_title (GTK_WINDOW (window), "Prueba");
  button = (GtkButton *) gtk_button_new_with_label ("OK");
  box = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#if GTK4
  gtk_box_append (box, GTK_WIDGET (graphic));
  gtk_box_append (box, GTK_WIDGET (button));
  gtk_window_set_child (GTK_WINDOW (window), GTK_WIDGET (box));
  g_signal_connect (graphic, "realize", G_CALLBACK (draw_realize), NULL);
  g_signal_connect (graphic, "resize", G_CALLBACK (draw_resize), NULL);
  g_signal_connect (graphic, "render", G_CALLBACK (draw_render), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (g_application_quit), 
                            G_APPLICATION (application));
  gtk_widget_show (GTK_WIDGET (window));
#else
  gtk_box_pack_start (box, GTK_WIDGET (graphic), TRUE, TRUE, 0);
  gtk_box_pack_start (box, GTK_WIDGET (button), FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (box));
  g_signal_connect (graphic, "realize", G_CALLBACK (draw_realize), NULL);
  g_signal_connect (graphic, "resize", G_CALLBACK (draw_resize), NULL);
  g_signal_connect (graphic, "render", G_CALLBACK (draw_render), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (g_application_quit), 
                            G_APPLICATION (application));
  gtk_widget_show_all (GTK_WIDGET (window));
#endif
}

int
main (int argn, char **argc)
{
  GtkApplication *application;
  int status;
  application = gtk_application_new ("es.csic.eead.prueba", 0);
  g_signal_connect (application, "activate", G_CALLBACK (application_activate),
                    NULL);
  status = g_application_run (G_APPLICATION (application), argn, argc);
  g_object_unref (application);
  return status;
}
