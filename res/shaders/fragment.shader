#version 410 core

in VS_OUT
{
    vec3 world_pos;
    vec3 normal;
    vec3 color;
} fs_in;

out vec4 color;

uniform vec3 light_pos;
uniform vec3 light_intensity;
uniform vec3 view_pos;

uniform vec3 ambient_coef;
uniform vec3 diffuse_coef;
uniform vec3 specular_coef;

uniform int phong_exp;

void main()
{
    vec3 normal = normalize(fs_in.normal);
    // vec3 r = light_pos - fs_in.world_pos;
    vec3 light_dir = normalize(light_pos - fs_in.world_pos);
    vec3 view_dir = normalize(view_pos - fs_in.world_pos);
    vec3 h = normalize(light_dir + view_dir);

    // vec3 irradiance = light_intensity * max(0.0, dot(normal, light_dir));// / dot(r,r);
    vec3 irradiance = light_intensity * abs(dot(normal, light_dir));// / dot(r,r);

    vec3 ambient_light = ambient_coef * light_intensity;
    vec3 diffuse_light = diffuse_coef * irradiance;
    vec3 specular_light = specular_coef * irradiance * max(0.0, pow(dot(normal,h), phong_exp));

    vec3 result = fs_in.color * (ambient_light + diffuse_light + specular_light);

    color = vec4(result,1.0);
}