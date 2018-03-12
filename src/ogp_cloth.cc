#include "ogp_cloth.h"

#include "ogp_input.h"
#include "ogp_render.h"

namespace ogp
{

void cloth_t::create(i32 M, i32 N, f32 width, f32 height, f32 z, physics_t *physics)
{
    if (M < 1 || N < 1) {
        PRINT_INT(M);
        PRINT_INT(N);
        std::terminate();
    }

    destroy(physics);

    std::vector<particle_t> m_particles;

    f32 tile_x = width / (f32)(M - 1);
    f32 tile_y = height / (f32)(N - 1);
    body = physics->create_body(body_type_e::body_dynamic);

    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < M; ++i) {
            vec3 position {i * tile_x, j * tile_y, z + 0.01f * i};
            particle_t particle = physics->create_particle(body, position);
            m_particles.push_back(particle);
        }
    }

    // poziome
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < M - 1; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = i + 1 + (j * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    // pionowe
    for (int j = 0; j < N - 1; ++j) {
        for (int i = 0; i < M; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = i + ((j + 1) * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    // poziome co 2
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < M - 2; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = i + 2 + (j * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    // pionowe co 2
    for (int j = 0; j < N - 2; ++j) {
        for (int i = 0; i < M; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = i + ((j + 2) * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    // skosne slash
    for (int j = 0; j < N - 1; ++j) {
        for (int i = 0; i < M - 1; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = (i + 1) + ((j + 1) * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    // skosne backslash
    for (int j = 0; j < N - 1; ++j) {
        for (int i = 1; i < M; ++i) {
            i32 index_p_A = i + (j * M);
            i32 index_p_B = (i - 1) + ((j + 1) * M);
            particle_t &p_A = m_particles.at(index_p_A);
            particle_t &p_B = m_particles.at(index_p_B);
            physics->create_constraint(body, p_A, body, p_B);
        }
    }

    hook = physics->create_body(body_type_e::body_static);

    for (int i = 0; i < M; ++i) {
        f32 x = (f32)i / M;
        particle_t p0 = physics->create_particle(hook, {x, 2.0f, z});
        m_hook_particles.push_back(p0);

        physics->create_pin(hook, p0, body, m_particles.at((N - 1) * M + i));
    }

    // faces
    for (i32 j = 0; j < N - 1; ++j) {
        for (i32 i = 0; i < M - 1; ++i) {

            i32 p0 = i + (j * M);
            i32 p1 = i + (j * M) + 1;
            i32 p2 = i + (j * M) + M;
            i32 p3 = i + (j * M) + M + 1;

            cloth_face_t cf0 {};
            cf0.p0 = m_particles.at(p0);
            cf0.p1 = m_particles.at(p1);
            cf0.p2 = m_particles.at(p2);
            m_faces.push_back(cf0);

            cloth_face_t cf1 {};
            cf1.p0 = m_particles.at(p2);
            cf1.p1 = m_particles.at(p1);
            cf1.p2 = m_particles.at(p3);
            m_faces.push_back(cf1);
        }
    }
}

void cloth_t::destroy(physics_t *physics)
{
    if (physics->body_exists(body)) physics->destroy_body(body);
    if (physics->body_exists(hook)) physics->destroy_body(hook);

    m_hook_particles.clear();
    m_faces.clear();
}

void cloth_t::step(f32 dt)
{
    NOT_USED(dt);
}

void cloth_t::update_faces(physics_t *physics, f32 frame_dt)
{
    if (physics->body_exists(body) == false) {
        return;
    }

    for (cloth_face_t &cf : m_faces) {
        cf.pos0 = physics->get_particle_pos(cf.p0, frame_dt);
        cf.pos1 = physics->get_particle_pos(cf.p1, frame_dt);
        cf.pos2 = physics->get_particle_pos(cf.p2, frame_dt);
    }

    f32 a = 0.0f;

    if (key_pressed("Q")) {
        a = 4.0f * frame_dt;
    }
    else if (key_pressed("E")) {
        a = -4.0f * frame_dt;
    }

    vec3 rot_axis = {0.0f, 1.0f, 0.0f};
    rot_axis = glm::normalize(rot_axis);
    quat orient = glm::angleAxis(glm::radians(a), rot_axis);

    orient = glm::normalize(orient);

    // FIXME tmp

    for (particle_t const &particle : m_hook_particles) {
        vec3 pos = physics->get_particle_pos(particle, frame_dt);

        pos = orient * pos;

        physics->set_particle_pos(particle, pos);
    }
}

void cloth_t::draw(render_context_t *rc)
{
    color_t color = rc->set_fill_color(80, 101, 166);
    for (cloth_face_t const &cf : m_faces) {
        rc->draw_color_face(cf.pos0, cf.pos1, cf.pos2);
        rc->draw_color_face(cf.pos0, cf.pos2, cf.pos1);
    }
    rc->set_fill_color(color);
}

vec3 cloth_t::get_seventh_pos(physics_t *physics, f32 frame_dt)
{
    return physics->get_particle_pos(m_faces[13].p1, frame_dt);
}

}  // namespace ogp
