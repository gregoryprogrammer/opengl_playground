#ifndef OGP_SETTINGS_H
#define OGP_SETTINGS_H

#include "ogp_defines.h"

namespace ogp
{

constexpr f32 LAYER_STEP {0.01f};

struct settings_t
{
    bool fullscreen {false};
    bool focus {true};
    bool use_fbo {true};
    f32 mpp {1.0f / 100.0f};  // meters per pixel

    struct
    {
        f32 const s {1.0f / 30.0f};
        f32 const ms {s * 1000.0f};
        f32 const us {ms * 1000.0f};
    } delta;

    struct {
        ivec2 window {1920, 1080};    // pixels, current window, ex: 800 x 600
        ivec2 logic {1920, 1080};     // pixels, ex: 320 x 200
        vec2 game {19.2f, 10.8f};   // meters, ex 16.0 x 9.0
        vec2 frame {19.2f, 10.8f};  // meters in window, ex: 30.0 x 9.0
    } size;

    f32 game_ratio() const
    {
        return size.game.x / size.game.y;
    };

    f32 window_ratio() const
    {
        f32 width = static_cast<f32>(size.window.x);
        f32 height = static_cast<f32>(size.window.y);
        return width / height;
    }

};

extern settings_t settings;

}  // namespace ogp

#endif  // OGP_SETTINGS_H
