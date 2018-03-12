#include "ogp_models.h"

#include "ogp_input.h"
#include "ogp_render.h"

namespace ogp
{

void pyramids_t::step(f32 dt)
{
    if (key_pressed("K")) {
        f32 rot_y = m_rot_y.get_next();
        rot_y = rot_y + 100.0f * dt;
        m_rot_y.step(rot_y);
    }
}

void pyramids_t::draw(render_context_t *rc, f32 frame_dt)
{
    f32 edge = 0.25f;
    f32 rot_y = m_rot_y.get(frame_dt);

    vec3 prev_origin = rc->set_origin({0.0f, 0.0f, 0.0f});
    vec3 prev_rot = rc->set_rotation({0.0f, rot_y, 0.0f});
    color_t prev_color = rc->set_fill_color(76, 70, 50);

    for (vec3 const &p : m_positions) {
        vec3 p0 = p + vec3 {-edge, 0.0f, edge};
        vec3 p1 = p + vec3 {edge, 0.0f, edge};
        vec3 p2 = p + vec3 {-edge, 0.0f, -edge};
        vec3 p3 = p + vec3 {edge, 0.0f, -edge};
        vec3 p4 = p + vec3 {0.0f, edge, 0.0f};

        rc->set_origin(p);
        rc->draw_color_face(p0, p1, p4);
        rc->draw_color_face(p1, p3, p4);
        rc->draw_color_face(p3, p2, p4);
        rc->draw_color_face(p2, p0, p4);
    }

    rc->set_origin(prev_origin);
    rc->set_rotation(prev_rot);
    rc->set_fill_color(prev_color);
}

void pyramids_t::add(vec3 position)
{
    m_positions.push_back(position);
}



void box_t::create(vec3 position, physics_t *physics)
{
    // body = physics->create_body(body_type_e::body_static);
    body = physics->create_body(body_type_e::body_dynamic);

    f32 edge = 0.15f;
    vec3 p0 = position + vec3 {-edge, -edge, edge};
    vec3 p1 = position + vec3 {edge, -edge, edge};
    vec3 p2 = position + vec3 {-edge, -edge, -edge};
    vec3 p3 = position + vec3 {edge, -edge, -edge};

    vec3 p4 = position + vec3 {-edge, edge, edge};
    vec3 p5 = position + vec3 {edge, edge, edge};
    vec3 p6 = position + vec3 {-edge, edge, -edge};
    vec3 p7 = position + vec3 {edge, edge, -edge};

    particle_t par0 = physics->create_particle(body, p0);
    particle_t par1 = physics->create_particle(body, p1);
    particle_t par2 = physics->create_particle(body, p2);
    particle_t par3 = physics->create_particle(body, p3);

    particle_t par4 = physics->create_particle(body, p4);
    particle_t par5 = physics->create_particle(body, p5);
    particle_t par6 = physics->create_particle(body, p6);
    particle_t par7 = physics->create_particle(body, p7);

    hook = physics->create_body(body_type_e::body_static);
    particle_t hook_par = physics->create_particle(hook, p4);

    // physics->create_pin(hook, hook_par, body, par4);

    physics->create_constraint(hook, hook_par, body, par4);

    // bottom
    physics->create_constraint(body, par0, body, par1);
    physics->create_constraint(body, par0, body, par2);
    physics->create_constraint(body, par1, body, par3);
    physics->create_constraint(body, par2, body, par3);
    physics->create_constraint(body, par0, body, par3);
    physics->create_constraint(body, par1, body, par2);

    // top
    physics->create_constraint(body, par4, body, par5);
    physics->create_constraint(body, par4, body, par6);
    physics->create_constraint(body, par5, body, par7);
    physics->create_constraint(body, par6, body, par7);
    physics->create_constraint(body, par4, body, par7);
    physics->create_constraint(body, par5, body, par6);

    // vertical
    physics->create_constraint(body, par0, body, par4);
    physics->create_constraint(body, par1, body, par5);
    physics->create_constraint(body, par2, body, par6);
    physics->create_constraint(body, par3, body, par7);

    // diagonal
    physics->create_constraint(body, par0, body, par5);
    physics->create_constraint(body, par1, body, par4);
    physics->create_constraint(body, par2, body, par4);
    physics->create_constraint(body, par0, body, par6);

    physics->create_constraint(body, par1, body, par7);
    physics->create_constraint(body, par3, body, par5);
    physics->create_constraint(body, par2, body, par7);
    physics->create_constraint(body, par3, body, par6);

    // inner
    physics->create_constraint(body, par0, body, par7);
    physics->create_constraint(body, par1, body, par6);
    physics->create_constraint(body, par4, body, par3);
    physics->create_constraint(body, par5, body, par2);

    {
        box_face_t f0 {};  // front
        f0.p0 = par0;
        f0.p1 = par1;
        f0.p2 = par4;
        f0.p3 = par5;
        m_faces.push_back(f0);
    }

    {
        box_face_t f0 {};  // left
        f0.p0 = par2;
        f0.p1 = par0;
        f0.p2 = par6;
        f0.p3 = par4;
        m_faces.push_back(f0);
    }

    {
        box_face_t f0 {};  // right
        f0.p0 = par1;
        f0.p1 = par3;
        f0.p2 = par5;
        f0.p3 = par7;
        m_faces.push_back(f0);
    }

    {
        box_face_t f0 {};  // back
        f0.p0 = par3;
        f0.p1 = par2;
        f0.p2 = par7;
        f0.p3 = par6;
        m_faces.push_back(f0);
    }

    {
        box_face_t f0 {};  // bottom
        f0.p0 = par2;
        f0.p1 = par3;
        f0.p2 = par0;
        f0.p3 = par1;
        m_faces.push_back(f0);
    }

    {
        box_face_t f0 {};  // top
        f0.p0 = par4;
        f0.p1 = par5;
        f0.p2 = par6;
        f0.p3 = par7;
        m_faces.push_back(f0);
    }
}

void box_t::step(f32 dt)
{
    NOT_USED(dt);
}

void box_t::update_faces(physics_t *physics, f32 frame_dt)
{
    for (box_face_t &cf : m_faces) {
        cf.pos0 = physics->get_particle_pos(cf.p0, frame_dt);
        cf.pos1 = physics->get_particle_pos(cf.p1, frame_dt);
        cf.pos2 = physics->get_particle_pos(cf.p2, frame_dt);
        cf.pos3 = physics->get_particle_pos(cf.p3, frame_dt);
    }

    if (key_just_pressed("G")) {
        physics->set_body_type(body, body_type_e::body_dynamic);
    }
}

void box_t::draw(render_context_t *rc)
{
    color_t color = rc->set_fill_color(166, 101, 80);
    for (box_face_t const &cf : m_faces) {
        rc->draw_color_quad(cf.pos0, cf.pos1, cf.pos2, cf.pos3);
    }
    rc->set_fill_color(color);
}



}  // namespace ogp
