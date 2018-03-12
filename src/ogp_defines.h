#ifndef OGP_DEFINES_H
#define OGP_DEFINES_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <set>
#include <unordered_set>

namespace ogp
{

using i32 = Sint32;
using i64 = Sint64;
using u8 = Uint8;
using u32 = Uint32;
using u64 = Uint64;
using f32 = float;

using vec2 = glm::vec2;
using ivec2 = glm::ivec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat3 = glm::mat3x3;
using mat4 = glm::mat4x4;
using quat = glm::quat;

// CAMERA ......................................................................

constexpr f32 OGP_CAMERA_PITCH_MIN = -89.0f;
constexpr f32 OGP_CAMERA_PITCH_MAX = 89.0f;
constexpr f32 OGP_CAMERA_YAW_MIN   = -180.0f;
constexpr f32 OGP_CAMERA_YAW_MAX   = 180.0f;
constexpr f32 OGP_CAMERA_ROLL_MIN  = 0.0f;
constexpr f32 OGP_CAMERA_ROLL_MAX  = 360.0f;
constexpr f32 OGP_ORTHO_Z_NEAR     = -1.0f;
constexpr f32 OGP_ORTHO_Z_FAR      = 1.0f;
constexpr f32 OGP_PERSPECTIVE_NEAR = 0.01f;
constexpr f32 OGP_PERSPECTIVE_FAR  = 100.0f;

// PHYSICS .....................................................................

constexpr i32 OGP_PHYSICS_NUM_PARTICLES      = 1024;
constexpr i32 OGP_PHYSICS_NUM_BODIES         = 128;
constexpr i32 OGP_PHYSICS_NUM_CONSTRAINTS    = 1024;
constexpr i32 OGP_PHYSICS_NUM_PINS           = 128;
constexpr i32 OGP_PHYSICS_NUM_USER_BODIES    = 128;

// MODELS ......................................................................

constexpr std::array<vec3, 8> OGP_CUBE_VERTICES = {
    vec3 {-1.0f, -1.0f,  1.0f},
    vec3 { 1.0f, -1.0f,  1.0f},
    vec3 {-1.0f, -1.0f, -1.0f},
    vec3 { 1.0f, -1.0f, -1.0f},
    vec3 {-1.0f,  1.0f,  1.0f},
    vec3 { 1.0f,  1.0f,  1.0f},
    vec3 {-1.0f,  1.0f, -1.0f},
    vec3 { 1.0f,  1.0f, -1.0f},
};

constexpr std::array<u32, 36> OGP_CUBE_INDICES = {
    0, 1, 4, 4, 1, 5,  // front
    2, 0, 6, 6, 0, 4,  // left
    1, 3, 5, 5, 3, 7,  // right
    3, 2, 7, 7, 2, 6,  // back
    4, 5, 6, 6, 5, 7,  // top
    2, 3, 0, 0, 3, 1,  // bottom
};

using index_value_t = i64;

struct index_t
{
    index_value_t value {-1};
    i64 version {-1};

    static index_t invalid()
    {
        index_t invalid;
        invalid.value = -1;
        invalid.version = -1;
        return invalid;
    }

    bool operator== (index_t const &other) const
    {
        return (other.value == value) && (other.version == version);
    }

    struct hash
    {
        std::size_t operator()(index_t const &index) const
        {
            return index.value;  // it is uniq already
        }
    };
};

template <typename T>
struct index_holder_t
{
    index_t index;
};

template <typename T>
bool operator== (index_holder_t<T> const &lhs, index_holder_t<T> const &rhs)
{
    return (lhs.index.value == rhs.index.value) && (lhs.index.version == rhs.index.version);
}

using unique_indices_t = std::unordered_set<index_t, index_t::hash>;

}  // namespace ogp

#endif  // OGP_DEFINES_H
