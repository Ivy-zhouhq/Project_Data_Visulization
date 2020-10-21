#version 330 core

in vec4 vs_color;
layout(location = 0) out vec4 out_color;
uniform vec4 color = vec4(1);

void main()
{
   	out_color = vs_color * color;
}