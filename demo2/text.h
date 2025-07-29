#ifndef TEXT__H
#define TEXT__H 1

typedef struct
{
  FT_Library ft;                ///< FreeType data.
  FT_Face face;                 ///< FreeType face to draw text.
  GLint attribute_position;     ///< Text variable position.
  GLint uniform_text;           ///< Text constant.
  GLint uniform_color;          ///< Color constant.
  GLuint vbo;                   ///< Text vertex buffer object.
  GLuint program;               ///< Text program
} Text;                         ///< Struct to define data to draw text.

int text_init (Text * text);
void text_destroy (Text * text);
void text_draw (Text * text, char *string, float x, float y, float sx, float sy,
                const GLfloat * color);

#endif
