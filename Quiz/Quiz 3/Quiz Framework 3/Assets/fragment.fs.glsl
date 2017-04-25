#version 410

in vec2 vv2texcoord;

uniform sampler2D us2dtex;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(us2dtex, vv2texcoord).rgb, 1.0);
}