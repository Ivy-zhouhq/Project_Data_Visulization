#version 330 core
uniform sampler2D texture0;

in vec2 tex_coord;
layout(location = 0) out vec4 out_color;

void main()
{
	vec4 val = texture(texture0, tex_coord);
   	out_color = val;
}