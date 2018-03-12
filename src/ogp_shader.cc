#include "ogp_shader.h"

#include "ogp_sdl.h"
#include "ogp_utils.h"

#include <string.h>
#include <glm/gtc/type_ptr.hpp>

namespace
{

void shader_program_print_log(GLuint sprogram)
{
    char log[2048];
    ogp::i32 i;
    glGetProgramInfoLog(sprogram, 2048, &i, log);
    log[i] = '\0';

    if (strcmp(log, "") != 0) {
        ogp_log_error("Shader compile log: %s", log);
        std::terminate();
    }
}

GLuint compile_shader(char const *filename, const GLenum &type)
{
    GLuint shader    = glCreateShader(type);
    const char *data = ogp::file_read(filename);

    if (data == NULL) {
        ogp_log_error("Shader read error: %s", SDL_GetError());
        std::terminate();
    }

    ogp_log_debug("Shader compiling: %s", filename);
    glShaderSource(shader, 1, &data, NULL);
    free((void *)data);
    glCompileShader(shader);

    char log[2048];
    ogp::i32 logsize;
    glGetShaderInfoLog(shader, 2048, &logsize, log);
    log[logsize] = '\0';  // FIXME get rid of '\n'

    if (strcmp(log, "") != 0) {
        ogp_log_error("Shader compile error: %s", log);
        std::terminate();
    }

    ogp_log_info("Shader %u: %s", shader, filename);
    ogp::gl_check_error();
    return shader;
}

}  // namespace [noname]

namespace ogp
{

shader_t::shader_t(char const *vsfile, char const *fsfile)
{
    m_program = glCreateProgram();
    // gl_check_error();

    m_vertex = compile_shader(vsfile, GL_VERTEX_SHADER);
    glAttachShader(m_program, m_vertex);
    shader_program_print_log(m_program);

    m_fragment = compile_shader(fsfile, GL_FRAGMENT_SHADER);
    glAttachShader(m_program, m_fragment);
    shader_program_print_log(m_program);

    link();

    ogp_log_info("Shader program %u: vertex: %u, fragment: %u", m_program, m_vertex, m_fragment);
    gl_check_error();
}

void shader_t::link()
{
    glLinkProgram(m_program);
    shader_program_print_log(m_program);
}

void shader_t::use()
{
    glUseProgram(m_program);
    shader_program_print_log(m_program);
    // NOTE debug info
    // SDL_Log("shader program use: %u", m_shader.program);
}

void shader_t::destroy()
{
    ogp_log_debug("Shader program %u: destroy");

    glDeleteProgram(m_program);
    glDeleteShader(m_fragment);
    glDeleteShader(m_vertex);
}

void shader_t::uniform_int(char const *name, i32 val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no i32 uniform named %s", m_program, name);
    } else {
        glUniform1i(location, val);
    }
    gl_check_error();
}

void shader_t::uniform_float(char const *name, f32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no float uniform named %s", m_program, name);
    } else {
        glUniform1fv(location, 1, val);
    }
    gl_check_error();
}

void shader_t::uniform_ivec2(char const *name, i32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no ivec2 uniform named %s", m_program, name);
    } else {
        glUniform2iv(location, 1, val);
    }
}

void shader_t::uniform_vec2(char const *name, f32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no vec2 uniform named %s", m_program, name);
    } else {
        glUniform2fv(location, 1, val);
    }
}

void shader_t::uniform_vec3(char const *name, f32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no vec3 uniform named %s", m_program, name);
    } else {
        glUniform3fv(location, 1, val);
    }
}

void shader_t::uniform_vec4(char const *name, f32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no vec4 uniform named %s", m_program, name);
    } else {
        glUniform4fv(location, 1, val);
    }
}

void shader_t::uniform_mat4(char const *name, f32 *val)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no mat4 uniform named %s", m_program,  name);
    } else {
        glUniformMatrix4fv(location, 1, GL_FALSE, val);
    }
}

void shader_t::bind_texture(char const *name, GLuint texture, i32 index, GLenum target)
{
    GLint location = location_uniform(name);
    if (location == -1) {
        ogp_log_warning("Shader %u has no texture uniform named %s", m_program, name);
    } else {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(target, texture);
        glUniform1i(location, index); // 0 base, 2 normal, 4 shadow
    }
}

GLint shader_t::location_attrib(char const *name)
{
    return glGetAttribLocation(m_program, name);
}

GLint shader_t::location_uniform(char const *name)
{
    return glGetUniformLocation(m_program, name);
}

}  // namespace ogp
