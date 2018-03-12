#include "ogp_player.h"

#include "ogp_utils.h"

namespace ogp
{

player_t::player_t(vec3 position)
    : m_sposition(position)
{
}

void player_t::mouse_movement(i32 dx, i32 dy)
{
    f32 sensitivity = 0.525f;

    m_yaw += static_cast<f32>(dx) * sensitivity;
    m_pitch -= static_cast<f32>(dy) * sensitivity;

    m_pitch = angle_unwind(m_pitch);
    m_yaw = angle_unwind(m_yaw);
    m_roll = angle_unwind(m_roll);

    m_pitch = clamp(m_pitch, OGP_CAMERA_PITCH_MIN, OGP_CAMERA_PITCH_MAX);
    m_yaw = clamp(m_yaw, OGP_CAMERA_YAW_MIN, OGP_CAMERA_YAW_MAX);
    m_roll = clamp(m_roll, OGP_CAMERA_ROLL_MIN, OGP_CAMERA_ROLL_MAX);

    // angles
    vec3 tmp_front;

    tmp_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    tmp_front.y = glm::sin(glm::radians(m_pitch));
    tmp_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

    m_front = glm::normalize(tmp_front);

    // up    = glm::normalize(glm::cross(right, front));
    vec3 WORLD_UP {0.0f, 1.0f, 0.0f};
    m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
}

void player_t::move(f32 forward, f32 strafe)
{
    vec3 pos = m_sposition.get_next();
    // vec3 front_fps = vec3 {m_front.x, 0.0f, m_front.z};
    vec3 front_fps = vec3 {m_front.x, m_front.y, m_front.z};

    pos += front_fps * forward;
    pos += m_right * strafe;

    m_sposition.step(pos);
}

vec3 player_t::get_position() const
{
    return m_sposition.get_next();
}

quat player_t::get_orientation() const
{
    quat q_pitch = glm::angleAxis(-glm::radians(m_pitch), vec3 {1.0f, 0.0f, 0.0f});
    quat q_yaw = glm::angleAxis(glm::radians(m_yaw + 90.0f), vec3 {0.0f, 1.0f, 0.0f});
    // quat q_roll = glm::angleAxis(glm::radians(roll), vec3 {0.0f, 0.0f, 1.0f});

    quat q_orientation = q_pitch * q_yaw;
    q_orientation = glm::normalize(q_orientation);

    return q_orientation;
}

}  // namespace ogp
