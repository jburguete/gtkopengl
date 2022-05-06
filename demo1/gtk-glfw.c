#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
GLuint vertex_array_id;
GLuint vertex_buffer;

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context;
GLFWwindow *glfw_window = NULL;

// Init draw
static int
draw_init ()
{
  const GLchar *gl_version = "#version 130\n";
  const GLchar *fs_source =
    "uniform vec3 color;" "void main()" "{" "gl_FragColor=vec4(color,1.f);" "}";
  const GLchar *vs_source =
    "in vec2 position;"
    "void main()" "{" "gl_Position=vec4(position,0.f,1.f);" "}";
  const GLchar *position_name = "position";
  const GLchar *color_name = "color";
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
  // Clear screen
  glClearColor (0., 0., 0., 1.);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Triangle
  glUseProgram (program);
  glUniform3fv (uniform_color, 1, green);
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
  glfwSetFramebufferSizeCallback (glfw_window, glfw_resize);
  glfwSetWindowRefreshCallback (glfw_window, glfw_render);
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
      gdk_gl_context_make_current (gl_context);
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      glfwMakeContextCurrent (glfw_window);
      glfwPollEvents ();
    }
  while (!glfwWindowShouldClose (glfw_window));
}

// GLFW quit
static void
glfw_quit ()
{
  glfwSetWindowShouldClose (glfw_window, 1);
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
  g_signal_connect (gtk_window, "destroy", (GCallback) glfw_quit, NULL);

  // Render window
  if (!glfw_init ())
    return 4;
  if (!draw_init ())
    return 3;
  glfw_loop ();
  glfw_free ();
  return 0;
}
