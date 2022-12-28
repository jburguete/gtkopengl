#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtk/gtk.h>
#include "config.h"
#include "draw.h"

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context;
GLFWwindow *glfw_window = NULL;

// GLFW render function
static void
glfw_render ()
{

#if DEBUG
  fprintf (stderr, "glfw_render: start\n");
#endif

  draw_render ();
  glfwSwapBuffers (glfw_window);

#if DEBUG
  fprintf (stderr, "glfw_render: end\n");
#endif

}

// Resize GLFW window
static void
glfw_resize (GLFWwindow * window __attribute__((unused)), int w, int h)
{

#if DEBUG
  fprintf (stderr, "glfw_realize: start\n");
#endif

  window_width = w;
  window_height = h;
  glViewport (0, 0, w, h);

#if DEBUG
  fprintf (stderr, "glfw_realize: end\n");
#endif

}

// Init GLFW
static int
glfw_init ()
{
  const char *msg;

#if DEBUG
  fprintf (stderr, "glfw_init: start\n");
#endif

  if (!glfwInit ())
    {
      msg = "Unable to init GLFW";
      goto end;
    }
  glfw_window
    = glfwCreateWindow (window_width, window_height, "GLFW", NULL, NULL);
  if (!glfw_window)
    {
      msg = "Unable to open the GLFW window";
      goto end;
    }
  glfwSetWindowSizeLimits (glfw_window, MINIMUM_WIDTH, MINIMUM_HEIGHT,
                           GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwMakeContextCurrent (glfw_window);
  glfwSetFramebufferSizeCallback (glfw_window, glfw_resize);
  glfwSetWindowRefreshCallback (glfw_window, glfw_render);
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "glfw_init: end on success\n");
#endif

  return 1;

end:

#if DEBUG
  fprintf (stderr, "glfw_init: end on error\n");
#endif

  puts (msg);
  return 0;
}

// GLFW loop
static void
glfw_loop ()
{
  GMainContext *context;

#if DEBUG
  fprintf (stderr, "glfw_loop: start\n");
#endif

  context = g_main_context_default ();
  do
    {
      if (gl_context)
        gdk_gl_context_make_current (gl_context);
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      glfwMakeContextCurrent (glfw_window);
      glfwPollEvents ();
    }
  while (!glfwWindowShouldClose (glfw_window));

#if DEBUG
  fprintf (stderr, "glfw_loop: end\n");
#endif

}

// GLFW quit
static void
glfw_quit ()
{

#if DEBUG
  fprintf (stderr, "glfw_quit: start\n");
#endif

  glfwSetWindowShouldClose (glfw_window, 1);

#if DEBUG
  fprintf (stderr, "glfw_quit: end\n");
#endif

}

// GLFW free
static void
glfw_free ()
{

#if DEBUG
  fprintf (stderr, "glfw_free: start\n");
#endif

  draw_free ();
  glfwTerminate ();

#if DEBUG
  fprintf (stderr, "glfw_free: end\n");
#endif

}

// Main function
int
main (int argn __attribute__((unused)), char **argc __attribute__((unused)))
{
  GtkButton *button_close;

  // Render window
  if (!glfw_init ())
    return 4;
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
  gtk_widget_show (GTK_WIDGET (gtk_window));
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
  g_signal_connect (gtk_window, "destroy", (GCallback) glfw_quit, NULL);

  // Main loop
  glfw_loop ();
  glfw_free ();
  return 0;
}
