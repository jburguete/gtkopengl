#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "draw.h"

// Window minimum size
#define MINIMUM_WIDTH 320
#define MINIMUM_HEIGHT 240
int window_width = MINIMUM_WIDTH;
int window_height = MINIMUM_HEIGHT;

// Windows
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
  while (!glfwWindowShouldClose (glfw_window))
    {
      glfwMakeContextCurrent (glfw_window);
      glfwPollEvents ();
    }
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
