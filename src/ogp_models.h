#ifndef OGP_MODELS_H
#define OGP_MODELS_H

#include "ogp_defines.h"
#include "ogp_physics.h"
#include "ogp_utils.h"

#include <vector>

namespace ogp
{

class render_context_t;

class pyramids_t
{
    smooth_t<f32> m_rot_y {0.0f};

    std::vector<vec3> m_positions;

public:

    void step(f32 dt);

    void draw(render_context_t *rc, f32 frame_dt);

    void add(vec3 position);
};

struct box_face_t
{
    particle_t p0;
    particle_t p1;
    particle_t p2;
    particle_t p3;

    vec3 pos0;
    vec3 pos1;
    vec3 pos2;
    vec3 pos3;
};

class box_t
{

    std::vector<box_face_t> m_faces;

public:

    body_t body;
    body_t hook;

    void create(vec3 position, physics_t *physics);

    void step(f32 dt);

    void update_faces(physics_t *physics, f32 frame_dt);

    void draw(render_context_t *rc);

};

}  // namespace ogp

#endif  // OGP_MODELS_H
