#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

out VS_OUT
{
    vec3 world_pos;
    vec3 normal;
    vec3 color;
} vs_out;

uniform mat4 model_mat;
uniform mat3 normal_mat;
uniform mat4 mvp_mat;

void main()
{
    gl_Position = mvp_mat * position;

    vs_out.world_pos = (model_mat * position).xyz;
    vs_out.normal = normalize(normal_mat * normal);
    vs_out.color = color;
}