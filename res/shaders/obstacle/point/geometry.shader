#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 50) out;

#define TWO_PI 6.28318530718
#define N 50
#define THETA (TWO_PI / N)
#define CCW_ROT_MAT mat2(cos(THETA), sin(THETA), -sin(THETA), cos(THETA))
#define CW_ROT_MAT mat2(cos(THETA), -sin(THETA), sin(THETA), cos(THETA))

uniform float u_range;
uniform mat4 u_proj_mat;

in VS_OUT
{
    vec2 position;
} gs_in[];

void main()
{
    vec2 p_center = gs_in[0].position;
    vec2 p_left = vec2(u_range, 0.0);
    vec2 p_right = vec2(u_range, 0.0);

    for(int i = 0; i < N; i++)
    {
        if(i % 2 == 0) // right
        {
            gl_Position = u_proj_mat * vec4(p_right + p_center, 0.0, 1.0);
            p_right = CCW_ROT_MAT * p_right;
        }
        else // left
        {
            p_left = CW_ROT_MAT * p_left;
            gl_Position = u_proj_mat * vec4(p_left + p_center, 0.0, 1.0);
        }
        EmitVertex();
    }
}