#include <stdio.h>
#include <GL/glew.h>
#include "config.h"
#include "draw.h"

// OpenGL variables
const GLfloat vertex_data[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.f, 0.5f };
const GLfloat green[] = { 0.f, 1.f, 0.f, };

GLuint program;
GLint in_position;
GLint uniform_color;
GLuint vertex_array_id;
GLuint vertex_buffer;

unsigned int window_width = MINIMUM_WIDTH;
unsigned int window_height = MINIMUM_HEIGHT;

// Init draw
int
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

#if DEBUG
  fprintf (stderr, "draw_init: start\n");
#endif

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

#if DEBUG
  fprintf (stderr, "draw_init: end on success\n");
#endif

  return 1;

end:

  // Ending on error

#if DEBUG
  fprintf (stderr, "draw_init: end on error\n");
#endif

  puts (error_msg);
  return 0;
}

// Render draw
void
draw_render ()
{

#if DEBUG
  fprintf (stderr, "draw_render: start\n");
#endif

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

#if DEBUG
  fprintf (stderr, "draw_render: end\n");
#endif

}

// Free draw
void
draw_free ()
{

#if DEBUG
  fprintf (stderr, "draw_free: start\n");
#endif

  glDeleteBuffers (1, &vertex_buffer);
  glDeleteProgram (program);

#if DEBUG
  fprintf (stderr, "draw_free: end\n");
#endif

}
