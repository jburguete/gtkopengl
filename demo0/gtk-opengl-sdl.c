#include <stdio.h>
#include <GL/glew.h>
#include <SDL.h>

#include "draw.h"

// Window minimum size
#define MINIMUM_WIDTH 320
#define MINIMUM_HEIGHT 240
int window_width = MINIMUM_WIDTH;
int window_height = MINIMUM_HEIGHT;

// Windows
SDL_Window *sdl_window = NULL;
SDL_GLContext sdl_context = NULL;

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
      window_width = MINIMUM_WIDTH;
      resize = 1;
    }
  else
    window_width = w;
  if (h < MINIMUM_HEIGHT)
    {
      window_height = MINIMUM_HEIGHT;
      resize = 1;
    }
  else
    window_height = h;
  if (resize)
    SDL_SetWindowSize (sdl_window, window_width, window_height);
  glViewport (0, 0, window_width, window_height);
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
  SDL_SetWindowSize (sdl_window, window_width, window_height);
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
  sdl_render ();
  while (1)
    {
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

// Main function
int
main ()
{
  if (!sdl_init ())
    return 4;
  if (!draw_init ())
    return 3;
  sdl_loop ();
  sdl_free ();
  return 0;
}
