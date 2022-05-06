#include <stdio.h>
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtk/gtk.h>

#include "image.h"
#include "text.h"
#include "draw.h"

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context;
GLFWwindow *glfw_window = NULL;

// GLFW render function
static void
glfw_render ()
{
  draw_render ();
  glfwSwapBuffers (glfw_window);
}

// Resize GLFW window
static void
glfw_resize (GLFWwindow * window __attribute__((unused)), int w, int h)
{
  window_width = w;
  window_height = h;
  glViewport (0, 0, w, h);
  glfw_render ();
}

// Init GLFW
static int
glfw_init ()
{
  if (!glfwInit ())
    {
      puts ("Unable to init GLFW");
      return 0;
    }
  glfw_window
    = glfwCreateWindow (window_width, window_height, "GLFW", NULL, NULL);
  if (!glfw_window)
    {
      puts ("Unable to open the GLFW window");
      return 0;
    }
  glfwSetWindowSizeLimits (glfw_window, MINIMUM_WIDTH, MINIMUM_HEIGHT,
                           GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwMakeContextCurrent (glfw_window);
  glfwSetFramebufferSizeCallback (glfw_window, glfw_resize);
  glfwSetWindowRefreshCallback (glfw_window, glfw_render);
  glViewport (0, 0, window_width, window_height);
  return 1;
}

// GLFW loop
static void
glfw_loop ()
{
  GMainContext *context = g_main_context_default ();
  do
    {
      gdk_gl_context_make_current (gl_context);
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      glfwMakeContextCurrent (glfw_window);
      glfwPollEvents ();
    }
  while (!glfwWindowShouldClose (glfw_window));
}

// GLFW free
static void
glfw_free ()
{
  draw_free ();
  glfwTerminate ();
}

// Main function
int
main (int argn __attribute__((unused)), char **argc __attribute__((unused)))
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
  gl_context = gdk_gl_context_get_current ();

  // Render window
  if (!glfw_init ())
    return 4;
  if (!draw_init ())
    return 3;
  g_signal_connect_swapped (gtk_window, "destroy",
                            (GCallback) glfwDestroyWindow, glfw_window);
  glfw_loop ();
  glfw_free ();

  return 0;
}
