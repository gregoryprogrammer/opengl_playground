#ifndef OGP_OPENGL_H
#define OGP_OPENGL_H

#include "ogp_defines.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

namespace ogp
{

struct color_t
{
    u8 r {0x00};
    u8 g {0x00};
    u8 b {0x00};
    u8 a {0x00};

    color_t(u8 r, u8 g, u8 b, u8 a = 255);

    static color_t from_float(f32 r, f32 g, f32 b, f32 a);

    static vec3 to_vec3(color_t color);

    operator int() { return 0; }
};

const color_t OGP_COLOR_RASPBERRY = color_t {227, 11, 93};
const color_t OGP_COLOR_WHITE = color_t {255, 255, 255};

struct vertex_PCT_t
{
    vec3 position {0.0f, 0.0f, 0.0f};
    vec4 color {1.0f, 1.0f, 1.0f, 1.0f};
    vec2 texcoord {0.0f, 0.0f};
};

// for primitives
struct vertex_PC_t
{
    vec3 position {0.0f, 0.0f, 0.0f};
    color_t color {0x00, 0x00, 0x00, 0xFF};
};

// for faces
struct vertex_PNC_t
{
    vec3 position {0.0f, 0.0f, 0.0f};
    vec3 normal {0.0f, 0.0f, 0.0f};
    color_t color {0x00, 0x00, 0x00, 0xFF};
};

using indices_t = std::vector<GLuint>;
using vertices_PC_t = std::vector<vertex_PC_t>;
using vertices_PNC_t = std::vector<vertex_PNC_t>;

enum class glsl_loc_e : GLuint
{
    position  = 0,
    color     = 1,
    texcoord  = 2,
    normal    = 3,
    tangent   = 4,
    bitangent = 5,
};

struct texture_t
{
    GLuint gl_id;
    i32 width;
    i32 height;
    GLenum target {GL_TEXTURE_2D};
};

// TODO gl_buffer_t(gl_buffer_t const &) = delete
// TODO gl_buffer_t(gl_buffer_t &&) = default;
class gl_buffer_t
{
    bool m_inited {false};

public:
    GLuint VAO {0};
    GLuint VBO {0};
    GLuint EBO {0};

    gl_buffer_t() = default;

    ~gl_buffer_t();

    void init();

    void cleanup();
};

struct draw_call_t
{
    GLenum mode {GL_POINTS};
    GLuint num_indices {0};
    GLuint offset {0};

    void execute() const;
};

GLenum gl_check_error_(const char *file, int line);
#define gl_check_error() gl_check_error_(__LOCALFILENAME__, __LINE__)

void print_opengl_info();

void window_resize(i32 width, i32 height);

void viewport(i32 width, i32 height, f32 ratio);

void set_cursor(char const *path);

void gl_setup_texture_filtering();

texture_t load_cube_texture(char const *path);

}  // namespace ogp

#endif  // OGP_OPENGL_H
