#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <SDL.h>
#include <gtk/gtk.h>

// Window minimum size
#define MINIMUM_WIDTH 320
#define MINIMUM_HEIGHT 240
int width = MINIMUM_WIDTH;
int height = MINIMUM_HEIGHT;

// OpenGL variables
const GLfloat vertex_data[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.f, 0.5f };
const GLfloat green[] = { 0.f, 1.f, 0.f, };
GLuint program;
GLint in_position;
GLint uniform_color;
GLint uniform_matrix;
GLuint vertex_array_id;
GLuint vertex_buffer;

// Windows
GtkWindow *gtk_window;
GtkGLArea *gtk_draw = NULL;
GLFWwindow *glfw_window = NULL;
SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_context = NULL;
GMainLoop *main_loop = NULL;

// Init draw
static int
draw_init ()
{
  const GLchar *gl_version = "#version 130\n";
   const GLchar *fs_source =
    "uniform vec3 color;"
    "void main()"
    "{"
    "gl_FragColor=vec4(color,1.f);"
    "}";
  const GLchar *vs_source =
    "in vec2 position;"
    "uniform mat4 matrix;"
    "void main()"
    "{"
    "gl_Position=matrix*vec4(position,0.f,1.f);"
    "}";
  const GLchar *position_name = "position";
  const GLchar *color_name = "color";
  const GLchar *matrix_name = "matrix";
  const GLchar *fs_sources[2] = { gl_version, fs_source };
  const GLchar *vs_sources[2] = { gl_version, vs_source };
  const char *error_msg;
  GLint error_code;
  GLuint fs, vs;
  GLenum glew_status;

  // Initing GLEW library
  glew_status = glewInit ();
  if (glew_status != GLEW_OK)
    {
      error_msg = (const char *) glewGetErrorString (glew_status);
      goto end;
    }
  if (!glewIsSupported ("GL_VERSION_3_0"))
    {
      error_msg = "OpenGL 3.0 is not supported";
      goto end;
    }

  // Compiling the fragment shader
  fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 2, fs_sources, NULL);
  glCompileShader (fs);
  glGetShaderiv (fs, GL_COMPILE_STATUS, &error_code);
  if (!error_code)
    {
      error_msg = "unable to compile the fragment shader";
      goto end;
    }

  // Compiling the vertex shader
  vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 2, vs_sources, NULL);
  glCompileShader (vs);
  glGetShaderiv (vs, GL_COMPILE_STATUS, &error_code);
  if (!error_code)
    {
      glDeleteShader (fs);
      error_msg = "unable to compile the vertex shader";
      goto end;
    }

  // Linking the program
  program = glCreateProgram ();
  glAttachShader (program, fs);
  glAttachShader (program, vs);
  glLinkProgram (program);
  glDetachShader (program, vs);
  glDetachShader (program, fs);
  glDeleteShader (vs);
  glDeleteShader (fs);
  glGetProgramiv (program, GL_LINK_STATUS, &error_code);
  if (!error_code)
    {
      program = 0;
      glDeleteShader (fs);
      error_msg = "unable to link the program";
      goto end;
    }

  // Setting up attributes
  in_position = glGetAttribLocation (program, position_name);
  if (in_position == -1)
    {
      error_msg = "could not bind position attribute";
      goto end;
    }
  uniform_color = glGetUniformLocation (program, color_name);
  if (uniform_color == -1)
    {
      error_msg = "could not bind color";
      goto end;
    }
  uniform_matrix = glGetUniformLocation (program, matrix_name);
  if (uniform_matrix == -1)
    {
      error_msg = "could not bind matrix";
      goto end;
    }

  // Triangle vertex array
  glGenVertexArrays (1, &vertex_array_id);
  glBindVertexArray (vertex_array_id);
  glGenBuffers (1, &vertex_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex_data), vertex_data,
                GL_STATIC_DRAW);


  // Ending on success
  return 1;

end:

  // Ending on error
  puts (error_msg);
  return 0;
}

// Render draw
static void
draw_render ()
{
  const GLfloat identity[] = {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
  };

  // Clear screen
  glClearColor (0., 0., 0., 0.);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Triangle
  glUseProgram (program);
  glUniform3fv (uniform_color, 1, green);
  glUniformMatrix4fv (uniform_matrix, 1, GL_FALSE, identity);
  glEnableVertexAttribArray (in_position);
  glVertexAttribPointer (in_position, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glDrawArrays (GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray (in_position);
}

// Free draw
static void
draw_free ()
{
  glDeleteBuffers (1, &vertex_buffer);
  glDeleteProgram (program);
}

// Init FreeGLUT
static void
freeglut_init (int *argn, char **argc)
{
  glutInit (argn, argc);
  glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize (width, height);
  glutCreateWindow ("FreeGLUT");
  glViewport (0, 0, width, height);
}

// FreeGLUT idle function
static void
freeglut_idle ()
{
  GMainContext *context = g_main_context_default ();
  while (g_main_context_pending (context))
    g_main_context_iteration (context, 0);
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
  width = w;
  height = h;
  glViewport (0, 0, width, height);
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
  glfw_window = glfwCreateWindow (width, height, "GLFW", NULL, NULL);
  if (!glfw_window)
    {
      puts ("Unable to open the GLFW window");
      return 0;
    }
  glfwSetWindowSizeLimits (glfw_window, MINIMUM_WIDTH, MINIMUM_HEIGHT,
                           GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwMakeContextCurrent (glfw_window);
  glfwSetWindowSizeCallback (glfw_window, glfw_resize);
  glViewport (0, 0, width, height);
  return 1;
}

// GLFW loop
static void
glfw_loop ()
{
  GMainContext *context = g_main_context_default ();
  do
    {
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      glfwMakeContextCurrent (glfw_window);
      glfw_render ();
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

// SDL render function
static void
sdl_render ()
{
  draw_render ();
  SDL_GL_SwapWindow (sdl_window);
}

// Resize SDL function
static void
sdl_resize (int w, int h)
{
  unsigned int resize = 0;
  if (w < MINIMUM_WIDTH)
    {
      width = MINIMUM_WIDTH;
      resize = 1;
    }
  else
    width = w;
  if (h < MINIMUM_HEIGHT)
    {
      height = MINIMUM_HEIGHT;
      resize = 1;
    }
  else
    height = h;
  if (resize)
    SDL_SetWindowSize (sdl_window, width, height);
  glViewport (0, 0, width, height);
}

// Init SDL
static int
sdl_init ()
{
  if (SDL_Init (SDL_INIT_VIDEO))
    {
      printf ("%s: %s\n", "Unable to init SDL", SDL_GetError ());
      return 0;
    }
  sdl_window
    = SDL_CreateWindow ("SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        MINIMUM_WIDTH, MINIMUM_HEIGHT,
                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (!sdl_window)
    {
      printf ("%s: %s\n", "Unable to create SDL window", SDL_GetError ());
      return 0;
    }
  sdl_context = SDL_GL_CreateContext(sdl_window);
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  if (!SDL_GL_CreateContext (sdl_window))
    {
      printf ("%s: %s\n", "Unable to create SDL context", SDL_GetError ());
      return 0;
    }
  return 1;
}

// SDL loop
static void
sdl_loop ()
{
  SDL_Event event[1];
  GMainContext *context = g_main_context_default ();
  sdl_render ();
  while (1)
    {
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      while (SDL_PollEvent (event))
        {
          switch (event->type)
            {
            case SDL_QUIT:
              return;
            case SDL_WINDOWEVENT:
              SDL_GL_MakeCurrent (sdl_window, sdl_context);
              switch (event->window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                  sdl_resize (event->window.data1, event->window.data2);
                  // fallthrough
                default: 
                  sdl_render ();
                }
            }
        }
    }
}

// SDL free
static void
sdl_free ()
{
  draw_free ();
  SDL_GL_DeleteContext (sdl_context);
  SDL_Quit ();
}

// GTK render function
static void
gtk_render ()
{
  draw_render ();
}

// GTK resize
static void
gtk_resize (GtkGLArea * widget __attribute__((unused)), int w, int h)
{
  width = w;
  height = h;
  glViewport (0, 0, width, height);
  gtk_render ();
}

// Init GTK
static void
gtk_start ()
{
  gtk_gl_area_make_current (gtk_draw);
  if (!draw_init ())
    return;
}

// GTK loop
static void
gtk_loop ()
{
  main_loop = g_main_loop_new (NULL, 0);
  g_main_loop_run (main_loop);
}

// GTK free
static void
gtk_free ()
{
  draw_free ();
  g_main_loop_unref (main_loop);
}

// Quit loop
static void
loop_quit ()
{
  SDL_Event event[1];
  if (!gtk_draw)
    {
      if (!glfw_window)
        {
          if (!sdl_window)
            glutLeaveMainLoop ();
          else
            {
              event->type = SDL_QUIT;
              SDL_PushEvent (event);
            }
        }
      else
        glfwDestroyWindow (glfw_window);
    }
  else
    g_main_loop_quit (main_loop);
}

// Main function
int
main (int argn, char **argc)
{
  int type;

  // Checking arguments
  switch (argn)
  {
  case 2:
    break;
  default:
    puts ("The syntax is:\n./gtkopengl type");
    return 1;
  }

  // GTK main window
  gtk_init ();
  gtk_window = (GtkWindow *) gtk_window_new ();
  gtk_widget_show (GTK_WIDGET (gtk_window));
  g_signal_connect (gtk_window, "destroy", loop_quit, NULL);

  // Render window
  type = atoi (argc[1]);
  switch (type)
  {
  case 1:
    freeglut_init (&argn, argc);
    if (!draw_init ())
      {
        return 3;
      }
    draw_render ();
    freeglut_loop ();
    draw_free ();
    break;
  case 2:
    if (!glfw_init ())
      return 4;
    if (!draw_init ())
      {
        return 3;
      }
    glfw_loop ();
    glfw_free ();
    break;
  case 3:
    if (!sdl_init ())
      return 4;
    if (!draw_init ())
      {
        return 3;
      }
    sdl_loop ();
    sdl_free ();
    break;
  case 4:
    gtk_draw = (GtkGLArea *) gtk_gl_area_new ();
    gtk_widget_set_size_request (GTK_WIDGET (gtk_draw), MINIMUM_WIDTH,
                                 MINIMUM_HEIGHT);
    g_signal_connect (gtk_draw, "realize", (GCallback) gtk_start, NULL);
    g_signal_connect (gtk_draw, "resize", (GCallback) gtk_resize, NULL);
    g_signal_connect (gtk_draw, "render", (GCallback) gtk_render, NULL);
    gtk_window_set_child (gtk_window, GTK_WIDGET (gtk_draw));
    gtk_window_set_title (gtk_window, "GtkGLArea");
    gtk_widget_show (GTK_WIDGET (gtk_window));
    gtk_loop ();
    gtk_free ();
    break;
  default:
    puts ("Unknown type (1: FreeGLUT, 2: GLFW, 3: SDL, 4: GTK)");
    return 2;
  }
  return 0;
}
