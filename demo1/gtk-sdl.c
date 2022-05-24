#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
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
GLuint vertex_array_id;
GLuint vertex_buffer;

// Windows
GtkWindow *gtk_window;
GdkGLContext *gl_context;
SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_context = NULL;

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
  SDL_SetWindowSize (sdl_window, width, height);
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetSwapInterval (1);
  sdl_context = SDL_GL_CreateContext (sdl_window);
  if (!sdl_context)
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
  while (1)
    {
      if (gl_context)
        gdk_gl_context_make_current (gl_context);
      while (g_main_context_pending (context))
        g_main_context_iteration (context, 0);
      SDL_RaiseWindow (sdl_window);
      SDL_GL_MakeCurrent (sdl_window, sdl_context);
      while (SDL_PollEvent (event))
        {
          switch (event->type)
            {
            case SDL_QUIT:
              return;
            case SDL_WINDOWEVENT:
              switch (event->window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                  sdl_resize (event->window.data1, event->window.data2);
                }
            }
          sdl_render ();
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

// Quit loop
static void
sdl_loop_quit ()
{
  SDL_Event event[1];
  event->type = SDL_QUIT;
  SDL_PushEvent (event);
}

// Main function
int
main (int argn __attribute__((unused)), char **argc __attribute__((unused)))
{
  GtkButton *button_close;

  // Render window
  if (!sdl_init ())
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
  g_signal_connect (gtk_window, "destroy", sdl_loop_quit, NULL);

  // Main loop
  sdl_loop ();
  sdl_free ();
  return 0;
}
