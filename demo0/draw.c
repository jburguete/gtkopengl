/**
 * \file draw.c
 * \brief Source file with functions and variables for initializing, rendering,
 *   and freeing related resources to draw a triangle with OpenGL.
 * \author Javier Burguete Tolosa.
 * \date 2022-2025.
 * \license BSD-2-Clause.
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <epoxy/gl.h>
#include "config.h"
#include "draw.h"

// OpenGL variables

///> vertex data for the triangle.
const GLfloat vertex_data[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.f, 0.5f };

///> green color.
const GLfloat green[] = { 0.f, 1.f, 0.f, };

GLuint program;                 ///< OpenGL program.
GLint in_position;              ///< attribute location for position.
GLint uniform_color;            ///< uniform location for color.
GLuint vertex_array_id;         ///< vertex array object ID.
GLuint vertex_buffer;           ///< vertex buffer object ID.

unsigned int window_width = MINIMUM_WIDTH;      ///< window width.
unsigned int window_height = MINIMUM_HEIGHT;    ///< window heigth.

/**
 * Initializes the draw resources.
 *
 * \return 1 if successful, 0 otherwise.
 */
int
draw_init ()
{
  const GLchar *gl_version = "#version 130\n";
  const GLchar *gl_version_es = "#version 100\n";
  const GLchar *fs_source =
    "uniform vec3 color;" "void main(){gl_FragColor=vec4(color,1.f);}";
  const GLchar *fs_source_es =
    "precision mediump float;"
    "uniform vec3 color;" "void main(){gl_FragColor=vec4(color,1.f);}";
  const GLchar *vs_source =
    "in vec2 position;"
    "void main(){gl_Position=vec4(position,0.f,1.f);}";
  const GLchar *vs_source_es =
    "attribute vec2 position;"
    "void main(){gl_Position=vec4(position,0.f,1.f);}";
  const GLchar *position_name = "position";
  const GLchar *color_name = "color";
  const GLchar *fs_sources[2], *vs_sources[2];
  const GLubyte *version;
  const char *error_msg;
  GLint error_code;
  GLuint fs, vs;

#if DEBUG
  fprintf (stderr, "draw_init: start\n");
#endif

  // OpenGL version
  version = glGetString (GL_VERSION);
  printf ("OpenGL=%s\n", version);
  if (strstr ((const char *) version, "OpenGL ES"))
    {
      fs_sources[0] = gl_version_es;
      fs_sources[1] = fs_source_es;
      vs_sources[0] = gl_version_es;
      vs_sources[1] = vs_source_es;
    }
  else
    {
      fs_sources[0] = gl_version;
      fs_sources[1] = fs_source;
      vs_sources[0] = gl_version;
      vs_sources[1] = vs_source;
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

/**
 * Renders the draw scene.
 */
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

  // Swap buffers
  glFlush ();

#if DEBUG
  fprintf (stderr, "draw_render: end\n");
#endif

}

/**
 * Free the draw resources.
 */
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
