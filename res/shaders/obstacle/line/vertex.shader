#version 410 core

layout(location = 0) in vec2 position_a;
layout(location = 1) in vec2 position_b;

out VS_OUT
{
    vec2 position_a;
    vec2 position_b;
} vs_out;

void main()
{
    vs_out.position_a = position_a;
    vs_out.position_b = position_b;
}