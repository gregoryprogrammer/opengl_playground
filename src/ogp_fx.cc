#include "ogp_fx.h"

#include "ogp_input.h"
#include "ogp_utils.h"

namespace ogp
{

effects_t effects;

void effects_t::step(f32 dt)
{
    NOT_USED(dt);
    // Post-process effects FX
    if (key_just_pressed("F5")) {
        effects.wave.enabled ^= true;
        if (effects.wave.enabled) {
            ogp_log_debug("FX: wave: on");
            effects.wave.time = 0.0f;
        }
        else {
            ogp_log_debug("FX: wave: off");
        }
    }

    if (effects.wave.enabled) {
        effects.wave.time += 0.1f;
        effects.wave.amplifier += 0.05f;
    }
    else {
        effects.wave.time += 0.1f;
        effects.wave.amplifier -= 0.05f;
    }
    effects.wave.amplifier = clamp(effects.wave.amplifier, 0.0f, 1.0f);

    // greyscale
    if (key_just_pressed("F6")) {
        effects.greyscale.enabled ^= true;
        if (effects.greyscale.enabled) {
            ogp_log_debug("FX: greyscale: on");
        }
        else {
            ogp_log_debug("FX: greyscale: off");
        }
    }

    if (effects.greyscale.enabled) {
        effects.greyscale.amplifier += 0.05f;
    }
    else {
        effects.greyscale.amplifier -= 0.05f;
    }
    effects.greyscale.amplifier = clamp(effects.greyscale.amplifier, 0.0f, 1.0f);

    if (key_just_pressed("F7")) {
        effects.negative.enabled ^= true;
        if (effects.negative.enabled) {
            ogp_log_debug("FX: negative: on");
        }
        else {
            ogp_log_debug("FX: negative: off");
        }
    }
    if (effects.negative.enabled) {
        effects.negative.amplifier += 0.05f;
    }
    else {
        effects.negative.amplifier -= 0.05f;
    }
    effects.negative.amplifier = clamp(effects.negative.amplifier, 0.0f, 1.0f);
}

}  // namespace ogp
