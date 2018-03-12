#ifndef OGP_PLAYER_H
#define OGP_PLAYER_H

#include "ogp_defines.h"
#include "ogp_math.h"
#include "ogp_utils.h"

namespace ogp
{

class player_t
{
    f32 m_yaw {-90.0f};
    f32 m_pitch {0.0f};
    f32 m_roll {0.0f};

    vec3 m_front {0.0f, 0.0f, 1.0f};
    vec3 m_up {0.0f, 1.0f, 0.0f};
    vec3 m_right {1.0f, 0.0f, 0.0f};

    smooth_t<vec3> m_sposition {vec3 {0.0f, 0.0f, 0.0f}};

public:

    player_t(vec3 position = vec3 {0.0f, 0.0f, 0.0f});

    void mouse_movement(i32 dx, i32 dy);

    void move(f32 forward, f32 strafe);

    vec3 get_position() const;

    quat get_orientation() const;
};


}  // namespace ogp

#endif  // OGP_PLAYER_H
