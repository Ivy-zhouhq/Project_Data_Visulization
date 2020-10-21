#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coord;
out vec2 tex_coord;
out vec3 normal;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main()
{
	gl_Position = proj_matrix * view_matrix * vec4(in_position, 1);
	normal = mat3(view_matrix) * in_normal;
	tex_coord = in_tex_coord;
}