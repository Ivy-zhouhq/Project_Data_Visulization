#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

out vec4 vs_color;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main()
{
	gl_Position = proj_matrix * view_matrix * vec4(in_position, 1);
	vs_color = in_color;
}