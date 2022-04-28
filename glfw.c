#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "image.h"
#include "text.h"
#include "draw.h"

GLFWwindow *window;

void
draw_resize (GLFWwindow * window __attribute__((unused)), ///< GLFW window.
             int width,         ///< Graphic window width.
             int height)        ///< Graphic window height.
{
  window_width = width;
  window_height = height;
  glViewport (0, 0, width, height);
}

int
main ()
{
  GLenum e;
  if (!glfwInit ())
    {
      fprintf (stderr, "Error al inicializar GLFW\n");
      return 1;
    }
  glfwWindowHint (GLFW_SAMPLES, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
//  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow (window_width, window_height, "Prueba", NULL, NULL);
  if (!window)
    {
      fprintf (stderr, "Error al abrir una ventana GLFW\n");
      glfwTerminate ();
      return 2;
    }
  glfwMakeContextCurrent (window);
  glewExperimental = 1;
  e = glewInit ();
  if (e != GLEW_OK)
    {
      fprintf (stderr, "Error: %s\n", glewGetErrorString (e));
      return 3;
    }
  else
    printf ("GLEW version %s\n", glewGetString (GLEW_VERSION));
  glfwSetInputMode (window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetWindowSizeCallback(window, draw_resize);
	logo = image_new ("logo.png");
  if (!draw_init ())
    return 4;
  else
    do
      {
        draw_render ();
        glfwSwapBuffers (window);
        glfwPollEvents ();
      }
    while (glfwGetKey (window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && !glfwWindowShouldClose (window));
  return 0;
}
