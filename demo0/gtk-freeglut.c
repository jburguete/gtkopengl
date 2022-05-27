#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "draw.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Window minimum size
#define MINIMUM_WIDTH 320
#define MINIMUM_HEIGHT 240
int width = MINIMUM_WIDTH;
int height = MINIMUM_HEIGHT;

// Windows
int glut_window;

// Init FreeGLUT
static void
freeglut_init (int *argn, char **argc)
{
  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (width, height);
  glut_window = glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, width, height);
}

// FreeGLUT idle function
static void
freeglut_idle ()
{
  glutSetWindow (glut_window);
}

// FreeGLUT resize function
static void
freeglut_resize (int w, int h)
{
  width = MAX (w, MINIMUM_WIDTH);
  height = MAX (h, MINIMUM_HEIGHT);
  glutReshapeWindow (width, height);
  glViewport (0, 0, width, height);
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
  freeglut_init (&argn, argc);
  if (!draw_init ())
    return 3;
  draw_render ();
  freeglut_loop ();
  draw_free ();
  return 0;
}
