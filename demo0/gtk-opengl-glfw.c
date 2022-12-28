#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "config.h"
#include "draw.h"

// Windows
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
  fprintf (stderr, "glfw_resize: start\n");
#endif

  window_width = w;
  window_height = h;
  glViewport (0, 0, w, h);

#if DEBUG
  fprintf (stderr, "glfw_resize: end\n");
#endif

}

// Init GLFW
static int
glfw_init ()
{
  const char *msg;
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
  fprintf (stderr, "glfw_resize: end on success\n");
#endif

  return 1;

end:

#if DEBUG
  fprintf (stderr, "glfw_resize: end on error\n");
#endif

  puts (msg);
  return 0;
}

// GLFW loop
static void
glfw_loop ()
{

#if DEBUG
  fprintf (stderr, "glfw_loop: start\n");
#endif

  while (!glfwWindowShouldClose (glfw_window))
    {
      glfwMakeContextCurrent (glfw_window);
      glfwPollEvents ();
    }

#if DEBUG
  fprintf (stderr, "glfw_loop: end\n");
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
main ()
{
  if (!glfw_init ())
    return 4;
  if (!draw_init ())
    return 3;
  glfw_loop ();
  glfw_free ();
  return 0;
}
