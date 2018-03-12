#ifndef OGP_SDL_H
#define OGP_SDL_H

#include <SDL2/SDL.h>

namespace ogp
{

void print_sdl_info();

void fullscreen(SDL_Window *window, bool on);

}  // namespace ogp

#endif  // OGP_SDL_H
