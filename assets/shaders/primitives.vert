#version 330 core

#define POSITION 0
#define COLOR 1
#define TEXCOORD 2
#define NORMAL 3

layout (location = POSITION) in vec3 a_position;
layout (location = COLOR) in vec4 a_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec4 v_color;

vec2 pixel_perfect(vec2 position)
{
    vec2 fix = vec2(0.375, 0.375);

    ivec2 pixels = ivec2(position * 100.0);
    vec2 meters = (vec2(pixels) + fix) / 100.0;

    return meters;
}

void main()
{
    vec3 position = a_position;

    // fix position to screen pixels
    /* position = vec3(pixel_perfect(a_position.xy), a_position.z); */

    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);

    v_color = a_color;
}

