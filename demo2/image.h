#ifndef IMAGE__H
#define IMAGE__H 1

/**
 * \struct Image
 * \brief A struct to define the image.
 */
typedef struct
{
  GLfloat matrix[16];           ///< Projection matrix.
  GLfloat vertices[8];          ///< Vertices.
  GLfloat square_texture[8];    ///< Square texture vertices.
  GLushort elements[6];         ///< Element indices.
  GLubyte *image;               ///< Image bytes.
  GLint uniform_texture;        ///< Texture constant.
  GLint attribute_texture;      ///< Texture variable.
  GLint attribute_texture_position;     ///< Texture variable position.
  GLint uniform_matrix;         ///< Projection matrix.
  GLuint vbo;                   ///< Vertices buffer object.
  GLuint ibo;                   ///< Indices buffer object.
  GLuint vbo_texture;           ///< Texture vertex buffer object.
  GLuint program_texture;       ///< Texture program.
  GLuint id_texture;            ///< Texture identifier.
  unsigned int width;           ///< Width.
  unsigned int height;          ///< Height.
  unsigned int size;            ///< Size in bytes.
} Image;

Image *image_new (char *name);
void image_destroy (Image * image);
int image_init (Image * image);
void image_draw (Image * image, unsigned int window_width,
                 unsigned int window_height);

#endif
