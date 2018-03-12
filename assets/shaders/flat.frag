#version 330 core

struct point_light_t
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct directional_light_t
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform point_light_t u_point_lights[NR_POINT_LIGHTS];
uniform int u_nr_point_lights;

#define NR_DIRECTIONAL_LIGHTS 1
uniform directional_light_t u_directional_lights[NR_DIRECTIONAL_LIGHTS];
uniform int u_nr_directional_lights;

uniform vec3 u_camera_pos_worldspace;

flat in vec4 v_color;
in vec3 v_normal_modelspace;

in vec3 v_position_worldspace;
in vec3 v_eye_dir_cameraspace;
flat in vec3 v_normal_cameraspace;

out vec4 out_color;

bool blinn = true;

vec3 calc_point_light(point_light_t light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 material_diffuse  = v_color.xyz;
    vec3 material_specular = v_color.xyz;
    vec3 material_ambient  = v_color.xyz;

    vec3 light_dir = normalize(light.position - frag_pos);

    // diffuse shading, cos(theta)
    float diffuse_coefficient = max(dot(normal, light_dir), 0.0);

    // specular shading, cos(alpha)
    float specular_coefficient;
    if (blinn) {
        vec3 halfway_dir = normalize(light_dir + view_dir);
        specular_coefficient = pow(max(dot(normal, halfway_dir), 0.0), 16.0);
    }
    else {
        vec3 reflect_dir = reflect(-light_dir, normal);
        specular_coefficient = pow(max(dot(view_dir, reflect_dir), 0.0), 8.0);
    }

    // attenuation TODO
    float dist = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient  = light.ambient  * material_ambient.xyz;
    vec3 diffuse  = light.diffuse  * diffuse_coefficient * material_diffuse.xyz;
    vec3 specular = light.specular * specular_coefficient * material_specular.xyz; 

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calc_directional_light(directional_light_t light, vec3 normal, vec3 view_dir)
{
    vec3 material_diffuse  = v_color.xyz;
    vec3 material_specular = v_color.xyz;
    vec3 material_ambient  = v_color.xyz;

    vec3 light_dir = normalize(-light.direction);

    // diffuse shading, cos(theta)
    float diffuse_coefficient = max(dot(normal, light_dir), 0.0);

    // specular shading, cos(alpha)
    float specular_coefficient;
    if (blinn) {
        vec3 halfway_dir = normalize(light_dir + view_dir);
        specular_coefficient = pow(max(dot(normal, halfway_dir), 0.0), 16.0);
    }
    else {
        vec3 reflect_dir = reflect(-light_dir, normal);
        specular_coefficient = pow(max(dot(view_dir, reflect_dir), 0.0), 8.0);
    }

    vec3 ambient  = light.ambient  * material_ambient.xyz;
    vec3 diffuse  = light.diffuse  * diffuse_coefficient * material_diffuse.xyz;
    vec3 specular = light.specular * specular_coefficient * material_specular.xyz; 

    return (ambient + diffuse + specular);
}

float near = 0.01;
float far  = 10.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec3 normal = normalize(v_normal_modelspace);
    vec3 frag_pos = v_position_worldspace;
    vec3 view_dir = normalize(u_camera_pos_worldspace - frag_pos);

    vec3 result = vec3 (0, 0, 0);
    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        if (i >= u_nr_point_lights) break;
        result += calc_point_light(u_point_lights[i], normal, frag_pos, view_dir);
    }
    for (int i = 0; i < NR_DIRECTIONAL_LIGHTS; ++i) {
        if (i >= u_nr_directional_lights) break;
        result += calc_directional_light(u_directional_lights[i], normal, view_dir);
    }

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // vec4 depth_color = vec4(vec3(depth), 1.0f);
    // out_color = depth_color;

    out_color = vec4(result, 1);
}

