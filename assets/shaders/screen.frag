#version 330 core

in vec4 v_color;
in vec2 v_texcoord;

out vec4 out_color;

uniform sampler2D u_texture_diffuse_0;

uniform float wave_time;
uniform float wave_amplifier;

uniform float greyscale_amplifier;
uniform float negative_amplifier;

void main()
{
    vec2 texcoord = v_texcoord;

    // sine wave
    texcoord.x += wave_amplifier * sin(texcoord.y * 4*2*3.14159 + wave_time) / 100;
    texcoord.y += wave_amplifier * cos(texcoord.x * 4*2*3.14159 + wave_time) / 100;

    vec4 diffuse = texture(u_texture_diffuse_0, texcoord);

    // grayscale
    float average = dot(vec3(0.2126, 0.7152, 0.0722), diffuse.rgb);
    diffuse = mix(diffuse, vec4(average, average, average, 1.0), greyscale_amplifier);

    // negative
    vec4 neg_diffuse = vec4(vec3(1.0 - diffuse), 1.0);
    diffuse = mix(diffuse, neg_diffuse,  negative_amplifier);

    out_color = diffuse;
}
