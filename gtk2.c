#include <stdio.h>
#include <glib.h>
#include <GL/glew.h>
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

// Init draw
static int
draw_init ()
{
  const GLchar *gl_version = "#version 120\n#define in attribute\n";
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

// Resize draw
static void
gtk_draw_resize (GtkGLArea * gl_area __attribute__((unused)),
                 int w,
                 int h)
{
  width = w;
  height = h;
  glViewport (0, 0, w, h);
  draw_render ();
}

static void
gtk_draw_realize (GtkGLArea * gl_area)
{
  gtk_gl_area_make_current (gl_area);
  if (gtk_gl_area_get_error (gl_area) != NULL)
    printf ("realizing error\n");
  draw_init ();
}

static void
application_activate (GtkApplication * application,
                      gpointer data __attribute__((unused)))
{
  GtkApplicationWindow *window;
  GtkGLArea *graphic;
  GtkButton *button;
  GtkBox *box;
  window = (GtkApplicationWindow *) gtk_application_window_new (application);
  graphic = (GtkGLArea *) gtk_gl_area_new ();
  gtk_widget_set_size_request (GTK_WIDGET (graphic), 640, 480);
  gtk_window_set_title (GTK_WINDOW (window), "Prueba");
  button = (GtkButton *) gtk_button_new_with_label ("OK");
  box = (GtkBox *) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
#if GTK4
  gtk_box_append (box, GTK_WIDGET (graphic));
  gtk_box_append (box, GTK_WIDGET (button));
  gtk_window_set_child (GTK_WINDOW (window), GTK_WIDGET (box));
  g_signal_connect (graphic, "realize", G_CALLBACK (gtk_draw_realize), NULL);
  g_signal_connect (graphic, "resize", G_CALLBACK (gtk_draw_resize), NULL);
  g_signal_connect (graphic, "render", G_CALLBACK (draw_render), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (g_application_quit), 
                            G_APPLICATION (application));
  gtk_widget_show (GTK_WIDGET (window));
#else
  gtk_box_pack_start (box, GTK_WIDGET (graphic), TRUE, TRUE, 0);
  gtk_box_pack_start (box, GTK_WIDGET (button), FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (box));
  g_signal_connect (graphic, "realize", G_CALLBACK (gtk_draw_realize), NULL);
  g_signal_connect (graphic, "resize", G_CALLBACK (gtk_draw_resize), NULL);
  g_signal_connect (graphic, "render", G_CALLBACK (draw_render), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (g_application_quit), 
                            G_APPLICATION (application));
  gtk_widget_show_all (GTK_WIDGET (window));
#endif
}

int
main (int argn, char **argc)
{
  GtkApplication *application;
  int status;
  application = gtk_application_new ("es.csic.eead.prueba", 0);
  g_signal_connect (application, "activate", G_CALLBACK (application_activate),
                    NULL);
  status = g_application_run (G_APPLICATION (application), argn, argc);
  g_object_unref (application);
  draw_free ();
  return status;
}
