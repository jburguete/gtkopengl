#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <png.h>
#include <epoxy/gl.h>

#include "image.h"

const char *fs_texture_source_v3 =
  "#version 330 core\n"
  "in vec2 t_position;"
  "out vec4 fcolor;"
  "uniform sampler2D texture_image;"
  "void main()" "{fcolor=texture(texture_image,t_position);}";
const char *vs_texture_source_v3 =
  "#version 330 core\n"
  "in vec2 position;"
  "in vec2 texture_position;"
  "out vec2 t_position;"
  "uniform mat4 matrix;"
  "void main()"
  "{gl_Position=matrix*vec4(position,0.,1.);"
  "t_position=texture_position;}";
const char *fs_texture_source_v2 =
  "#version 120\n"
  "varying vec2 t_position;"
  "uniform sampler2D texture_image;"
  "void main()" "{gl_FragColor=texture2D(texture_image,t_position);}";
const char *vs_texture_source_v2 =
  "#version 120\n"
  "attribute vec2 position;"
  "attribute vec2 texture_position;"
  "varying vec2 t_position;"
  "uniform mat4 matrix;"
  "void main()"
  "{gl_Position=matrix*vec4(position,0.,1.);"
  "t_position=texture_position;}";
const char *fs_texture_source_es =
  "#version 100\n"
  "precision mediump float;"
  "varying vec2 t_position;"
  "uniform sampler2D texture_image;"
  "void main()" "{gl_FragColor=texture2D(texture_image,t_position);}";
const char *vs_texture_source_es =
  "#version 100\n"
  "attribute vec2 position;"
  "attribute vec2 texture_position;"
  "varying vec2 t_position;"
  "uniform mat4 matrix;"
  "void main()"
  "{gl_Position=matrix*vec4(position,0.,1.);"
  "t_position=texture_position;}";

/**
 * Function to read the image on a PNG file.
 *
 * \return pointer to the Image struct data on success, NULL on error.
 */
Image *
image_new (char *name)          ///< Image PNG file name.
{
  const GLfloat matrix[16] = {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, -1.f, 1.f
  };
  const GLfloat vertices[8] = {
    -1.f, 1.f,
    -1.f, -1.f,
    1.f, -1.f,
    1.f, 1.f
  };
  const GLfloat square_texture[8] = {
    0.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0
  };
  const GLushort elements[6] = {
    0, 1, 2,
    2, 3, 0
  };
  Image *image;
  png_struct *png;
  png_info *info;
  png_byte **row_pointers;
  FILE *file;
  unsigned int i, row_bytes;

#if DEBUG
  printf ("image_new: start\n");
  fflush (stdout);
#endif

  // initing image
  image = NULL;

  // starting png structs
  png = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  info = png_create_info_struct (png);

  // opening file
  file = fopen (name, "rb");
  if (!file)
    goto error1;

  // reading file and transforming to 8 bits RGBA format
  if (setjmp (png_jmpbuf (png)))
    goto error2;
  png_init_io (png, file);
  if (setjmp (png_jmpbuf (png)))
    goto error2;
  png_read_png (png,
                info,
                PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING |
                PNG_TRANSFORM_EXPAND, NULL);

  // allocating image
  image = (Image *) g_slice_alloc (sizeof (Image));

  // copying pixels in the OpenGL order
  image->width = png_get_image_width (png, info);
  image->height = png_get_image_height (png, info);
  row_bytes = png_get_rowbytes (png, info);
  image->size = row_bytes * image->height;
  image->image = (GLubyte *) g_slice_alloc (image->size);
  if (!image->image)
    {
      g_slice_free1 (sizeof (Image), image);
      image = NULL;
      goto error2;
    }
  row_pointers = png_get_rows (png, info);
  for (i = 0; i < image->height; i++)
    memcpy (image->image + (row_bytes * (image->height - 1 - i)),
            row_pointers[i], row_bytes);
  memcpy (image->matrix, matrix, 16 * sizeof (GLfloat));
  memcpy (image->vertices, vertices, 8 * sizeof (GLfloat));
  memcpy (image->square_texture, square_texture, 8 * sizeof (GLfloat));
  memcpy (image->elements, elements, 6 * sizeof (GLushort));

error2:
  // closing file
  fclose (file);

error1:
  // freeing memory
  png_destroy_read_struct (&png, &info, NULL);

#if DEBUG
  printf ("image_new: end\n");
  fflush (stdout);
#endif
  return image;
}

/**
 * Function to init the variables used to draw the image.
 *
 * \return 1 on success, 0 on error.
 */
int
image_init (Image * image)      ///< Image struct.
{
  const char *vertex_name = "position";
  const char *texture_name = "texture_image";
  const char *texture_position_name = "texture_position";
  const char *matrix_name = "matrix";
  const char *vs_texture_source[1];
  const char *fs_texture_source[1];
  // GLSL version
  const char *error_message;
  GLint k;
  GLuint vs, fs;

#if DEBUG
  printf ("image_init: start\n");
  fflush (stdout);
#endif

  // Select shaders
  if (strstr ((const char *) glGetString (GL_VERSION), "OpenGL ES"))
    {
      fs_texture_source[0] = fs_texture_source_es;
      vs_texture_source[0] = vs_texture_source_es;
    }
  else if (epoxy_gl_version () >= 33)
    {
      fs_texture_source[0] = fs_texture_source_v3;
      vs_texture_source[0] = vs_texture_source_v3;
    }
  else
    {
      fs_texture_source[0] = fs_texture_source_v2;
      vs_texture_source[0] = vs_texture_source_v2;
    }

  fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 1, fs_texture_source, NULL);
  glCompileShader (fs);
  glGetShaderiv (fs, GL_COMPILE_STATUS, &k);
  if (!k)
    {
      error_message = "unable to compile the texture fragment shader";
      goto exit_on_error;
    }

  vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 1, vs_texture_source, NULL);
  glCompileShader (vs);
  glGetShaderiv (vs, GL_COMPILE_STATUS, &k);
  if (!k)
    {
      error_message = "unable to compile the 2D texture vertex shader";
      goto exit_on_error;
    }

  image->program_texture = glCreateProgram ();
  glAttachShader (image->program_texture, fs);
  glAttachShader (image->program_texture, vs);
  glLinkProgram (image->program_texture);
  glDetachShader (image->program_texture, vs);
  glDetachShader (image->program_texture, fs);
  glDeleteShader (vs);
  glDeleteShader (fs);
  glGetProgramiv (image->program_texture, GL_LINK_STATUS, &k);
  if (!k)
    {
      error_message = "unable to link the program 2D texture";
      goto exit_on_error;
    }

  glActiveTexture (GL_TEXTURE0);
  glGenTextures (1, &image->id_texture);
  glBindTexture (GL_TEXTURE_2D, image->id_texture);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D (GL_TEXTURE_2D,  // target
                0,              // level, 0 = base, no minimap,
                GL_RGBA,        // internalformat
                image->width,   // width
                image->height,  // height
                0,              // border, always 0 in OpenGL ES
                GL_RGBA,        // format
                GL_UNSIGNED_BYTE,       // type
                image->image);  // image data

  image->attribute_texture
    = glGetAttribLocation (image->program_texture, vertex_name);
  if (image->attribute_texture == -1)
    {
      error_message = "could not bind vertex attribute";
      goto exit_on_error;
    }

  image->attribute_texture_position
    = glGetAttribLocation (image->program_texture, texture_position_name);
  if (image->attribute_texture_position == -1)
    {
      error_message = "could not bind position attribute";
      goto exit_on_error;
    }

  image->uniform_texture
    = glGetUniformLocation (image->program_texture, texture_name);
  if (image->uniform_texture == -1)
    {
      error_message = "could not bind uniform texture";
      goto exit_on_error;
    }

  image->uniform_matrix
    = glGetUniformLocation (image->program_texture, matrix_name);
  if (image->uniform_matrix == -1)
    {
      error_message = "could not bind uniform matrix";
      goto exit_on_error;
    }

  glGenBuffers (1, &image->vbo);
  glBindBuffer (GL_ARRAY_BUFFER, image->vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (image->vertices), image->vertices,
                GL_STATIC_DRAW);

  glGenBuffers (1, &image->ibo);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, image->ibo);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (image->elements),
                image->elements, GL_STATIC_DRAW);

  glGenBuffers (1, &image->vbo_texture);
  glBindBuffer (GL_ARRAY_BUFFER, image->vbo_texture);
  glBufferData (GL_ARRAY_BUFFER, sizeof (image->square_texture),
                image->square_texture, GL_STATIC_DRAW);

#if DEBUG
  printf ("image_init: end\n");
  fflush (stdout);
#endif
  return 1;

exit_on_error:
  printf ("ERROR! Image: %s\n", error_message);
#if DEBUG
  printf ("image_init: end\n");
  fflush (stdout);
#endif
  return 0;
}

/**
 * Function to free the memory used by the image.
 */
void
image_destroy (Image * image)   ///< Image struct.
{
#if DEBUG
  printf ("image_destroy: start\n");
  fflush (stdout);
#endif

  glDeleteBuffers (1, &image->ibo);
  glDeleteBuffers (1, &image->vbo);
  glDeleteBuffers (1, &image->vbo_texture);
  glDeleteTextures (1, &image->id_texture);
  glDeleteProgram (image->program_texture);
  g_slice_free1 (image->size, image->image);

#if DEBUG
  printf ("image_destroy: end\n");
  fflush (stdout);
#endif
}

/**
 * Function to draw the image.
 */
void
image_draw (Image * image,      ///< Image struct.
            unsigned int window_width,  ///< Window width.
            unsigned int window_height) ///< Window height.
{
  float cp, sp;
  cp = ((float) image->width) / window_width;
  sp = ((float) image->height) / window_height;
  image->matrix[0] = cp;
  image->matrix[5] = sp;
  image->matrix[12] = cp - 1.f;
  image->matrix[13] = sp - 1.f;
  glUseProgram (image->program_texture);
  glUniformMatrix4fv (image->uniform_matrix, 1, GL_FALSE, image->matrix);
  glUniform1i (image->uniform_texture, 0);
  glBindTexture (GL_TEXTURE_2D, image->id_texture);
  glBindBuffer (GL_ARRAY_BUFFER, image->vbo_texture);
  glEnableVertexAttribArray (image->attribute_texture_position);
  glVertexAttribPointer (image->attribute_texture_position,
                         2, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer (GL_ARRAY_BUFFER, image->vbo);
  glEnableVertexAttribArray (image->attribute_texture);
  glVertexAttribPointer (image->attribute_texture, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, image->ibo);
  glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  glDisableVertexAttribArray (image->attribute_texture);
  glDisableVertexAttribArray (image->attribute_texture_position);
}
