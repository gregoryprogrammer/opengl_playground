#include "ogp_mesh.h"

#include "ogp_array.h"
#include "ogp_utils.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <map>

namespace ogp
{

array_t<color_mesh_t, 64> meshes;

void color_mesh_t::recalculate_normals()
{
    for (face_t const &face : faces) {
        vec3 p0 = vertices[face.v0].position;
        vec3 p1 = vertices[face.v1].position;
        vec3 p2 = vertices[face.v2].position;

        vec3 a = p1 - p0;
        vec3 b = p2 - p0;
        vec3 normal = glm::normalize(glm::cross(a, b));

        vertices[face.v0].normal = normal;
        vertices[face.v1].normal = normal;
        vertices[face.v2].normal = normal;
    }
}

void color_mesh_t::update_vbo()
{
    indices_t indices {};
    for (face_t const &face : faces) {
        indices.push_back(face.v0);
        indices.push_back(face.v1);
        indices.push_back(face.v2);
    }

    size_t data_size  = sizeof(vertex_PNC_t) * vertices.size();

    glBindVertexArray(gl_buffer.VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buffer.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, gl_buffer.VBO);
    glBufferData(GL_ARRAY_BUFFER, data_size, &vertices[0], GL_DYNAMIC_DRAW);

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

    draw_call.mode = GL_TRIANGLES;
    draw_call.num_indices = indices.size();
    draw_call.offset = 0;
}

color_mesh_t *get_color_mesh(mesh_t mesh)
{
    return meshes.get(mesh.index);
}

mesh_t create_cube(f32 edge)
{
    color_mesh_t cmesh {};

    faces_t faces;
    color_t color {OGP_COLOR_RASPBERRY};
    f32 half_edge = edge * 0.5f;

    for (vec3 const v : OGP_FLAT_CUBE_VERTICES) {
        vertex_PNC_t vertex {v * half_edge, {1.0f, 0.0f, 1.0f}, color};
        cmesh.vertices.push_back(vertex);
    }

    for (face_t const f : OGP_FLAT_CUBE_FACES) {
        cmesh.faces.push_back(f);
    }

    cmesh.gl_buffer.init();
    cmesh.recalculate_normals();
    cmesh.update_vbo();

    mesh_t mesh {};
    mesh.index = meshes.add(cmesh);
    return mesh;
}

mesh_t load_mesh(char const *filename)
{
    ogp_log_info("Loading mesh from: %s", filename);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);

    if (!err.empty()) { // `err` may contain warning message.
        ogp_log_error("%s", err.c_str());
    }

    if (!ret) {
        terminate("error loading mesh");
    }

    color_mesh_t cmesh {};

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            i32 fv = shapes[s].mesh.num_face_vertices[f];

            // ogp_log_me("face with %d vertices", fv);
            assert(fv == 3);

            // Loop over vertices in the face.
            for (i32 v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                // ogp_log_me("vertex");
                f32 vx = attrib.vertices[3 * idx.vertex_index + 0];
                f32 vy = attrib.vertices[3 * idx.vertex_index + 1];
                f32 vz = attrib.vertices[3 * idx.vertex_index + 2];
                // float nx = attrib.normals[3 * idx.normal_index + 0];
                // float ny = attrib.normals[3 * idx.normal_index + 1];
                // float nz = attrib.normals[3 * idx.normal_index + 2];
                // float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                // float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                // ogp_log_me("vertex end");

                vertex_PNC_t vertex {};
                vertex.position = vec3 {vx, vy, vz};
                vertex.color = OGP_COLOR_WHITE;

                cmesh.vertices.push_back(vertex);
            }

            face_t face = face_t {};
            face.v0 = index_offset + 0;
            face.v1 = index_offset + 1;
            face.v2 = index_offset + 2;
            cmesh.faces.push_back(face);

            index_offset += fv;

            // per-face material
            // shapes[s].mesh.material_ids[f];
        }

        // material for the first face
/*
 *         i32  material_id = shapes[0].mesh.material_ids[0];
 *         log_me("material_id = %d", material_id);
 * 
 *         tinyobj::material_t material = materials[material_id];
 * 
 *         char const *diffuse_texname = material.diffuse_texname.c_str();
 *         log_me("tex = %s", diffuse_texname);
 * 
 * 
 *         r_mesh.material.texture = load_texture(diffuse_texname);
 * 
 */

    }

    cmesh.gl_buffer.init();
    cmesh.recalculate_normals();
    cmesh.update_vbo();

    mesh_t mesh {};
    mesh.index = meshes.add(cmesh);
    return mesh;
}

}  // namespace ogp
