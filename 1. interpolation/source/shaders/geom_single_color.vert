#version 330 core

layout(location = 0) in vec3 in_position;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main()
{
	gl_Position = proj_matrix * view_matrix * vec4(in_position, 1);
}