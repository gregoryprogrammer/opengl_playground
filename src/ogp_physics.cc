#include "ogp_physics.h"

#include "ogp_render.h"
#include "ogp_utils.h"

#include <numeric>

namespace ogp
{

void physics_t::satisfy_pins()
{
    for (p_pin_t const &p_pin : m_db.p_pins) {
        p_particle_t *m = m_db.p_particles.get(p_pin.master.particle.index);
        p_particle_t *s = m_db.p_particles.get(p_pin.slave.particle.index);

        s->position.prev = m->position.prev;
        s->position.now = m->position.now;
        s->position.next = m->position.next;

        s->velocity.prev = m->velocity.prev;
        s->velocity.now = m->velocity.now;
        s->velocity.next = m->velocity.next;

        s->acceleration.prev = m->acceleration.prev;
        s->acceleration.now = m->acceleration.now;
        s->acceleration.next = m->acceleration.next;
    }
}

void physics_t::satisfy_constraint(constraint_t constraint)
{
    constexpr i32 NUM_ITERATIONS = 16;

    p_constraint_t const *p_constraint = m_db.p_constraints.get(constraint.index);

    p_particle_t *pa = m_db.p_particles.get(p_constraint->A.particle.index);
    p_particle_t *pb = m_db.p_particles.get(p_constraint->B.particle.index);

    f32 length = p_constraint->length;

    for (i32 j = 0; j < NUM_ITERATIONS; ++j) {

        vec3 pa_pos = pa->position.next;
        vec3 pb_pos = pb->position.next;

        vec3 delta = pb_pos - pa_pos;
        f32 delta_length = glm::length(delta);
        f32 diff = (delta_length - length) / delta_length;  // TODO c.L = c.rest_length

        bool pinned_a = is_pinned(p_constraint->A.particle) || (get_body_type(p_constraint->A.body) == body_type_e::body_static);
        bool pinned_b = is_pinned(p_constraint->B.particle) || (get_body_type(p_constraint->B.body) == body_type_e::body_static);

        f32 coeff_a = 0.5f;
        f32 coeff_b = 0.5f;

        // TODO optimize, someday
        if (!pinned_a && pinned_b) {
            coeff_a = 1.0f;
            coeff_b = 0.0f;
        }
        else if (pinned_a && !pinned_b) {
            coeff_a = 0.0f;
            coeff_b = 1.0f;
        }
        else if (pinned_a && pinned_b) {
            coeff_a = 0.0f;
            coeff_b = 0.0f;
        }

        pa_pos += delta * coeff_a * diff;
        pb_pos -= delta * coeff_b * diff;

        pa->calculations.position_next.push_back(pa_pos);
        pb->calculations.position_next.push_back(pb_pos);
    }
}

void physics_t::satisfy_constraints()
{
    for (p_constraint_t const &p_constraint : m_db.p_constraints) {
        satisfy_constraint(p_constraint.constraint);
    }

    // TODO only for constrained particles
    for (p_particle_t &p_particle : m_db.p_particles) {
        size_t s = p_particle.calculations.position_next.size();
        if (s == 0) continue;
        auto begin = std::begin(p_particle.calculations.position_next);
        auto end = std::end(p_particle.calculations.position_next);
        vec3 pos = std::accumulate(begin, end, vec3 {0.0f, 0.0f, 0.0f});
        pos = pos * (1.0f / s);

        p_particle.position.next = pos;
        p_particle.calculations.position_next.clear();
    }
}

void physics_t::step(f32 dt)
{
    for (body_t const &body : m_user_db.dynamic_bodies) {
        add_force(body, m_gravity);
    }

    // TODO satisfy_springs

    for (body_t const &body : m_user_db.dynamic_bodies) {
        solve_verlet(body, dt);
    }

    satisfy_constraints();

    for (body_t const &body : m_user_db.dynamic_bodies) {
        set_force(body, {0.0f, 0.0f, 0.0f});
    }

    satisfy_pins();

    for (body_t const &body : m_user_db.dynamic_bodies) {
        make_move(body);
    }
}

body_t physics_t::create_body(body_type_e body_type)
{
    p_body_t p_body {};
    p_body.body_type = body_type;

    body_t body;
    body.index = m_db.p_bodies.add(p_body);

    if (body_type == body_type_e::body_dynamic) {
        m_user_db.dynamic_bodies.add(body);
    }
    else if (body_type == body_type_e::body_kinematic) {
        m_user_db.kinematic_bodies.add(body);
    }
    else if (body_type == body_type_e::body_static) {
        m_user_db.static_bodies.add(body);
    }

    ogp_log_info("Body create: index = %d", body.index.value);

    return body;
}

void physics_t::destroy_body(body_t body)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);

    if (p_body == nullptr) {
        ogp_log_warning("there is no such body you want destroy");
        return;
    }

    // TODO
    // in this order:
    // [x] 1. destroy related constraints
    // [x] 2. destroy related pins
    // [x] 3. destroy related particles
    // [x] 4. destroy body and its edges

    // (1) DESTROY BODY RELATED CONSTRAINTS ....................................

    std::vector<constraint_t> constraints_to_remove;
    for (p_constraint_t const &p_constraint : m_db.p_constraints) {
        bool test_a = (body.index == p_constraint.A.body.index);
        bool test_b = (body.index == p_constraint.B.body.index);
        if (test_a || test_b) {
            constraints_to_remove.push_back(p_constraint.constraint);
        }
    }
    for (constraint_t const &constraint : constraints_to_remove) {
        destroy_constraint(constraint);
    }

    // (2) DESTROY BODY RELATED PINS ...........................................

    std::vector<pin_t> pins_to_remove;
    for (p_pin_t const &p_pin : m_db.p_pins) {
        bool test_master = (body.index == p_pin.master.body.index);
        bool test_slave = (body.index == p_pin.slave.body.index);
        if (test_master || test_slave) {
            pins_to_remove.push_back(p_pin.pin);
        }
    }
    for (pin_t const &pin : pins_to_remove) {
        destroy_pin(pin);
    }

    // (3) DESTROY RELATED PARTICLES ...........................................

    for (particle_t const &particle : p_body->particles) {
        m_db.p_particles.remove_index(particle.index);
    }
    p_body->particles.clear();

    // (4) DESTROY BODY ........................................................

    if (p_body->body_type == body_type_e::body_dynamic) {
        m_user_db.dynamic_bodies.remove_element(body);
    }
    else if (p_body->body_type == body_type_e::body_kinematic) {
        m_user_db.kinematic_bodies.remove_element(body);
    }
    else if (p_body->body_type == body_type_e::body_static) {
        m_user_db.static_bodies.remove_element(body);
    }

    ogp_log_info("Body destroy: index = %d", body.index.value);
    m_db.p_bodies.remove_index(body.index);
}

void physics_t::set_body_type(body_t body, body_type_e body_type)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);
    if (p_body == nullptr) return;

    m_user_db.dynamic_bodies.remove_element(body);
    m_user_db.kinematic_bodies.remove_element(body);
    m_user_db.static_bodies.remove_element(body);

    p_body->body_type = body_type;

    if (body_type == body_type_e::body_dynamic) {
        m_user_db.dynamic_bodies.add(body);
    }
    else if (body_type == body_type_e::body_kinematic) {
        m_user_db.kinematic_bodies.add(body);
    }
    else if (body_type == body_type_e::body_static) {
        m_user_db.static_bodies.add(body);
    }
}

body_type_e physics_t::get_body_type(body_t body) const
{
    return m_db.p_bodies.get(body.index)->body_type;
}

bool physics_t::body_exists(body_t body) const
{
    return m_db.p_bodies.exists(body.index);
}

void physics_t::set_force(body_t body, vec3 force)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);
    if (p_body == nullptr) return;

    for (particle_t const &particle : p_body->particles) {
        p_particle_t *p_particle = m_db.p_particles.get(particle.index);
        p_particle->force = force;
    }
}

void physics_t::add_force(body_t body, vec3 force)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);
    if (p_body == nullptr) return;

    for (particle_t const &particle : p_body->particles) {
        p_particle_t *p_particle = m_db.p_particles.get(particle.index);
        p_particle->force += force;
    }
}

vec3 physics_t::get_particle_pos(particle_t particle, f32 frame_dt)
{
    p_particle_t const *p_particle = m_db.p_particles.get(particle.index);
    NULL_WARNING(p_particle);
    if (p_particle == nullptr) return vec3 {0.0f, 0.0f, 0.0f};

    vec3 now = p_particle->position.prev;
    vec3 next = p_particle->position.next;

    vec3 interp = now + (next - now) * frame_dt;
    return interp;
}

void physics_t::set_particle_pos(particle_t particle, vec3 pos)
{
    p_particle_t *p_particle = m_db.p_particles.get(particle.index);
    NULL_WARNING(p_particle);
    if (p_particle == nullptr) return;

    p_particle->position.prev = pos;
    p_particle->position.now  = pos;
    p_particle->position.next = pos;
}

void physics_t::solve_verlet(body_t body, f32 dt)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);
    if (p_body == nullptr) return;

    for (particle_t const &particle : p_body->particles) {

        if (is_pinned(particle)) continue;

        p_particle_t *p = m_db.p_particles.get(particle.index);
        vec3 r_now  = p->position.now;
        vec3 r_prev = p->position.prev;

        vec3 a_now = p->force / p->mass;

        vec3 r_next = r_now * 2.0f - r_prev + a_now * dt * dt;
        vec3 v_next = (r_next - r_prev) * (1.0f / dt) * 0.5f;

        p->position.next = r_next;
        p->velocity.next = v_next;
    }
}

void physics_t::make_move(body_t body)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);
    if (p_body == nullptr) return;

    for (particle_t const &particle : p_body->particles) {
        p_particle_t *p = m_db.p_particles.get(particle.index);

        p->position.prev = p->position.now;
        p->position.now = p->position.next;

        p->velocity.prev = p->velocity.now;
        p->velocity.now = p->velocity.next;

        p->acceleration.prev = p->acceleration.now;
        p->acceleration.now = p->acceleration.next;
    }
}

particle_t physics_t::create_particle(body_t body, vec3 position)
{
    p_body_t *p_body = m_db.p_bodies.get(body.index);
    NULL_WARNING(p_body);

    particle_t particle {};

    if (p_body == nullptr) return particle;

    p_particle_t p_particle;
    p_particle.position.prev = position;
    p_particle.position.now = position;
    p_particle.position.next = position;

    particle.index = m_db.p_particles.add(p_particle);

    p_body->particles.push_back(particle);
    p_body->uniq_particles.insert(particle.index);

    return particle;
}

void physics_t::destroy_particle(particle_t particle)
{
    // FIXME destroy hell (look: destroy_body)
    // (1) DESTROY PARTICLE RELATED CONSTRAINTS ................................

    std::vector<constraint_t> constraints_to_remove;
    for (p_constraint_t const &p_constraint : m_db.p_constraints) {
        bool test_a = (particle.index == p_constraint.A.particle.index);
        bool test_b = (particle.index == p_constraint.B.particle.index);
        if (test_a || test_b) {
            constraints_to_remove.push_back(p_constraint.constraint);
        }
    }
    for (constraint_t const &constraint : constraints_to_remove) {
        destroy_constraint(constraint);
    }

    // (2) DESTROY BODY RELATED PINS ...........................................

    std::vector<pin_t> pins_to_remove;
    for (p_pin_t const &p_pin : m_db.p_pins) {
        bool test_master = (particle.index == p_pin.master.particle.index);
        bool test_slave = (particle.index == p_pin.slave.particle.index);
        if (test_master || test_slave) {
            pins_to_remove.push_back(p_pin.pin);
        }
    }
    for (pin_t const &pin : pins_to_remove) {
        destroy_pin(pin);
    }
}

constraint_t physics_t::create_constraint(body_t body_A, particle_t particle_A, body_t body_B, particle_t particle_B)  // done
{
    p_body_t *p_body_A = m_db.p_bodies.get(body_A.index);
    p_body_t *p_body_B = m_db.p_bodies.get(body_B.index);

    NULL_WARNING(p_body_A);
    NULL_WARNING(p_body_B);

    if (p_body_A == nullptr || p_body_B == nullptr) std::terminate();

    // TODO check in_A and in_B
    bool in_A = p_body_A->uniq_particles.count(particle_A.index) == 1;
    bool in_B = p_body_B->uniq_particles.count(particle_B.index) == 1;
    if (!in_A) {
        ogp_log_error("there is no such particle %d in body %d", particle_A.index.value, body_A.index.value);
        terminate("");
    }
    if (!in_B) {
        ogp_log_error("there is no such particle %d in body %d", particle_B.index.value, body_B.index.value);
        terminate("");
    }

    p_particle_t *p_particle_A = m_db.p_particles.get(particle_A.index);
    p_particle_t *p_particle_B = m_db.p_particles.get(particle_B.index);

    NULL_WARNING(p_particle_A);
    NULL_WARNING(p_particle_B);

    constraint_t constraint {};
    constraint.index = index_t::invalid();

    if (p_particle_A == nullptr || p_particle_B == nullptr) {
        TODO("what to do with enexpected nullptr?");
        terminate("unexpected nullptr");
        return constraint;
    }

    vec3 pos_A = p_particle_A->position.now;
    vec3 pos_B = p_particle_B->position.now;

    p_constraint_t p_constraint {};
    p_constraint.A.particle = particle_A;
    p_constraint.A.body = body_A;;
    p_constraint.B.particle = particle_B;
    p_constraint.B.body = body_B;;
    p_constraint.length = glm::distance(pos_A, pos_B);

    constraint.index = m_db.p_constraints.add(p_constraint);

    // assign constraint index holder to its own definition (p_constraint)
    m_db.p_constraints.get(constraint.index)->constraint = constraint;

    return constraint;
}

void physics_t::destroy_constraint(constraint_t constraint)  // done
{
    m_db.p_constraints.remove_index(constraint.index);
}

pin_t physics_t::create_pin(body_t body_master, particle_t master, body_t body_slave, particle_t slave)
{
    p_pin_t p_pin {};
    p_pin.master.body = body_master;
    p_pin.master.particle = master;
    p_pin.slave.body = body_slave;
    p_pin.slave.particle = slave;

    // FIXME
    m_pinned_particles.insert(slave.index);

    pin_t pin {};
    pin.index = m_db.p_pins.add(p_pin);

    // assign pin index holder to its own definition (p_pin)
    m_db.p_pins.get(pin.index)->pin = pin;

    return pin;
}

void physics_t::destroy_pin(pin_t pin)
{
    m_db.p_pins.remove_index(pin.index);
}

bool physics_t::is_pinned(particle_t particle)
{
    return m_pinned_particles.count(particle.index) == 1;
}

void physics_t::debug_draw(render_context_t *rc, f32 frame_dt)
{
    // DRAW PARTICLES ..........................................................

    rc->set_line_color(230, 230, 230);
    for (p_particle_t const &p_particle : m_db.p_particles) {
        vec3 pos_now = p_particle.position.now;
        vec3 pos_next = p_particle.position.next;
        vec3 pos = pos_now + (pos_next - pos_now) * frame_dt;
        rc->draw_point(pos);
    }

    // DRAW CONSTRAINTS ........................................................

    rc->set_line_color(245, 204, 77);
    for (p_constraint_t const &p_constraint : m_db.p_constraints) {
        vec3 pos_A = get_particle_pos(p_constraint.A.particle, frame_dt);
        vec3 pos_B = get_particle_pos(p_constraint.B.particle, frame_dt);

        rc->draw_line(pos_A, pos_B);
    }
}

void physics_t::debug_print_stats() const
{
    ogp_log_info("physics_t stats :");
    ogp_log_info("    particles   : %d", m_db.p_particles.size());
    ogp_log_info("    bodies      : %d", m_db.p_bodies.size());
    ogp_log_info("    constraints : %d", m_db.p_constraints.size());
    ogp_log_info("    pins        : %d", m_db.p_pins.size());
}


// FROM OLD ....................................................................

/*
void physics_move(p_particles_t &p_particles)
{
    for (p_particle_t &p : p_particles) {
        p.position.prev = p.position.now;
        p.position.now = p.position.next;

        p.velocity.prev = p.velocity.now;
        p.velocity.now = p.velocity.next;

        p.acceleration.prev = p.acceleration.now;
        p.acceleration.now = p.acceleration.next;
    }
}


void physics_solve_backward_euler(p_particles_t &p_particles, f32 dt)
{
    for (p_particle_t &p : p_particles) {
        vec3 r_now  = p.position.now;
        vec3 v_now  = p.velocity.now;

        vec3 a_now = p.force / p.mass;

        vec3 a_next = a_now;
        vec3 v_next = v_now + a_next * dt;
        vec3 r_next = r_now + v_next * dt;

        p.position.next = r_next;
        p.velocity.next = v_next;
        p.acceleration.next = a_next;
    }
}


void physics_solve_verlet(p_particles_t &p_particles, f32 dt)
{
    for (p_particle_t &p : p_particles) {
        vec3 r_now  = p.position.now;
        vec3 r_prev = p.position.prev;

        vec3 a_now = p.force / p.mass;

        vec3 r_next = r_now * 2.0f - r_prev + a_now * dt * dt;
        vec3 v_next = (r_next - r_prev) * (1.0f / dt) * 0.5f;

        p.position.next = r_next;
        p.velocity.next = v_next;
    }
}

void physics_solve_velocity_verlet(p_particles_t &p_particles, f32 dt)
{
    for (p_particle_t &p : p_particles) {
        vec3 r_now = p.position.now;
        vec3 v_now = p.velocity.now;
        vec3 a_now = p.acceleration.now;

        f32 mass = p.mass;
        vec3 a_next = p.force / mass;

        vec3 r_next = r_now + v_now * dt + (a_now / 2.0f) * dt * dt;
        vec3 v_next = v_now + ((a_next + a_now) / 2.0f) * dt;

        p.position.next = r_next;
        p.velocity.next = v_next;

        p.acceleration.next = a_next;
    }
}
*/

/*
static void physics_spring_t_set_force_all(p_particle2d_t *p, i32 num, vec2 force)
{
    for (i32 i = 0; i < num; ++i) {
        p->force = force;
        p = p + 1;
    }
}

void physics2d_force_bodies(physics_spring_t *physics, p_body2d_t *b, i32 num, vec2 force)
{
    for (i32 i = 0; i < num; ++i) {

        if (b->type == body_type_e::body_static || b->type == body_type_e::body_kinematic) {
            b = b + 1;  // FIXME bug-prone
            continue;
        }

        i32 num_body_particles = b->num_particles;

        for (i32 j = 0; j < num_body_particles; ++j) {
            index_t p_id = b->particles[j];
            p_particle2d_t *p = physics->db.particles.get(p_id);
            p->force += force;
            p = p + 1;
        }

        b = b + 1;
    }
}

static void physics2d_satisfy_pins(physics_spring_t *physics, p_pin_t *p, i32 num)
{
    for (i32 i = 0; i < num; ++i) {
        index_t pm_id = p->master.particle;
        index_t ps_id = p->slave.particle;

        p_particle2d_t *pm = physics->db.particles.get(pm_id);
        p_particle2d_t *ps = physics->db.particles.get(ps_id);

        ps->position.prev = pm->position.prev;
        ps->position.now = pm->position.now;
        ps->position.next = pm->position.next;

        ps->velocity.prev = pm->velocity.prev;
        ps->velocity.now = pm->velocity.now;
        ps->velocity.next = pm->velocity.next;

        ps->acceleration.prev = pm->acceleration.prev;
        ps->acceleration.now = pm->acceleration.now;
        ps->acceleration.next = pm->acceleration.next;

        p = p + 1;
    }
}

static void physics2d_satisfy_constraints(physics_spring_t *physics, p_constraint_t *p_constraint, i32 num)
{
    constexpr i32 NUM_ITERATIONS = 16;

    for (i32 i = 0; i < num; ++i) {
        index_t pa_id = p_constraint->a.particle;
        index_t pb_id = p_constraint->b.particle;

        p_particle2d_t *pa = physics->db.particles.get(pa_id);
        p_particle2d_t *pb = physics->db.particles.get(pb_id);

        f32 length = p_constraint->length;

        for (i32 j = 0; j < NUM_ITERATIONS; ++j) {

            vec2 pa_pos = pa->position.next;
            vec2 pb_pos = pb->position.next;

            auto delta = pb_pos - pa_pos;
            auto delta_length = vec2_length(delta);
            auto diff = (delta_length - length) / delta_length;  // TODO c.L = c.rest_length

            pa_pos += delta * 0.5f * diff;
            pb_pos -= delta * 0.5f * diff;

            pa->position.next = pa_pos;
            pb->position.next = pb_pos;
        }

        p_constraint = p_constraint + 1;
    }

}

static void physics2d_satisfy_springs(physics_spring_t *physics, p_spring_t *p_spring, i32 num)
{
    for (i32 i = 0; i < num; ++i) {
        index_t pa_id = p_spring->a.particle;
        index_t pb_id = p_spring->b.particle;
        f32 init_length = p_spring->length;
        f32 k = p_spring->springness;

        p_particle2d_t *p_particle_a = physics->db.particles.get(pa_id);
        p_particle2d_t *p_particle_b = physics->db.particles.get(pb_id);

        vec2 pa_pos = p_particle_a->position.next;
        vec2 pb_pos = p_particle_b->position.next;

        vec2 delta = pb_pos - pa_pos;
        f32 diff = vec2_length(delta);

        f32 d = (diff - init_length) / diff;

        glm::normalize(delta);

        vec2 x = delta * d;

        vec2 spring_force = -k * x;

        // FIXME damping

        p_particle_a->force += -spring_force;
        p_particle_b->force +=  spring_force;

        p_spring = p_spring + 1;
    }

}

*/

}  // namespace ogp
