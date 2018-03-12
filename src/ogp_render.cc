#include "ogp_render.h"

#include "ogp_fx.h"
#include "ogp_input.h"
#include "ogp_math.h"
#include "ogp_settings.h"
#include "ogp_utils.h"

#include <vector>

namespace ogp
{

color_t::color_t(u8 r, u8 g, u8 b, u8 a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

color_t color_t::from_float(f32 r, f32 g, f32 b, f32 a)
{
    color_t color {0, 0, 0, 0};
    color.r = static_cast<u8>(r * 255.0f);
    color.g = static_cast<u8>(g * 255.0f);
    color.b = static_cast<u8>(b * 255.0f);
    color.a = static_cast<u8>(a * 255.0f);
    return color;
}

vec3 color_t::to_vec3(color_t color)
{
    vec3 fcolor {1.0f, 1.0f, 1.0f};
    fcolor.x = static_cast<f32>(color.r) / 255.0f;
    fcolor.y = static_cast<f32>(color.g) / 255.0f;
    fcolor.z = static_cast<f32>(color.b) / 255.0f;
    return fcolor;
}

/* .------------------------------.
 * |                              |
 * |        RENDER CONTEXT        |
 * |                              |
 * '------------------------------'
 */

render_context_t::render_context_t ()
{
    m_render_buffer.create_batch(&m_requests);
}

void render_context_t::reset()
{
    m_requests.points.reset();
    m_requests.lines.reset();
    m_requests.color_faces.reset();
    m_requests.debug_shapes.reset();
    m_requests.point_lights.reset();
    m_requests.directional_lights.reset();
    m_requests.color_meshes.reset();

    m_line_color = color_t {0xFF, 0xFF, 0xFF, 0xFF};
    m_fill_color = color_t {0xFF, 0xFF, 0xFF, 0xFF};

    m_transform.translation = vec3{0.0f, 0.0f, 0.0f};
    m_transform.rotation    = vec3{0.0f, 0.0f, 0.0f};
    m_transform.origin      = vec3{0.0f, 0.0f, 0.0f};
}

void render_context_t::update_vbo()
{
    m_render_buffer.update_vbo(&m_requests);
}

void render_context_t::render(camera_t const *camera)
{
    m_render_buffer.render(camera);
}

color_t render_context_t::set_line_color(color_t color)
{
    return set_line_color(color.r, color.g, color.b, color.a);
}

color_t render_context_t::set_line_color(u8 r, u8 g, u8 b, u8 a)
{
    color_t prev = m_line_color;
    m_line_color.r = r;
    m_line_color.g = g;
    m_line_color.b = b;
    m_line_color.a = a;
    return prev;
}

color_t render_context_t::set_fill_color(color_t color)
{
    return set_fill_color(color.r, color.g, color.b, color.a);
}

color_t render_context_t::set_fill_color(u8 r, u8 g, u8 b, u8 a)
{
    color_t prev = m_fill_color;
    m_fill_color.r = r;
    m_fill_color.g = g;
    m_fill_color.b = b;
    m_fill_color.a = a;
    return prev;
}

vec3 render_context_t::set_translation(vec3 translation)
{
    vec3 prev = m_transform.translation;
    m_transform.translation = translation;
    return prev;
}

vec3 render_context_t::set_rotation(vec3 rotation)
{
    vec3 prev = m_transform.rotation;
    m_transform.rotation = rotation;
    return prev;
}

vec3 render_context_t::set_origin(vec3 origin)
{
    vec3 prev = m_transform.origin;
    m_transform.origin = origin;
    return prev;
}

void render_context_t::draw_point(vec3 position)
{
    rr_point_t rr_point = rr_point_t {};
    rr_point.v.position   = position;
    rr_point.v.color      = m_line_color;

    rr_point.transform = m_transform;

    m_requests.points.add(rr_point);
}

void render_context_t::draw_line(vec3 begin, vec3 end)
{
    rr_line_t rr_line   = rr_line_t{};
    rr_line.v0.position = begin;
    rr_line.v1.position = end;
    rr_line.v0.color    = m_line_color;
    rr_line.v1.color    = m_line_color;

    rr_line.transform = m_transform;

    m_requests.lines.add(rr_line);
}

void render_context_t::draw_color_face(vec3 p0, vec3 p1, vec3 p2)
{
    rr_color_face_t rr_color_face = rr_color_face_t {};

    rr_color_face.v0.position = p0;
    rr_color_face.v1.position = p1;
    rr_color_face.v2.position = p2;

    rr_color_face.v0.color = m_fill_color;
    rr_color_face.v1.color = m_fill_color;
    rr_color_face.v2.color = m_fill_color;

    rr_color_face.transform = m_transform;

    m_requests.color_faces.add(rr_color_face);
}

void render_context_t::draw_color_quad(vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    draw_color_face(p0, p1, p2);
    draw_color_face(p2, p1, p3);
}

void render_context_t::draw_cube()
{
    vec3 p0 = vec3 {0.0f, 0.0f, 1.0f};
    vec3 p1 = vec3 {1.0f, 0.0f, 1.0f};
    vec3 p2 = vec3 {0.0f, 0.0f, 0.0f};
    vec3 p3 = vec3 {1.0f, 0.0f, 0.0f};

    vec3 p4 = vec3 {0.0f, 1.0f, 1.0f};
    vec3 p5 = vec3 {1.0f, 1.0f, 1.0f};
    vec3 p6 = vec3 {0.0f, 1.0f, 0.0f};
    vec3 p7 = vec3 {1.0f, 1.0f, 0.0f};

    // front
    draw_line(p0, p1);
    draw_line(p0, p4);
    draw_line(p1, p5);
    draw_line(p4, p5);

    // back
    draw_line(p2, p3);
    draw_line(p2, p6);
    draw_line(p3, p7);
    draw_line(p6, p7);

    // cross bars
    draw_line(p0, p2);
    draw_line(p1, p3);
    draw_line(p4, p6);
    draw_line(p5, p7);
}

void render_context_t::draw_color_mesh(mesh_t mesh, transform_t transform)
{
    rr_color_mesh_t rr_color_mesh {};
    rr_color_mesh.mesh = mesh;
    rr_color_mesh.transform = transform;

    m_requests.color_meshes.add(rr_color_mesh);
}

void render_context_t::add_point_light(vec3 position, color_t color)
{
    rr_point_light_t light {};
    light.position = position;
    light.ambient = color;
    light.diffuse = color;
    light.specular = color;
    m_requests.point_lights.add(light);

    // debug cube
    rr_debug_shape_t rr_debug_shape {};
    rr_debug_shape.position = position;
    rr_debug_shape.color = color;
    m_requests.debug_shapes.add(rr_debug_shape);
}

void render_context_t::add_directional_light(vec3 direction, color_t color)
{
    rr_directional_light_t light {};
    light.direction = direction;
    light.ambient = color;
    light.diffuse = color;
    light.specular = color;
    m_requests.directional_lights.add(light);

    // debug cube
    rr_debug_shape_t rr_debug_shape {};
    rr_debug_shape.position = vec3 {0.0f, 2.0f, 0.0f};
    rr_debug_shape.color = color;
    m_requests.debug_shapes.add(rr_debug_shape);
}

/* .-----------------------------.
 * |                             |
 * |        RENDER BUFFER        |
 * |                             |
 * '-----------------------------'
 */

void render_buffer_t::create_batch(r_requests_t const *r_requests)
{
    // PRIMITIVES
    {
        i32 num_points = r_requests->points.capacity();
        i32 num_lines = r_requests->lines.capacity();
        size_t points_size = sizeof(vertex_PC_t) * num_points;
        size_t lines_size  = sizeof(vertex_PC_t) * num_lines * 2;

        ogp_log_me("points size = %u", points_size);
        ogp_log_me("lines size = %u", lines_size);

        size_t data_size = points_size + lines_size;
        size_t indices_size = num_points + num_lines * 2;

        m_primitives.init();

        glBindVertexArray(m_primitives.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_primitives.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_primitives.VBO);
        glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);

        GLsizei stride = sizeof(vertex_PC_t);

        u32 base_offset     = 0;
        u32 offset_position = base_offset + offsetof(vertex_PC_t, position);
        u32 offset_color    = base_offset + offsetof(vertex_PC_t, color);

        GLuint loc_position = static_cast<GLuint>(glsl_loc_e::position);
        GLuint loc_color    = static_cast<GLuint>(glsl_loc_e::color);

        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_position));
        glEnableVertexAttribArray(loc_position);

        glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<void *>(offset_color));
        glEnableVertexAttribArray(loc_color);

        glBindVertexArray(0);
    }

    // FACES
    {
        i32 num_color_faces = r_requests->color_faces.capacity();
        size_t color_faces_size  = sizeof(vertex_PNC_t) * num_color_faces * 3;
        size_t data_size = color_faces_size;
        size_t indices_size = num_color_faces * 3;

        ogp_log_me("color_faces size = %u", color_faces_size);

        m_color_faces.init();

        glBindVertexArray(m_color_faces.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_color_faces.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_color_faces.VBO);
        glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);

        GLsizei stride = sizeof(vertex_PNC_t);

        u32 base_offset     = 0;
        u32 offset_position = base_offset + offsetof(vertex_PNC_t, position);
        u32 offset_normal   = base_offset + offsetof(vertex_PNC_t, normal);
        u32 offset_color    = base_offset + offsetof(vertex_PNC_t, color);

        GLuint loc_position = static_cast<GLuint>(glsl_loc_e::position);
        GLuint loc_normal   = static_cast<GLuint>(glsl_loc_e::normal);
        GLuint loc_color    = static_cast<GLuint>(glsl_loc_e::color);

        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_position));
        glEnableVertexAttribArray(loc_position);

        glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_normal));
        glEnableVertexAttribArray(loc_normal);

        glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<void *>(offset_color));
        glEnableVertexAttribArray(loc_color);

        glBindVertexArray(0);
    }

    // LIGHTS
    {
        i32 num_debug_cubes = r_requests->point_lights.capacity();
        size_t cubes_size = sizeof(vertex_PC_t) * 8 * num_debug_cubes;

        ogp_log_me("debug_cubes size = %u", cubes_size);

        size_t data_size = cubes_size;
        size_t indices_size = cubes_size;

        m_debug_shapes.init();

        glBindVertexArray(m_debug_shapes.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_debug_shapes.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_debug_shapes.VBO);
        glBufferData(GL_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);

        GLsizei stride = sizeof(vertex_PC_t);

        u32 base_offset     = 0;
        u32 offset_position = base_offset + offsetof(vertex_PC_t, position);
        u32 offset_color    = base_offset + offsetof(vertex_PC_t, color);

        GLuint loc_position = static_cast<GLuint>(glsl_loc_e::position);
        GLuint loc_color    = static_cast<GLuint>(glsl_loc_e::color);

        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_position));
        glEnableVertexAttribArray(loc_position);

        glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<void *>(offset_color));
        glEnableVertexAttribArray(loc_color);

        glBindVertexArray(0);
    }
}

void render_buffer_t::update_vbo(r_requests_t const *r_requests)
{
    vertices_PC_t vertices_PC;
    vertices_PNC_t vertices_PNC;
    std::vector<GLuint> indices_PC;
    std::vector<GLuint> indices_PNC;

    GLuint index_PC = 0;
    GLuint index_PNC = 0;

    // VBO POINTS ..............................................................

    for (auto rr_point : r_requests->points) {

        vec3 origin = rr_point.transform.origin;
        vec3 translation = rr_point.transform.translation;
        vec3 rotation = rr_point.transform.rotation;

        quat orientation = euler_to_quat(rotation);
        orientation = glm::normalize(orientation);

        vec3 p = rr_point.v.position;
        p = orientation * (p + translation - origin) + origin;

        rr_point.v.position = p;

        vertices_PC.push_back(rr_point.v);
        indices_PC.push_back(index_PC++);
    }

    m_draw_call_points.mode = GL_POINTS;
    m_draw_call_points.num_indices = indices_PC.size() - m_draw_call_points.offset;
    m_draw_call_points.offset = 0;

    // VBO LINES ...............................................................

    m_draw_call_lines.offset = index_PC;

    for (auto rr_line : r_requests->lines) {

        vec3 origin = rr_line.transform.origin;
        vec3 translation = rr_line.transform.translation;
        vec3 rotation = rr_line.transform.rotation;

        quat orientation = euler_to_quat(rotation);
        orientation = glm::normalize(orientation);

        vec3 p0 = rr_line.v0.position;
        vec3 p1 = rr_line.v1.position;

        p0 = orientation * (p0 + translation - origin) + origin;
        p1 = orientation * (p1 + translation - origin) + origin;

        rr_line.v0.position = p0;
        rr_line.v1.position = p1;

        vertices_PC.push_back(rr_line.v0);
        vertices_PC.push_back(rr_line.v1);
        indices_PC.push_back(index_PC++);
        indices_PC.push_back(index_PC++);
    }

    m_draw_call_lines.mode = GL_LINES;
    m_draw_call_lines.num_indices = indices_PC.size() - m_draw_call_lines.offset;

    // VBO FACES ...............................................................

    m_draw_call_color_faces.offset = index_PNC;

    for (auto rr_face : r_requests->color_faces) {

        vec3 origin = rr_face.transform.origin;
        vec3 translation = rr_face.transform.translation;
        vec3 rotation = rr_face.transform.rotation;

        quat orientation = euler_to_quat(rotation);
        orientation = glm::normalize(orientation);

        vec3 p0 = rr_face.v0.position;
        vec3 p1 = rr_face.v1.position;
        vec3 p2 = rr_face.v2.position;

        p0 = orientation * (p0 + translation - origin) + origin;
        p1 = orientation * (p1 + translation - origin) + origin;
        p2 = orientation * (p2 + translation - origin) + origin;

        rr_face.v0.position = p0;
        rr_face.v1.position = p1;
        rr_face.v2.position = p2;

        vertex_PNC_t v0;
        vertex_PNC_t v1;
        vertex_PNC_t v2;

        v0.position = p0;
        v1.position = p1;
        v2.position = p2;

        vec3 a = p1 - p0;
        vec3 b = p2 - p0;
        vec3 normal = glm::normalize(glm::cross(a, b));

        v0.normal = normal;
        v1.normal = normal;
        v2.normal = normal;

        v0.color = rr_face.v0.color;
        v1.color = rr_face.v1.color;
        v2.color = rr_face.v2.color;

        vertices_PNC.push_back(v0);
        vertices_PNC.push_back(v1);
        vertices_PNC.push_back(v2);
        indices_PNC.push_back(index_PNC++);
        indices_PNC.push_back(index_PNC++);
        indices_PNC.push_back(index_PNC++);
    }

    m_draw_call_color_faces.mode = GL_TRIANGLES;
    m_draw_call_color_faces.num_indices = indices_PNC.size() - m_draw_call_color_faces.offset;

    // primitives
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_primitives.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_PC.size() * sizeof(GLuint), &indices_PC[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_primitives.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_PC.size() * sizeof(vertex_PC_t), &vertices_PC[0], GL_DYNAMIC_DRAW);

    // faces
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_color_faces.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_PNC.size() * sizeof(GLuint), &indices_PNC[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_color_faces.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_PNC.size() * sizeof(vertex_PNC_t), &vertices_PNC[0], GL_DYNAMIC_DRAW);

    // debug cubes
    {
        std::vector<vertex_PC_t> vertices_PC;
        std::vector<GLuint> indices_PC;

        GLuint index = 0;
        for (rr_debug_shape_t const &rr_debug_shape : r_requests->debug_shapes) {
            vec3 p = rr_debug_shape.position;
            color_t c = rr_debug_shape.color;

            f32 edge = 0.05f;
            for (vec3 const v : OGP_CUBE_VERTICES) {
                vertex_PC_t vertex {p + v * edge, c};
                vertices_PC.push_back(vertex);
            }

            for (GLuint const u : OGP_CUBE_INDICES) {
                indices_PC.push_back(index + u);
            }

            index += 8;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_debug_shapes.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_PC.size() * sizeof(GLuint), &indices_PC[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_debug_shapes.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices_PC.size() * sizeof(vertex_PC_t), &vertices_PC[0], GL_DYNAMIC_DRAW);

        m_draw_call_debug.mode = GL_TRIANGLES;
        m_draw_call_debug.num_indices = indices_PC.size();
    }

    // copy point lights to buffer
    m_point_lights.clear();
    for (rr_point_light_t const &rr_point_light : r_requests->point_lights) {
        m_point_lights.push_back(rr_point_light);
    }

    // copy directional lights to buffer
    m_directional_lights.clear();
    for (rr_directional_light_t const &rr_directional_light : r_requests->directional_lights) {
        m_directional_lights.push_back(rr_directional_light);
    }

    // copy color mesh requests
    m_color_meshes.clear();
    for (rr_color_mesh_t const &rr_color_mesh : r_requests->color_meshes) {
        m_color_meshes.push_back(rr_color_mesh);
    }

    gl_check_error();
}

void render_buffer_t::render(camera_t const *camera)
{
    mat4 u_model      = mat4{1.0f};
    mat4 u_view_3D    = camera->view();
    mat4 u_projection = camera->projection();

    // RENDER PRIMITIVES .......................................................

    glBindVertexArray(m_primitives.VAO);

    pr_shader.use();
    pr_shader.uniform_mat4("u_model", glm::value_ptr(u_model));
    pr_shader.uniform_mat4("u_view", glm::value_ptr(u_view_3D));
    pr_shader.uniform_mat4("u_projection", glm::value_ptr(u_projection));

    m_draw_call_points.execute();
    m_draw_call_lines.execute();

    glBindVertexArray(0);

    // RENDER FACES ............................................................

    glBindVertexArray(m_color_faces.VAO);

    flat_shader.use();
    flat_shader.uniform_mat4("u_model", glm::value_ptr(u_model));
    flat_shader.uniform_mat4("u_view", glm::value_ptr(u_view_3D));
    flat_shader.uniform_mat4("u_projection", glm::value_ptr(u_projection));

    // point_lights
    for (std::size_t i = 0; i < m_point_lights.size(); ++i) {
        std::string number = std::to_string(i);
        std::string u_point_light_position = "u_point_lights[" + number + "].position";
        std::string u_point_light_ambient  = "u_point_lights[" + number + "].ambient";
        std::string u_point_light_diffuse  = "u_point_lights[" + number + "].diffuse";
        std::string u_point_light_specular = "u_point_lights[" + number + "].specular";

        std::string u_point_light_constant  = "u_point_lights[" + number + "].constant";
        std::string u_point_light_linear = "u_point_lights[" + number + "].linear";
        std::string u_point_light_quadratic = "u_point_lights[" + number + "].quadratic";

        rr_point_light_t const &rr_point_light = m_point_lights[i];

        vec3 u_light_pos_worldspace = rr_point_light.position;
        vec3 u_light_ambient = color_t::to_vec3(rr_point_light.ambient);
        vec3 u_light_diffuse = color_t::to_vec3(rr_point_light.diffuse);
        vec3 u_light_specular = color_t::to_vec3(rr_point_light.specular);

        flat_shader.uniform_vec3(u_point_light_position.c_str(), glm::value_ptr(u_light_pos_worldspace));
        flat_shader.uniform_vec3(u_point_light_ambient.c_str(), glm::value_ptr(u_light_ambient));
        flat_shader.uniform_vec3(u_point_light_diffuse.c_str(), glm::value_ptr(u_light_diffuse));
        flat_shader.uniform_vec3(u_point_light_specular.c_str(), glm::value_ptr(u_light_specular));

        f32 u_light_constant = 1.0f;
        f32 u_light_linear = 0.7f;
        f32 u_light_quadratic = 1.8f;

        flat_shader.uniform_float(u_point_light_constant.c_str(), &u_light_constant);
        flat_shader.uniform_float(u_point_light_linear.c_str(), &u_light_linear);
        flat_shader.uniform_float(u_point_light_quadratic.c_str(), &u_light_quadratic);
    }

    u32 u_nr_point_lights = m_point_lights.size();
    flat_shader.uniform_int("u_nr_point_lights", u_nr_point_lights);

    u32 u_nr_directional_lights = m_directional_lights.size();
    flat_shader.uniform_int("u_nr_directional_lights", u_nr_directional_lights);

    // directional_lights
    for (std::size_t i = 0; i < m_directional_lights.size(); ++i) {
        std::string number = std::to_string(i);
        std::string u_directional_light_direction = "u_directional_lights[" + number + "].direction";
        std::string u_directional_light_ambient  = "u_directional_lights[" + number + "].ambient";
        std::string u_directional_light_diffuse  = "u_directional_lights[" + number + "].diffuse";
        std::string u_directional_light_specular = "u_directional_lights[" + number + "].specular";

        rr_directional_light_t const &rr_directional_light = m_directional_lights[i];

        vec3 u_light_dir_worldspace = rr_directional_light.direction;
        vec3 u_light_ambient = color_t::to_vec3(rr_directional_light.ambient);
        vec3 u_light_diffuse = color_t::to_vec3(rr_directional_light.diffuse);
        vec3 u_light_specular = color_t::to_vec3(rr_directional_light.specular);

        flat_shader.uniform_vec3(u_directional_light_direction.c_str(), glm::value_ptr(u_light_dir_worldspace));
        flat_shader.uniform_vec3(u_directional_light_ambient.c_str(), glm::value_ptr(u_light_ambient));
        flat_shader.uniform_vec3(u_directional_light_diffuse.c_str(), glm::value_ptr(u_light_diffuse));
        flat_shader.uniform_vec3(u_directional_light_specular.c_str(), glm::value_ptr(u_light_specular));
    }

    vec3 cam_pos = camera->get_position();
    flat_shader.uniform_vec3("u_camera_pos_worldspace", glm::value_ptr(cam_pos));

    m_draw_call_color_faces.execute();

    glBindVertexArray(0);

    // RENDER COLOR MESHES .....................................................

    for (rr_color_mesh_t const &rr_color_mesh : m_color_meshes) {
        color_mesh_t const *const cmesh = get_color_mesh(rr_color_mesh.mesh);

        mat4 u_model      = mat4{1.0f};

        vec3 origin = rr_color_mesh.transform.origin;
        vec3 translation = rr_color_mesh.transform.translation;
        quat orientation = euler_to_quat(rr_color_mesh.transform.rotation);
        orientation = glm::normalize(orientation);

        u_model = glm::translate(u_model, origin);
        u_model = u_model * glm::mat4_cast(orientation);
        u_model = glm::translate(u_model, -origin + translation);

        u_model = glm::scale(u_model, rr_color_mesh.transform.scale);

        flat_shader.uniform_mat4("u_model", glm::value_ptr(u_model));
        glBindVertexArray(cmesh->gl_buffer.VAO);
        cmesh->draw_call.execute();
        glBindVertexArray(0);
    }

    // RENDER DEBUG SHAPES .....................................................

    glBindVertexArray(m_debug_shapes.VAO);

    pr_shader.use();
    pr_shader.uniform_mat4("u_model", glm::value_ptr(u_model));
    pr_shader.uniform_mat4("u_view", glm::value_ptr(u_view_3D));
    pr_shader.uniform_mat4("u_projection", glm::value_ptr(u_projection));

    m_draw_call_debug.execute();

    glBindVertexArray(0);

    gl_check_error();
}

/* .-----------------------.
 * |                       |
 * |          FBO          |
 * |                       |
 * '-----------------------'
 */

void fbo_t::create(ivec2 size)
{
    NOT_USED(size);

    color_0.width = settings.size.logic.x;
    color_0.height = settings.size.logic.y;

    // Create new FBO, texture and RBO
    glGenFramebuffers(1, &id);

    glGenTextures(1, &color_0.gl_id);
    glBindTexture(GL_TEXTURE_2D, color_0.gl_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_0.width, color_0.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, color_0.width, color_0.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach texture and RBO to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_0.gl_id, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fbo_t::destroy()
{
    // Detach texture and RBO from FBO
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1, &id);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &color_0.gl_id);
}

/* .------------------------.
 * |                        |
 * |        RENDERER        |
 * |                        |
 * '------------------------'
 */

renderer_t::renderer_t()
{
    create_buffers();

    // SCREEN QUAD .............................................................

    m_screen_buffer.init();

    glBindVertexArray(m_screen_buffer.VAO);

    f32 hw = 1.0f;
    f32 hh = 1.0f;

    vertex_PCT_t v0, v1, v2, v3;
    v0.position.x = -hw;
    v0.position.y = -hh;
    v0.texcoord.x = 0.0f;
    v0.texcoord.y = 0.0f;

    v1.position.x = hw;
    v1.position.y = -hh;
    v1.texcoord.x = 1.0f;
    v1.texcoord.y = 0.0f;

    v2.position.x = -hw;
    v2.position.y = hh;
    v2.texcoord.x = 0.0f;
    v2.texcoord.y = 1.0f;

    v3.position.x = hw;
    v3.position.y = hh;
    v3.texcoord.x = 1.0f;
    v3.texcoord.y = 1.0f;

    std::vector<vertex_PCT_t> vertices = {v0, v1, v2, v3};
    indices_t indices = indices_t {0, 1, 2, 2, 1, 3};

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screen_buffer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_screen_buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex_PCT_t), &vertices[0], GL_DYNAMIC_DRAW);

    GLsizei stride = sizeof(vertex_PCT_t);

    u32 base_offset     = 0;
    u32 offset_position = base_offset + offsetof(vertex_PCT_t, position);
    u32 offset_color    = base_offset + offsetof(vertex_PCT_t, color);
    u32 offset_texcoord = base_offset + offsetof(vertex_PCT_t, texcoord);

    GLuint loc_position = static_cast<GLuint>(glsl_loc_e::position);
    GLuint loc_color    = static_cast<GLuint>(glsl_loc_e::color);
    GLuint loc_texcoord = static_cast<GLuint>(glsl_loc_e::texcoord);

    glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_position));
    glEnableVertexAttribArray(loc_position);

    glVertexAttribPointer(loc_color, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_color));
    glEnableVertexAttribArray(loc_color);

    glVertexAttribPointer(loc_texcoord, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void *>(offset_texcoord));
    glEnableVertexAttribArray(loc_texcoord);

    glBindVertexArray(0);

    // SKYBOX ..................................................................
    {
        /* back
         * v2  v3
         * v0  v1
         */
        vec3 v0 {-1.0f, -1.0f, -1.0f};
        vec3 v1 { 1.0f, -1.0f, -1.0f};
        vec3 v2 {-1.0f,  1.0f, -1.0f};
        vec3 v3 { 1.0f,  1.0f, -1.0f};

        /* front
         * v6  v7
         * v4  v5
         */
        vec3 v4 {-1.0f, -1.0f,  1.0f};
        vec3 v5 { 1.0f, -1.0f,  1.0f};
        vec3 v6 {-1.0f,  1.0f,  1.0f};
        vec3 v7 { 1.0f,  1.0f,  1.0f};

        std::vector<vec3> vertices {
            v5, v4, v7, v6,  // front
            v0, v1, v2, v3,  // back
            v1, v5, v3, v7,  // right
            v4, v0, v6, v2,  // left
            v2, v3, v6, v7,  // top
            v4, v5, v0, v1   // bottom
        };

        std::vector<GLuint> indices;
        for (i32 i = 0; i < 24; i+=4) {
            indices.push_back(0 + i);
            indices.push_back(1 + i);
            indices.push_back(2 + i);
            indices.push_back(2 + i);
            indices.push_back(1 + i);
            indices.push_back(3 + i);
        }

        m_skybox.gl_buffer.init();

        glBindVertexArray(m_skybox.gl_buffer.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_skybox.gl_buffer.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_skybox.gl_buffer.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

        GLuint loc_position = static_cast<GLuint>(glsl_loc_e::position);

        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray(loc_position);

        glBindVertexArray(0);
    }
}

renderer_t::~renderer_t()
{
}

void renderer_t::default_gl_setup()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);  // turn off depth buffer
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);     // only back side (which front in ogp)

    glEnable(GL_BLEND);

    // for normal alpha (png)
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // premultipled
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(4.0f);
    glLineWidth(1.0f);

    gl_check_error();
}

void renderer_t::resize()
{
    create_buffers();
}

void renderer_t::create_buffers()
{
    m_fbo.create(settings.size.logic);
}

void renderer_t::set_skybox(char const *dirname)
{
    m_skybox.texture = load_cube_texture(dirname);
}

void renderer_t::begin_frame()
{
    gl_check_error();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo.id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    }
    else {
        ogp_log_error("Framebuffer %u: is not complete", m_fbo.id);
    }

    glViewport(0, 0, m_fbo.color_0.width, m_fbo.color_0.height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl_check_error();
}

void renderer_t::end_frame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, settings.size.window.x, settings.size.window.y);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_screen_buffer.VAO);

    // FX
    screen_shader.use();
    screen_shader.uniform_float("wave_time", &effects.wave.time);
    screen_shader.uniform_float("wave_amplifier", &effects.wave.amplifier);
    screen_shader.uniform_float("greyscale_amplifier", &effects.greyscale.amplifier);
    screen_shader.uniform_float("negative_amplifier", &effects.negative.amplifier);
    screen_shader.bind_texture("u_texture_diffuse_0", m_fbo.color_0.gl_id, 0, GL_TEXTURE_2D);

    // two triangles for screen quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    gl_check_error();
}

void renderer_t::render_skybox(camera_t const *camera)
{
    // remove any translation component of the view matrix
    mat4 u_view = mat4(mat3(camera->view()));
    mat4 u_projection = camera->projection();

    sky_shader.use();
    sky_shader.uniform_mat4("u_view", glm::value_ptr(u_view));
    sky_shader.uniform_mat4("u_projection", glm::value_ptr(u_projection));
    sky_shader.bind_texture("u_texture_skybox_0", m_skybox.texture.gl_id, 0, GL_TEXTURE_CUBE_MAP);

    glDepthMask(GL_FALSE);  // remember to turn depth writing off (why?)

    glBindVertexArray(m_skybox.gl_buffer.VAO);
    // 36 = 6 walls * 2 tri * 3 vert
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);

    gl_check_error();
}

void renderer_t::render(render_context_t const *rc, camera_t const *camera)
{
    NOT_USED(rc);
    NOT_USED(camera);
}

}  // namespace ogp
