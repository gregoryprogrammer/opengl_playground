#version 330 core

#define POSITION 0
#define COLOR 1
#define TEXCOORD 2
#define NORMAL 3

layout (location = POSITION) in vec3 a_position;
layout (location = COLOR) in vec4 a_color;
layout (location = TEXCOORD) in vec2 a_texcoord;
layout (location = NORMAL) in vec2 a_normal;

out vec4 v_color;
out vec2 v_texcoord;

void main()
{
        gl_Position = vec4(a_position.xy, 0.0f, 1.0f);
        v_texcoord = a_texcoord;
        v_color = a_color;
}

