#ifndef OGP_RENDER_H
#define OGP_RENDER_H

#include "ogp_array.h"
#include "ogp_camera.h"
#include "ogp_defines.h"
#include "ogp_mesh.h"
#include "ogp_opengl.h"
#include "ogp_shader.h"

#include <vector>

namespace ogp
{

struct transform_t
{
    vec3 translation {0.0f, 0.0f, 0.0f};
    vec3 rotation {0.0f, 0.0f, 0.0f};
    vec3 origin {0.0f, 0.0f, 0.0f};
    vec3 scale {1.0f, 1.0f, 1.0f};
};

/* .-------------------------------.
 * |                               |
 * |        RENDER REQUESTS        |
 * |                               |
 * '-------------------------------'
 */

struct rr_point_t
{
    vertex_PC_t v;
    transform_t transform;
};

struct rr_line_t
{
    vertex_PC_t v0;
    vertex_PC_t v1;
    transform_t transform;
};

struct rr_color_face_t
{
    vertex_PC_t v0;
    vertex_PC_t v1;
    vertex_PC_t v2;
    transform_t transform;
};

struct rr_color_mesh_t
{
    mesh_t mesh;
    transform_t transform;
};

struct rr_debug_shape_t
{
    vec3 position {0.0f, 0.0f, 0.0f};
    color_t color {0xFF, 0xFF, 0xFF, 0xFF};
};

struct rr_point_light_t
{
    vec3 position {0.0f, 0.0f, 0.0f};
    color_t ambient {0xFF, 0xFF, 0xFF, 0xFF};
    color_t diffuse {0xFF, 0xFF, 0xFF, 0xFF};
    color_t specular {0xFF, 0xFF, 0xFF, 0xFF};
};

struct rr_directional_light_t
{
    vec3 direction {0.0f, 0.0f, 0.0f};
    color_t ambient {0xFF, 0xFF, 0xFF, 0xFF};
    color_t diffuse {0xFF, 0xFF, 0xFF, 0xFF};
    color_t specular {0xFF, 0xFF, 0xFF, 0xFF};
};

struct r_requests_t
{
    array_t<rr_point_t, 1000> points;
    array_t<rr_line_t, 2048> lines;
    array_t<rr_color_face_t, 1024> color_faces;
    array_t<rr_debug_shape_t, 1024> debug_shapes;
    array_t<rr_point_light_t, 128> point_lights;
    array_t<rr_directional_light_t, 128> directional_lights;
    array_t<rr_color_mesh_t, 64> color_meshes;
};

/* .-----------------------------.
 * |                             |
 * |        RENDER BUFFER        |
 * |                             |
 * '-----------------------------'
 */

class render_buffer_t
{
    gl_buffer_t m_primitives;
    gl_buffer_t m_color_faces;
    gl_buffer_t m_debug_shapes;

    draw_call_t m_draw_call_points;
    draw_call_t m_draw_call_lines;
    draw_call_t m_draw_call_color_faces;
    draw_call_t m_draw_call_debug;

    shader_t pr_shader {"assets/shaders/primitives.vert", "assets/shaders/primitives.frag"};
    shader_t flat_shader {"assets/shaders/flat.vert", "assets/shaders/flat.frag"};

    std::vector<rr_point_light_t> m_point_lights;
    std::vector<rr_directional_light_t> m_directional_lights;
    std::vector<rr_color_mesh_t> m_color_meshes;

public:

    render_buffer_t() = default;

    ~render_buffer_t() = default;

    void create_batch(r_requests_t const *r_requests);

    void update_vbo(r_requests_t const *r_requests);

    void render(camera_t const *camera);
};

/* .------------------------------.
 * |                              |
 * |        RENDER CONTEXT        |
 * |                              |
 * '------------------------------'
 */

class render_context_t
{
    render_buffer_t m_render_buffer;

    r_requests_t m_requests;

    color_t m_line_color {0xFF, 0xFF, 0xFF, 0xFF};

    color_t m_fill_color {0xFF, 0xFF, 0xFF, 0xFF};

    transform_t m_transform;

public:

    render_context_t ();

    r_requests_t const &requests_ref() const { return m_requests; }

    void reset();

    void update_vbo();

    void render(camera_t const *camera);

    color_t set_line_color(color_t color);

    color_t set_line_color(u8 r, u8 g, u8 b, u8 a = 0xFF);

    color_t set_fill_color(color_t color);

    color_t set_fill_color(u8 r, u8 g, u8 b, u8 a = 0xFF);

    // TRANSFORM ...............................................................

    vec3 set_translation(vec3 translation);

    vec3 set_rotation(vec3 rotation);

    vec3 set_origin(vec3 origin);

    // PRIMITIVES ..............................................................

    void draw_point(vec3 position);

    void draw_line(vec3 begin, vec3 end);

    void draw_color_face(vec3 p0, vec3 p1, vec3 p2);

    void draw_color_quad(vec3 p0, vec3 p1, vec3 p2, vec3 p3);

    void draw_cube();

    // MEHES ...................................................................

    void draw_color_mesh(mesh_t mesh, transform_t transform = {});

    // LIGHTS ..................................................................

    void add_point_light(vec3 position, color_t color);

    void add_directional_light(vec3 direction, color_t color);

};
/* .-----------------------.
 * |                       |
 * |          FBO          |
 * |                       |
 * '-----------------------'
 */

struct fbo_t
{
    GLuint id;
    GLuint rbo;
    texture_t color_0;

    void create(ivec2 size);
    void destroy();
};

/* .------------------------.
 * |                        |
 * |        RENDERER        |
 * |                        |
 * '------------------------'
 */

struct skybox_t
{
    gl_buffer_t gl_buffer;
    texture_t texture;
};

class renderer_t
{
    gl_buffer_t m_screen_buffer;
    fbo_t m_fbo;

    skybox_t m_skybox;

    shader_t sky_shader {"assets/shaders/skybox.vert", "assets/shaders/skybox.frag"};
    shader_t screen_shader {"assets/shaders/screen.vert", "assets/shaders/screen.frag"};

    void create_buffers();

public:
    renderer_t();

    ~renderer_t();

    void default_gl_setup();

    void resize();

    void set_skybox(char const *dirname);

    void begin_frame();

    void end_frame();

    void render_skybox(camera_t const *camera);

    void render(render_context_t const *rc, camera_t const *camera);
};

}  // namespace ogp

#endif  // OGP_RENDER_H
