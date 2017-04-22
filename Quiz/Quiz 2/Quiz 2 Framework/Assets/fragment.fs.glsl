#version 410

in vec3 vv3color;

layout(location = 0) out vec4 fragColor;

uniform int flag = 0;

void main()
{
	if(flag == 0) fragColor = vec4(vv3color, 1.0);
	else if(flag == 1) fragColor = vec4(vec3(1.0), 1.0) - vec4(vv3color, 1.0);
}