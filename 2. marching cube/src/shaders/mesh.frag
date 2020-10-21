#version 330 core

in vec3 normal;
in vec2 tex_coord;
layout(location = 0) out vec4 out_color;

void main()
{
	const vec3 L = normalize(vec3(0,0,1));
	vec3 N = normalize(normal);

	vec3 res = max(0.0, dot(L,N)) * vec3(1,1,1);
   	out_color = vec4(res, 1);
}