#ifndef OGP_MESH_H
#define OGP_MESH_H

#include "ogp_defines.h"
#include "ogp_opengl.h"

#include <vector>

namespace ogp
{

struct mesh_t : public index_holder_t<mesh_t> { };

struct face_t
{
    u32 v0;
    u32 v1;
    u32 v2;
};

constexpr std::array<vec3, 36> OGP_FLAT_CUBE_VERTICES = {
    vec3 {-1.0f, -1.0f,  1.0f}, vec3 { 1.0f, -1.0f,  1.0f}, vec3 {-1.0f,  1.0f,  1.0f},  // front   A =  0,  1,  2
    vec3 {-1.0f,  1.0f,  1.0f}, vec3 { 1.0f, -1.0f,  1.0f}, vec3 { 1.0f,  1.0f,  1.0f},  // front   B =  3,  4,  5

    vec3 {-1.0f, -1.0f, -1.0f}, vec3 {-1.0f, -1.0f,  1.0f}, vec3 {-1.0f,  1.0f, -1.0f},  // left    A =  6,  7,  8
    vec3 {-1.0f,  1.0f, -1.0f}, vec3 {-1.0f, -1.0f,  1.0f}, vec3 {-1.0f,  1.0f,  1.0f},  // left    B =  9, 10, 11

    vec3 { 1.0f, -1.0f,  1.0f}, vec3 { 1.0f, -1.0f, -1.0f}, vec3 { 1.0f,  1.0f,  1.0f},  // right   A =  12, 13, 14
    vec3 { 1.0f,  1.0f,  1.0f}, vec3 { 1.0f, -1.0f, -1.0f}, vec3 { 1.0f,  1.0f, -1.0f},  // right   B =  15, 16, 17

    vec3 { 1.0f, -1.0f, -1.0f}, vec3 {-1.0f, -1.0f, -1.0f}, vec3 { 1.0f,  1.0f, -1.0f},  // back    A =  18, 19, 20
    vec3 { 1.0f,  1.0f, -1.0f}, vec3 {-1.0f, -1.0f, -1.0f}, vec3 {-1.0f,  1.0f, -1.0f},  // back    B =  21, 22, 23

    vec3 {-1.0f,  1.0f,  1.0f}, vec3 { 1.0f,  1.0f,  1.0f}, vec3 {-1.0f,  1.0f, -1.0f},  // top     A =  24, 25, 26
    vec3 {-1.0f,  1.0f, -1.0f}, vec3 { 1.0f,  1.0f,  1.0f}, vec3 { 1.0f,  1.0f, -1.0f},  // top     B =  27, 28, 29

    vec3 {-1.0f, -1.0f, -1.0f}, vec3 { 1.0f, -1.0f, -1.0f}, vec3 {-1.0f, -1.0f,  1.0f},  // bottom  A =  30, 31, 32
    vec3 {-1.0f, -1.0f,  1.0f}, vec3 { 1.0f, -1.0f, -1.0f}, vec3 { 1.0f, -1.0f,  1.0f},  // bottom  B =  33, 34, 35
};

constexpr std::array<face_t, 12> OGP_FLAT_CUBE_FACES = {
    face_t { 0,  1,  2}, face_t { 3,  4,  5},  //  front A B
    face_t { 6,  7,  8}, face_t { 9, 10, 11},  //   left A B
    face_t {12, 13, 14}, face_t {15, 16, 17},  //  right A B
    face_t {18, 19, 20}, face_t {21, 22, 23},  //   back A B
    face_t {24, 25, 26}, face_t {27, 28, 29},  //    top A B
    face_t {30, 31, 32}, face_t {33, 34, 35},  // bottom A B
};

using faces_t = std::vector<face_t>;

struct color_mesh_t
{
    vertices_PNC_t vertices;
    faces_t faces;
    gl_buffer_t gl_buffer;
    draw_call_t draw_call;

    void recalculate_normals();

    void update_vbo();
};

color_mesh_t *get_color_mesh(mesh_t mesh);

mesh_t create_cube(f32 edge = 1.0f);

mesh_t load_mesh(char const *filename);

}  // namespace ogp

#endif  // OGP_MESH_H
