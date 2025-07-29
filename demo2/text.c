#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <png.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include <epoxy/gl.h>

#include "image.h"
#include "text.h"

/**
 * Function to init the variables used to draw text.
 *
 * \return 1 on success, 0 on error.
 */
int
text_init (Text * text)         ///< Text struct data.
{
  const char *fs_source =
    "uniform sampler2D text;"
    "uniform vec4 color;"
    "out vec2 textcoord;"
    "void main ()"
    "{gl_FragColor=vec4(1.,1.,1.,texture2D(text,textcoord).a)*color;}";
  const char *vs_source =
    "in vec4 position;"
    "out vec2 textcoord;"
    "void main (){gl_Position=vec4(position.xy,0.,1.);textcoord=position.zw;}";
  const char *vertex_name = "position";
  const char *color_name = "color";
  const char *text_name = "text";
  // GLSL version
  const char *version;
  const char *fs_sources[2];
  const char *vs_sources[2];
  const char *error_message;
  GLint k;
  GLuint vs, fs;

  // Select shaders
  if (strstr ((const char *) glGetString (GL_VERSION), "OpenGL ES"))
    version = "#version 100\n#define in attribute\n#define out varying\n"
	      "precision mediump float;";
  else if (epoxy_gl_version () >= 33)
    version = "#version 330 core\n";
  else
    version = "#version 120\n#define in attribute\n#define out varying\n";
  fs_sources[0] = version;
  fs_sources[1] = fs_source;
  vs_sources[0] = version;
  vs_sources[1] = vs_source;

  fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 2, fs_sources, NULL);
  glCompileShader (fs);
  glGetShaderiv (fs, GL_COMPILE_STATUS, &k);
  if (!k)
    {
      error_message = "unable to compile the text fragment shader";
      goto exit_on_error;
    }

  vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 2, vs_sources, NULL);
  glCompileShader (vs);
  glGetShaderiv (vs, GL_COMPILE_STATUS, &k);
  if (!k)
    {
      error_message = "unable to compile the text vertex shader";
      goto exit_on_error;
    }

  text->program = glCreateProgram ();
  glAttachShader (text->program, fs);
  glAttachShader (text->program, vs);
  glLinkProgram (text->program);
  glDetachShader (text->program, vs);
  glDetachShader (text->program, fs);
  glDeleteShader (vs);
  glDeleteShader (fs);
  glGetProgramiv (text->program, GL_LINK_STATUS, &k);
  if (!k)
    {
      error_message = "unable to link the program text";
      goto exit_on_error;
    }

  text->attribute_position = glGetAttribLocation (text->program, vertex_name);
  if (text->attribute_position == -1)
    {
      error_message = "could not bind position attribute";
      goto exit_on_error;
    }
  text->uniform_text = glGetUniformLocation (text->program, text_name);
  if (text->uniform_text == -1)
    {
      error_message = "could not bind text uniform";
      goto exit_on_error;
    }
  text->uniform_color = glGetUniformLocation (text->program, color_name);
  if (text->uniform_color == -1)
    {
      error_message = "could not bind color uniform";
      goto exit_on_error;
    }

  // Initing FreeType
  if (FT_Init_FreeType (&text->ft))
    {
      error_message = "could not init freetype library";
      goto exit_on_error;
    }
  if (FT_New_Face (text->ft, FONT, 0, &text->face))
    {
      error_message = "could not open font";
      goto exit_on_error;
    }
  FT_Select_Charmap (text->face, ft_encoding_unicode);
  FT_Set_Pixel_Sizes (text->face, 0, 12);

  glGenBuffers (1, &text->vbo);
  return 1;

exit_on_error:
  printf ("ERROR! Text: %s\n", error_message);
  return 0;
}

/**
 * Function to free the memory used to draw text.
 */
void
text_destroy (Text * text)      ///< Text struct data.
{
#if DEBUG
  printf ("text_destroy: start\n");
  fflush (stdout);
#endif

  glDeleteBuffers (1, &text->vbo);
  glDeleteProgram (text->program);
  FT_Done_Face (text->face);
  FT_Done_Library (text->ft);

#if DEBUG
  printf ("text_destroy: end\n");
  fflush (stdout);
#endif
}

/**
 * Function to draw a string.
 */
void
text_draw (Text * text,         ///< Text struct data.
           char *string,        ///< String.
           float x,             ///< x initial coordinate.
           float y,             ///< y initial coordinate.
           float sx,            ///< x scale factor.
           float sy,            ///< y scale factor.
           const GLfloat * color)       ///< array of RBGA colors.
{
  float box[16];
  FT_Face face;
  float x2, y2, w, h;
  FT_UInt glyph;
  GLuint id;
  gunichar c;

#if DEBUG
  printf ("text_draw: start\n");
  fflush (stdout);
#endif

  box[2] = 0.f;
  box[3] = 0.f;
  box[6] = 1.f;
  box[7] = 0.f;
  box[10] = 0.f;
  box[11] = 1.f;
  box[14] = 1.f;
  box[15] = 1.f;

  glUseProgram (text->program);
  glGenTextures (1, &id);
  glBindTexture (GL_TEXTURE_2D, id);
  glUniform1i (text->uniform_text, 0);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  glUniform4fv (text->uniform_color, 1, color);
  glEnableVertexAttribArray (text->attribute_position);
  glBindBuffer (GL_ARRAY_BUFFER, text->vbo);
  glVertexAttribPointer (text->attribute_position, 4, GL_FLOAT, GL_FALSE, 0, 0);
  face = text->face;
  for (; *string; ++string)
    {
      c = g_utf8_get_char (string);
      glyph = FT_Get_Char_Index (face, c);
      if (FT_Load_Glyph (face, glyph, FT_LOAD_RENDER))
        continue;
      glTexImage2D (GL_TEXTURE_2D,
                    0,
                    GL_ALPHA,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0, GL_ALPHA, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
      x2 = x + face->glyph->bitmap_left * sx;
      y2 = -y - face->glyph->bitmap_top * sy;
      w = face->glyph->bitmap.width * sx;
      h = face->glyph->bitmap.rows * sy;
      box[0] = x2;
      box[1] = -y2;
      box[4] = x2 + w;
      box[5] = -y2;
      box[8] = x2;
      box[9] = -y2 - h;
      box[12] = x2 + w;
      box[13] = -y2 - h;
      glBufferData (GL_ARRAY_BUFFER, sizeof (box), box, GL_DYNAMIC_DRAW);
      glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
      x += (face->glyph->advance.x >> 6) * sx;
      y += (face->glyph->advance.y >> 6) * sy;
    }
  glDisableVertexAttribArray (text->attribute_position);
  glDeleteTextures (1, &id);

#if DEBUG
  printf ("text_draw: end\n");
  fflush (stdout);
#endif
}
