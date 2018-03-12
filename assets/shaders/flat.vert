#version 330 core

#define POSITION 0
#define COLOR 1
#define NORMAL 3

layout (location = POSITION) in vec3 a_position;
layout (location = COLOR) in vec4 a_color;
layout (location = NORMAL) in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

flat out vec4 v_color;
out vec3 v_normal_modelspace;

out vec3 v_position_worldspace;
flat out vec3 v_normal_cameraspace;

void main()
{
    vec3 position = a_position;

    // convert position from modelspace to clipspace
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);

    // position of the vertex in worldspace
    v_position_worldspace = (u_model * vec4(position, 1.0)).xyz;

    // vector that goes from the vertex to the camera, in camera space.
    // in camera space, the camera is at the orign (0, 0, 0)
    // v_position_cameraspace = (u_view * u_model * vec4(position, 1.0)).xyz;

    // normal of the vertex in camera space
    v_normal_cameraspace = (u_view * u_model * vec4(a_normal, 0.0)).xyz;

    mat4 normal_mtx = mat4(transpose(inverse(mat3(u_model))));
    v_normal_modelspace = (normal_mtx * vec4(a_normal, 0.0)).xyz;

    v_color = a_color;
}

