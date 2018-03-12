#ifndef OGP_PHYSICS_H
#define OGP_PHYSICS_H

#include "ogp_array.h"
#include "ogp_defines.h"

#include <vector>

namespace ogp
{

class render_context_t;

struct body_t : public index_holder_t<body_t> { };
struct particle_t : public index_holder_t<particle_t> { };
struct constraint_t : public index_holder_t<constraint_t> { };
struct pin_t : public index_holder_t<pin_t> { };

enum class body_type_e : i32
{
    body_dynamic = 0,
    body_kinematic,
    body_static,
};

struct p_particle_t
{
    f32 mass {1.0f};
    f32 radius {0.01f};
    struct
    {
        vec3 prev {0.0f, 0.0f, 0.0f};
        vec3 now {0.0f, 0.0f, 0.0f};
        vec3 next {0.0f, 0.0f, 0.0f};
    } position;
    struct
    {
        vec3 prev {0.0f, 0.0f, 0.0f};
        vec3 now {0.0f, 0.0f, 0.0f};
        vec3 next {0.0f, 0.0f, 0.0f};
    } velocity;
    struct
    {
        vec3 prev {0.0f, 0.0f, 0.0f};
        vec3 now {0.0f, 0.0f, 0.0f};
        vec3 next {0.0f, 0.0f, 0.0f};
    } acceleration;
    vec3 force {0.0f, 0.0f, 0.0f};

    struct
    {
        std::vector<vec3> position_next;
    } calculations;
};

// using p_particles_t = std::vector<p_particle_t>;
using particles_t = std::vector<particle_t>;

struct p_body_t
{
    body_type_e body_type {body_type_e::body_dynamic};
    particles_t particles;
    unique_indices_t uniq_particles;
};

struct p_constraint_t
{
    constraint_t constraint;

    struct {
        body_t body;
        particle_t particle;
    } A;

    struct {
        body_t body;
        particle_t particle;
    } B;

    f32 length;  // initial length
};

struct p_pin_t
{
    pin_t pin;

    struct {
        body_t body;
        particle_t particle;
    } master;

    struct {
        body_t body;
        particle_t particle;
    } slave;
};

class physics_t
{
    vec3 m_gravity {0.0f, -9.81f, 0.0f};

    struct {
        array_t<p_particle_t, OGP_PHYSICS_NUM_PARTICLES> p_particles;
        array_t<p_body_t,OGP_PHYSICS_NUM_BODIES> p_bodies;
        array_t<p_constraint_t, OGP_PHYSICS_NUM_CONSTRAINTS> p_constraints;
        array_t<p_pin_t, OGP_PHYSICS_NUM_PINS> p_pins;
    } m_db;

    struct {
        array_t<body_t, OGP_PHYSICS_NUM_USER_BODIES> dynamic_bodies;
        array_t<body_t, OGP_PHYSICS_NUM_USER_BODIES> kinematic_bodies;
        array_t<body_t, OGP_PHYSICS_NUM_USER_BODIES> static_bodies;
    } m_user_db;

    unique_indices_t m_pinned_particles;

    void satisfy_pins();

    void satisfy_constraint(constraint_t constraint);

    void satisfy_constraints();

    void solve_verlet(body_t body, f32 dt);

    void make_move(body_t body);

public:

    physics_t() = default;

    physics_t(physics_t const &physics) = delete;

    void step(f32 dt);

    body_t create_body(body_type_e body_type);

    void destroy_body(body_t body);

    void set_body_type(body_t body, body_type_e body_type);

    body_type_e get_body_type(body_t body) const;

    bool body_exists(body_t body) const;

    particle_t create_particle(body_t body, vec3 position);

    void destroy_particle(particle_t particle);

    constraint_t create_constraint(body_t body_a, particle_t particle_A, body_t body_b, particle_t particle_B);

    void destroy_constraint(constraint_t constraint);

    pin_t create_pin(body_t body_master, particle_t master, body_t body_slave, particle_t slave);

    void destroy_pin(pin_t pin);

    bool is_pinned(particle_t particle);

    void set_force(body_t body, vec3 force);

    void add_force(body_t body, vec3 force);

    vec3 get_particle_pos(particle_t particle, f32 frame_dt);

    void set_particle_pos(particle_t particle, vec3 pos);

    void debug_draw(render_context_t *rc, f32 frame_dt);

    void debug_print_stats() const;
};

struct spring_t
{
    i32 index_p0;
    i32 index_p1;
};

// void physics_move(p_particles_t &p_particles);
// void physics_solve_backward_euler(p_particles_t &p_particles, f32 dt);
// void physics_solve_verlet(p_particles_t &p_particles, f32 dt);
// void physics_solve_velocity_verlet(p_particles_t &p_particles, f32 dt);

}  // namespace ogp

#endif  // OGP_PHYSICS_H
