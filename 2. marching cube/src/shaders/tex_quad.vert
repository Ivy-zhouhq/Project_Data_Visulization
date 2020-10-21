#version 330 core
uniform vec2 min_coord = vec2(0);
uniform vec2 max_coord = vec2(1);

in vec4 position;
out vec2 tex_coord;

void main()
{
	vec2 p = position.xy * 0.5 + 0.5;
	vec2 s = max_coord - min_coord;
	vec2 c = min_coord + p * s;

	gl_Position = vec4(c * 2.0 - 1.0, 0, 1);
	tex_coord = p;
}