#include "ogp_camera.h"

#include "ogp_defines.h"
#include "ogp_utils.h"
#include "ogp_settings.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

namespace ogp
{

camera_t::camera_t(vec3 position)
{
    m_sposition.set(position);
    set_projection_3d();
}

mat4 camera_t::view() const
{
    return m_view;
}

mat4 camera_t::projection() const
{
    return m_projection;
}

void camera_t::set_projection_2d()
{
    f32 width = settings.size.frame.x;
    f32 height = settings.size.frame.y;

    f32 left = -width / 2.0f;
    f32 right = width / 2.0f;
    f32 bottom = -height / 2.0f;
    f32 top = height / 2.0f;

    m_projection = glm::ortho(left, right, bottom, top, OGP_ORTHO_Z_NEAR, OGP_ORTHO_Z_FAR);
}

void camera_t::set_projection_3d()
{
    m_projection = glm::perspective(this->fov, settings.window_ratio(), OGP_PERSPECTIVE_NEAR, OGP_PERSPECTIVE_FAR);
}

vec3 camera_t::get_position() const
{
    return m_sposition.get();
}

void camera_t::step(quat orientation)
{
    m_orientation = m_orientation_next;
    m_orientation_next = orientation;
}

void camera_t::draw_step(f32 frame_dt)
{
    quat orientation = glm::slerp(m_orientation, m_orientation_next, frame_dt);
    orientation = glm::normalize(orientation);

    mat4 translation = mat4 {1.0f};
    vec3 cam_pos = m_sposition.get(frame_dt);
    translation = glm::translate(translation, -cam_pos);

    m_view = glm::mat4_cast(orientation) * translation;
}

void camera_t::move_to(vec3 point)
{
    m_sposition.step(point);
}

}  // namespace ogp
