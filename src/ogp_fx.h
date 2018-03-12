#ifndef OGP_FX_H
#define OGP_FX_H

#include "ogp_defines.h"

namespace ogp
{

struct effects_t
{
    struct {
        bool enabled {false};
        f32 time {0.0f};
        f32 amplifier {0.0f};
    } wave;

    struct {
        bool enabled {false};
        f32 amplifier {0.0f};
    } greyscale;

    struct {
        bool enabled {false};
        f32 amplifier {0.0f};
    } negative;

    void step(f32 dt);
};

extern effects_t effects;

}  // namespace ogp

#endif  // OGP_FX_H
