#version 410

in vec3 vv3color;

uniform int u1iflag;

layout(location = 0) out vec4 fragColor;

void main()
{
	if (u1iflag == 1)
	{
		fragColor = vec4(vec3(1.0) - vv3color, 1.0);
	}
	else
	{
		fragColor = vec4(vv3color, 1.0);
	}
}