#version 410 core

in vec4 position;

out VS_OUT
{
	vec4 color;
} vs_out;

uniform mat4 mv_matrix;
uniform int col_matrix;

void main(void)
{
	gl_Position =  mv_matrix * position;
	if(col_matrix == 0) vs_out.color = position * 2.0 + vec4(0.5, 0.5, 0.5, 0.0);
	else if(col_matrix == 1) vs_out.color = vec4(0.0, 0.0, 1.0, 0.0);
	else if(col_matrix == 2) vs_out.color = vec4(1.0, 0.0, 0.0, 0.0);
	else if(col_matrix == 3) vs_out.color = vec4(0.0, 0.0, 0.0, 0.0);
}  