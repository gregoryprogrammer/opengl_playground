#include "ogp_opengl.h"

#include "ogp_settings.h"
#include "ogp_utils.h"

#include <map>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace ogp
{

static std::map<char const *, texture_t, cmp_str> textures;

gl_buffer_t::~gl_buffer_t()
{
    if (m_inited == 1) {
        ogp_log_warning("gl buffer leak, you should cleanup!");
    }
}

void gl_buffer_t::init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    m_inited = 1;
}

void gl_buffer_t::cleanup()
{
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    m_inited = 0;
}

void draw_call_t::execute() const
{
    GLuint byte_offset = offset * sizeof((static_cast<draw_call_t *>(0))->offset);
    glDrawElements(mode, num_indices, GL_UNSIGNED_INT, reinterpret_cast<void *>(byte_offset));
    gl_check_error();
}

GLenum gl_check_error_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        char const *error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        ogp_log_error("%s: %s:%d", error, file, line);
        std::terminate();
    }
    return errorCode;
}

void print_opengl_info()
{
    ogp_log_system("GL_VERSION: %s", glGetString(GL_VERSION));
    ogp_log_system("GL_RENDERER: %s", glGetString(GL_RENDERER));
    ogp_log_system("GL_VENDOR: %s", glGetString(GL_VENDOR));
    ogp_log_system("GL_SHADING_LANGUAGE_VERSION: %s",
               glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint max_texture_size            = -1;
    GLint max_vertex_attribs          = -1;
    GLint max_uniform_buffer_bindings = -1;
    GLint max_vertex_uniform_blocks   = -1;
    GLint max_geometry_uniform_blocks = -1;
    GLint max_fragment_uniform_blocks = -1;
    GLint max_renderbuffer_size       = -1;

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_vertex_uniform_blocks);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &max_geometry_uniform_blocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &max_fragment_uniform_blocks);

    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);

    ogp_log_debug("Maximum size of a texture: %dx%d", max_texture_size, max_texture_size);
    ogp_log_debug("Maximum size of renderbuffer: %d", max_renderbuffer_size);

    ogp_log_debug("Maximum nr of vertex attributes supported: %d",
              max_vertex_attribs);
    ogp_log_debug("Maximum nr of uniform bindings supported: %d",
             max_uniform_buffer_bindings);
    ogp_log_debug("Maximum nr of vertex uniform blocks supported: %d",
             max_vertex_uniform_blocks);
    ogp_log_debug("Maximum nr of geometry uniform blocks supported: %d",
             max_geometry_uniform_blocks);
    ogp_log_debug("Maximum nr of fragment uniform blocks supported: %d",
             max_fragment_uniform_blocks);
}

void window_resize(i32 width, i32 height)
{
    settings.size.window.x = static_cast<f32>(width);
    settings.size.window.y = static_cast<f32>(height);

    auto window_ratio = settings.window_ratio();

    f32 frame_width = 0.0f;
    f32 frame_height = 0.0f;

    if (window_ratio > settings.game_ratio()) {
        frame_height = settings.size.game.y;
        frame_width = frame_height * window_ratio;
    } else {
        frame_width = settings.size.game.x;
        frame_height = frame_width * (1.0f / window_ratio);
    }

    settings.size.frame.x = frame_width;
    settings.size.frame.y = frame_height;

    viewport(width, height, window_ratio);
    print_size_info();
}

void viewport(i32 width, i32 height, f32 ratio)
{
    i32 vwidth  = width;
    i32 vheight = static_cast<i32>((vwidth * (1.0f / ratio)));

    if (vheight > height) {
        vheight = height;
        vwidth  = static_cast<i32>((vheight * ratio));
    }

    i32 vx = static_cast<i32>((width - vwidth) * 0.5f);
    i32 vy = static_cast<i32>((height - vheight) * 0.5f);

    ogp_log_info("glViewport(%d, %d, %d, %d);", vx, vy, vwidth, vheight);

    glViewport(vx, vy, vwidth, vheight); // FIXME
}

void set_cursor(char const *path)
{
    i32 w, h, comp;
    stbi_set_flip_vertically_on_load(false);
    u8 *data = stbi_load(path, &w, &h, &comp, STBI_rgb_alpha);

    if (data == nullptr) {
        ogp_log_error("Cursor data is NULL");
        std::terminate();
    }

    u32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, w, h, 32, w * sizeof(u32), rmask, gmask, bmask, amask);

    SDL_Cursor *cursor = SDL_CreateColorCursor(surface, 0, 0);
    SDL_SetCursor(cursor);

    stbi_image_free(data);
}

/*
 * vec3 pixel_perfect_0375(vec3 position)
 * {
 *     constexpr f32 magic = 0.375f;
 * 
 *     i32 x = meters_to_pixels(position.x);
 *     i32 y = meters_to_pixels(position.y);
 * 
 *     f32 ppx = pixels_to_meters((f32)x + magic);
 *     f32 ppy = pixels_to_meters((f32)y + magic);
 * 
 *     position.x = ppx;
 *     position.y = ppy;
 * 
 *     return position;
 * }
 * 
 */

void gl_setup_texture_filtering()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

texture_t load_cube_texture(char const *path)
{
    auto tex = textures.find(path);
    if (tex != textures.end()) {
        return tex->second;
    }

    char p_right [256];
    char p_left [256];
    char p_top [256];
    char p_bottom [256];
    char p_back [256];
    char p_front [256];

    snprintf(p_right, sizeof p_right, "%s/right.png", path);
    snprintf(p_left, sizeof p_left, "%s/left.png", path);
    snprintf(p_top, sizeof p_top, "%s/top.png", path);
    snprintf(p_bottom, sizeof p_bottom, "%s/bottom.png", path);
    snprintf(p_back, sizeof p_back, "%s/back.png", path);
    snprintf(p_front, sizeof p_front, "%s/front.png", path);

    std::vector<char const *> faces {
            p_right,
            p_left,
            p_top,
            p_bottom,
            p_back,
            p_front,
    };

    GLuint texture_id;
    i32 w, h, comp;
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    stbi_set_flip_vertically_on_load(false);

/*
 *             "/right.png",
 *             "/left.png",
 *             "/top.png",
 *             "/bottom.png",
 *             "/back.png",
 *             "/front.png",
 *
 */

    // front
    {
        unsigned char *image = stbi_load(faces[5], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[5]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("texture_t cube %d: %s, (%d, %d) ", texture_id, faces[5], w, h);
    }

    // back
    {
        unsigned char *image = stbi_load(faces[4], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[4]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("texture_t cube %d: %s, (%d, %d) ", texture_id, faces[4], w, h);
    }

    // right
    {
        unsigned char *image = stbi_load(faces[0], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[0]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("texture_t cube %d: %s, (%d, %d) ", texture_id, faces[0], w, h);
    }

    // left
    {
        unsigned char *image = stbi_load(faces[1], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[1]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("texture_t cube %d: %s, (%d, %d) ", texture_id, faces[1], w, h);
    }

    // top
    {
        unsigned char *image = stbi_load(faces[2], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[2]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("texture_t cube %d: %s, (%d, %d) ", texture_id, faces[2], w, h);
    }

    // bottom
    {
        unsigned char *image = stbi_load(faces[3], &w, &h, &comp, STBI_rgb);

        if (image == nullptr) {
            ogp_log_error("Texture: null image: %s", faces[3]);
            std::terminate();
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        ogp_log_info("Texture cube %d: %s, (%d, %d) ", texture_id, faces[3], w, h);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    texture_t texture = texture_t{};
    texture.gl_id     = texture_id;
    texture.width     = w;
    texture.height    = h;
    texture.target    = GL_TEXTURE_CUBE_MAP;

    return texture;
}

}  // namespace ogp
