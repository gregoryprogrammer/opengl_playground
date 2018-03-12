#version 330 core

in vec3 v_texcoord;

out vec4 out_color;

uniform samplerCube u_texture_skybox_0;

void main()
{
    out_color = texture(u_texture_skybox_0, v_texcoord);
}

