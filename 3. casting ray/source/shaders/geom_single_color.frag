#version 330 core

layout(location = 0) out vec4 out_color;
uniform vec4 color = vec4(1);

void main()
{
   	out_color = color;
}