#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 128) out;

#define PI 3.14159265359
#define N 50
#define THETA (PI / (N-1))
#define CCW_ROT_MAT mat2(cos(THETA), sin(THETA), -sin(THETA), cos(THETA))
#define CW_ROT_MAT mat2(cos(THETA), -sin(THETA), sin(THETA), cos(THETA))

uniform float u_range;
uniform mat4 u_proj_mat;

in VS_OUT
{
    vec2 position_a;
    vec2 position_b;
} gs_in[];

void DrawHalfCircle(vec2 p_center, vec2 p_left, vec2 p_right)
{
    for(int i = 0; i < N; i++)
    {
        if(i % 2 == 0) // right
        {
            gl_Position = u_proj_mat * vec4(p_right + p_center, 0.0, 1.0);
            p_right = CCW_ROT_MAT * p_right;
        }
        else // left
        {
            gl_Position = u_proj_mat * vec4(p_left + p_center, 0.0, 1.0);
            p_left = CW_ROT_MAT * p_left;
        }
        EmitVertex();
    }
    EndPrimitive();
}

void main()
{
    vec2 a = gs_in[0].position_a;
    vec2 b = gs_in[0].position_b;

    vec3 up = vec3(0.0, 0.0, 1.0);
    vec3 a_to_b = vec3(b - a, 0.0);
    vec2 right = normalize(cross(a_to_b, up).xy);

    gl_Position = u_proj_mat * vec4(a + right * u_range, 0.0, 1.0);
    EmitVertex();
    gl_Position = u_proj_mat * vec4(a - right * u_range, 0.0, 1.0);
    EmitVertex();

    gl_Position = u_proj_mat * vec4(b + right * u_range, 0.0, 1.0);
    EmitVertex();
    gl_Position = u_proj_mat * vec4(b - right * u_range, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();

    DrawHalfCircle(a,  right * u_range, -right * u_range); // left
    DrawHalfCircle(b, -right * u_range,  right * u_range); // right
}