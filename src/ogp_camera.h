#ifndef OGP_CAMERA_H
#define OGP_CAMERA_H

#include "ogp_defines.h"
#include "ogp_math.h"
#include "ogp_utils.h"

namespace ogp
{

class camera_t
{
    mat4 m_view {1.0f};

    mat4 m_projection {1.0f};

    quat m_orientation;
    quat m_orientation_next;

    smooth_t<vec3> m_sposition { vec3 {0.0f, 0.0f, 0.0f} };

public:

    f32 fov {45.0f};

    camera_t (vec3 position);

    mat4 view() const;

    mat4 projection() const;

    void set_projection_2d();

    void set_projection_3d();

    vec3 get_position() const;

    void step(quat orientation);

    void draw_step(f32 frame_dt);

    void move_to(vec3 point);
};


}  // namespace ogp

#endif  // OGP_CAMERA_H
