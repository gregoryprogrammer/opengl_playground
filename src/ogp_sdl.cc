#include "ogp_sdl.h"

#include "ogp_opengl.h"
#include "ogp_utils.h"
#include "ogp_settings.h"

namespace ogp
{

void print_sdl_info(void)
{
    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    ogp_log_system("SDL compiled: %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    ogp_log_system( "SDL linked: %d.%d.%d", linked.major, linked.minor, linked.patch);
}

void fullscreen(SDL_Window *window, bool on)
{
    settings.fullscreen = on;
    if (on) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else {
        SDL_SetWindowFullscreen(window, 0);
    }
}

}  // namespace ogp
