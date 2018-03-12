#ifndef OGP_CLOTH_H
#define OGP_CLOTH_H

#include "ogp_defines.h"
#include "ogp_physics.h"

#include <vector>

namespace ogp
{

class render_context_t;

struct cloth_face_t
{
    particle_t p0;
    particle_t p1;
    particle_t p2;

    vec3 pos0;
    vec3 pos1;
    vec3 pos2;
};

class cloth_t
{
    std::vector<particle_t> m_hook_particles;

    std::vector<cloth_face_t> m_faces;

public:

    body_t body;

    body_t hook;

    cloth_t() = default;

    void create(i32 M, i32 N, f32 width, f32 height, f32 z, physics_t *physics);

    void destroy(physics_t *physics);

    void step(f32 dt);

    void update_faces(physics_t *physics, f32 frame_dt);

    void draw(render_context_t *rc);

    vec3 get_seventh_pos(physics_t *physics, f32 frame_dt);
};

}  // namespace ogp

#endif  // OGP_CLOTH_H
