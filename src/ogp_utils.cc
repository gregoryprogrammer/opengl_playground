#include "ogp_utils.h"

#include "ogp_defines.h"
#include "ogp_settings.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdarg.h>
#include <cstring>

namespace ogp
{

void ogplog(LogLevel level, const char *filename, const char *function, i32 line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    u32 ticks = SDL_GetTicks();
    char timestamp[64];
    f32 seconds = f32 (ticks) / 1000.0f;
    snprintf(timestamp, 64, "%f", seconds);

    char location[64];
    snprintf(location, 64, "%s:%d", filename, line);

    switch (level) {
    case LogLevel::SYSTEM:
        fprintf(stderr, ANSI_COLOR_RESET);
        fprintf(stderr, "SYSTEM  " LOGFORMAT, timestamp, location, function);
        break;

    case LogLevel::INFO:
        fprintf(stderr, ANSI_COLOR_GREEN);
        fprintf(stderr, "INFO    " LOGFORMAT, timestamp, location, function);
        break;

    case LogLevel::DEBUG:
        fprintf(stderr, ANSI_COLOR_BLUE);
        fprintf(stderr, "DEBUG   " LOGFORMAT, timestamp, location, function);
        break;

    case LogLevel::WARNING:
        fprintf(stderr, ANSI_COLOR_YELLOW);
        fprintf(stderr, "WARNING " LOGFORMAT, timestamp, location, function);
        break;

    case LogLevel::ERROR:
        fprintf(stderr, ANSI_COLOR_RED);
        fprintf(stderr, "ERROR   " LOGFORMAT, timestamp, location, function);
        break;
    case LogLevel::ME:
        fprintf(stderr, ANSI_COLOR_MAGENTA);
        fprintf(stderr, "USER    " LOGFORMAT, timestamp, location, function);
        break;
    case LogLevel::TODO:
        fprintf(stderr, ANSI_COLOR_CYAN);
        fprintf(stderr, "TODO    " LOGFORMAT, timestamp, location, function);
        break;
    }

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n" ANSI_COLOR_RESET);

    va_end(args);
}

void Mr_Terminator()
{
    ogp_log_error("Hasta la vista, baby");
    std::abort();
}

void terminate(char const *msg)
{
    ogp_log_error("%s", msg);
    std::terminate();
}

void print_working_dir()
{
    ogp_log_debug("Working dir: %s", SDL_GetBasePath());
}

void print_size_info()
{
    ogp_log_info("Size:");
    ogp_log_info("    window (pixels) = %d x %d", settings.size.window.x, settings.size.window.y);
    ogp_log_info("    logic  (pixels) = %d x %d", settings.size.logic.x, settings.size.logic.y);
    ogp_log_info("    game   (meters) = %.2f x %.2f", settings.size.game.x, settings.size.game.y);
    ogp_log_info("    frame  (meters) = %f x %f", settings.size.frame.x, settings.size.frame.y);
    // ogp_log_info("    pixs per meters = %f", settings.mpp);
}

char *file_read(char const *filename)
{
    SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
    if (rw == NULL) return NULL;

    i64 res_size = SDL_RWsize(rw);
    char *res       = (char *)malloc(res_size + 1);

    i64 nb_read_total = 0, nb_read = 1;
    char *buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
        nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);

    if (nb_read_total != res_size) {
        free(res);
        return NULL;
    }

    res[nb_read_total] = '\0';
    return res;
}

bool cmp_str::operator()(char const *a, char const *b) const
{
    return std::strcmp(a, b) < 0;
}

}
