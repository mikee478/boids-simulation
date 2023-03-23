#version 410 core

layout(location = 0) in vec2 position;

out VS_OUT
{
    vec2 position;
} vs_out;

void main()
{
    vs_out.position = position;
}