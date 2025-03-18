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
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <epoxy/gl.h>

#include "image.h"
#include "text.h"
#include "draw.h"

#define NFRAGMENT_V3 4
const char *fragment_shader_source_v3[NFRAGMENT_V3] = {
  "#version 330 core\n",
  "uniform vec3 color;",
  "out vec3 fcolor;",
  "void main(){fcolor=color;}"
};

#define NVERTEX_V3 4
const char *vertex_shader_source_v3[NVERTEX_V3] = {
  "#version 330 core\n",
  "in vec3 vertex;",
  "uniform mat4 matrix;",
  "void main(){gl_Position=matrix*vec4(vertex,1.);}"
};

#define NFRAGMENT_V2 3
const char *fragment_shader_source_v2[NFRAGMENT_V2] = {
  "#version 120\n",
  "uniform vec3 color;",
  "void main(){gl_FragColor=vec4(color,0.);}"
};

#define NVERTEX_V2 4
const char *vertex_shader_source_v2[NVERTEX_V2] = {
  "#version 120\n",
  "attribute vec3 vertex;",
  "uniform mat4 matrix;",
  "void main(){gl_Position=matrix*vec4(vertex,1.);}"
};

#define NFRAGMENT_ES 4
const char *fragment_shader_source_es[NFRAGMENT_ES] = {
  "#version 100\n",
  "precision mediump float;",
  "uniform vec3 color;",
  "void main(){gl_FragColor=vec4(color,0.);}"
};

#define NVERTEX_ES 4
const char *vertex_shader_source_es[NVERTEX_ES] = {
  "#version 100\n",
  "attribute vec3 vertex;",
  "uniform mat4 matrix;",
  "void main(){gl_Position=matrix*vec4(vertex,1.);}"
};

const GLfloat red[4] = { 1.f, 0.f, 0.f, 1.f };
const GLfloat green[4] = { 0.f, 1.f, 0.f, 1.f };
const GLfloat blew[4] = { 0.f, 0.f, 1.f, 1.f };

const GLfloat identity[16] = {
  1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, 0.f, 1.f
};

const GLfloat vertex1_data[] = {
  -1.0f, -1.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  0.0f, 1.0f, 0.0f
};

const GLfloat vertex2_data[] = {
  -1.0f, 1.0f, 0.5f,
  1.0f, 1.0f, 0.5f,
  0.0f, -1.0f, 0.5f
};

GLuint fragment_shader_id;
GLuint vertex_shader_id;
GLuint program_id;
GLint color_id;
GLint matrix_id;
GLuint vertex1_array_id;
GLuint vertex2_array_id;
GLuint vertex1_buffer;
GLuint vertex2_buffer;

Image *logo;                    ///< Logo data.
Text text[1];                   ///< Text data.

unsigned int window_width = MINIMUM_WIDTH;
unsigned int window_height = MINIMUM_HEIGHT;

// Init draw
int
draw_init ()
{
  const char **fragment_shader_source, **vertex_shader_source;
  const char *error_message;
  GLint result;
  GLuint nfragment, nvertex;
  const GLubyte *version;

  // OpenGL version
  version = glGetString (GL_VERSION);
  printf ("OpenGL=%s\n", version);

  // Opening logo
  logo = image_new ("logo.png");
  if (!logo)
    {
      error_message = "Unable to open the logo";
      goto exit_on_error;
    }

  // Select shaders
  if (strstr ((const char *) version, "OpenGL ES"))
    {
      nfragment = NFRAGMENT_ES;
      fragment_shader_source = fragment_shader_source_es;
      nvertex = NVERTEX_ES;
      vertex_shader_source = vertex_shader_source_es;
    }
  else if (epoxy_gl_version () >= 33)
    {
      nfragment = NFRAGMENT_V3;
      fragment_shader_source = fragment_shader_source_v3;
      nvertex = NVERTEX_V3;
      vertex_shader_source = vertex_shader_source_v3;
    }
  else
    {
      nfragment = NFRAGMENT_V2;
      fragment_shader_source = fragment_shader_source_v2;
      nvertex = NVERTEX_V2;
      vertex_shader_source = vertex_shader_source_v2;
    }

  // Enable
  glEnable (GL_DEPTH_TEST);

  // Fragment shacer
  fragment_shader_id = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fragment_shader_id, nfragment, fragment_shader_source, NULL);
  glCompileShader (fragment_shader_id);
  glGetShaderiv (fragment_shader_id, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      error_message = "Unable to compile fragment shader";
      goto exit_on_error;
    }

  // Vertex shader
  vertex_shader_id = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vertex_shader_id, nvertex, vertex_shader_source, NULL);
  glCompileShader (vertex_shader_id);
  glGetShaderiv (vertex_shader_id, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      error_message = "Unable to compile vertex shader";
      goto exit_on_error;
    }

  // Program
  program_id = glCreateProgram ();
  glAttachShader (program_id, fragment_shader_id);
  glAttachShader (program_id, vertex_shader_id);
  glLinkProgram (program_id);
  glDetachShader (program_id, vertex_shader_id);
  glDetachShader (program_id, fragment_shader_id);
  glDeleteShader (fragment_shader_id);
  glDeleteShader (vertex_shader_id);
  glGetProgramiv (program_id, GL_LINK_STATUS, &result);
  if (!result)
    {
      error_message = "Unable to link shader";
      goto exit_on_error;
    }

  // Position matrix
  matrix_id = glGetUniformLocation (program_id, "matrix");
  if (matrix_id == -1)
    {
      error_message = "could not bind matrix";
      goto exit_on_error;
    }
  color_id = glGetUniformLocation (program_id, "color");
  if (color_id == -1)
    {
      error_message = "could not bind color";
      goto exit_on_error;
    }

  // 1st vertex array
  glGenVertexArrays (1, &vertex1_array_id);
  glBindVertexArray (vertex1_array_id);
  glGenBuffers (1, &vertex1_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, vertex1_buffer);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex1_data), vertex1_data,
                GL_STATIC_DRAW);

  // 2nd vertex array
  glGenVertexArrays (1, &vertex2_array_id);
  glBindVertexArray (vertex2_array_id);
  glGenBuffers (1, &vertex2_buffer);
  glBindBuffer (GL_ARRAY_BUFFER, vertex2_buffer);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertex2_data), vertex2_data,
                GL_STATIC_DRAW);

  // init logo
  if (!image_init (logo))
    {
      error_message = "Unable to init the logo";
      goto exit_on_error;
    }

  // init text
  if (!text_init (text))
    {
      error_message = "Unable to init the text drawing";
      goto exit_on_error;
    }

  // return on success
  return 1;

exit_on_error:

  // return on error
  fprintf (stderr, "ERROR: %s\n", error_message);
  return 0;
}

// Render draw
void
draw_render ()
{
  // clear screen
  glClearColor (0., 0., 0., 1.);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 1st triangle
  glUseProgram (program_id);
  glUniformMatrix4fv (matrix_id, 1, GL_FALSE, identity);
  glUniform3fv (color_id, 1, red);
  glEnableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, vertex1_buffer);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glDrawArrays (GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray (0);

  // 2nd triangle
  glUniform3fv (color_id, 1, green);
  glEnableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, vertex2_buffer);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glDrawArrays (GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray (0);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Draw the logo
  image_draw (logo, window_width, window_height);
  // Draw the text
  text_draw (text, "Prueba", 0.6, -0.1, 0.01, 0.01, blew);
  glDisable (GL_BLEND);

  // Swap buffers
  glFlush ();
}

// Free draw
void
draw_free ()
{
  text_destroy (text);
  image_destroy (logo);
  glDeleteBuffers (1, &vertex1_buffer);
  glDeleteBuffers (1, &vertex2_buffer);
  glDeleteProgram (program_id);
}
