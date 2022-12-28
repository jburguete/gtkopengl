#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "config.h"
#include "draw.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Windows
int glut_window;

// Init FreeGLUT
static void
freeglut_init (int *argn, char **argc)
{

#if DEBUG
  fprintf (stderr, "freeglut_init: start\n");
#endif

  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (window_width, window_height);
  glut_window = glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "freeglut_init: end\n");
#endif

}

// FreeGLUT idle function
static void
freeglut_idle ()
{

#if DEBUG
  fprintf (stderr, "freeglut_idle: start\n");
#endif

  glutSetWindow (glut_window);

#if DEBUG
  fprintf (stderr, "freeglut_idle: end\n");
#endif

}

// FreeGLUT resize function
static void
freeglut_resize (int w, int h)
{

#if DEBUG
  fprintf (stderr, "freeglut_resize: start\n");
#endif

  window_width = MAX (w, MINIMUM_WIDTH);
  window_height = MAX (h, MINIMUM_HEIGHT);
  glutReshapeWindow (window_width, window_height);
  glViewport (0, 0, window_width, window_height);

#if DEBUG
  fprintf (stderr, "freeglut_resize: end\n");
#endif

}

// FreeGLUT render function
static void
freeglut_render ()
{

#if DEBUG
  fprintf (stderr, "freeglut_render: start\n");
#endif

  draw_render ();
  glutSwapBuffers ();

#if DEBUG
  fprintf (stderr, "freeglut_render: end\n");
#endif

}

// FreeGLUT loop
static void
freeglut_loop ()
{

#if DEBUG
  fprintf (stderr, "freeglut_loop: start\n");
#endif

  glutIdleFunc (freeglut_idle);
  glutReshapeFunc (freeglut_resize);
  glutDisplayFunc (freeglut_render);
  glutMainLoop ();

#if DEBUG
  fprintf (stderr, "freeglut_loop: end\n");
#endif

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
