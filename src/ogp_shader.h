#ifndef OGP_SHADER_H
#define OGP_SHADER_H

#include "ogp_defines.h"
#include "ogp_opengl.h"

namespace ogp
{

class shader_t
{
    GLuint m_program;
    GLuint m_vertex;
    GLuint m_fragment;

public:
    shader_t(char const *vsfile, char const *fsfile);

    void link();

    void use();

    void destroy();

    void uniform_int(char const *name, i32 val);

    void uniform_ivec2(char const *name, i32 *val);

    void uniform_vec2(char const *name, f32 *val);

    void uniform_vec3(char const *name, f32 *val);

    void uniform_vec4(char const *name, f32 *val);

    void uniform_mat4(char const *name, f32 *val);

    void uniform_float(char const *name, f32 *val);

    void bind_texture(char const *name, GLuint texture, i32 index, GLenum target);

    GLint location_attrib(char const *name);

    GLint location_uniform(char const *name);
};

}  // namespace ogp

#endif  // OGP_SHADER_H
