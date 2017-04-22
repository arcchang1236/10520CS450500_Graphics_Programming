#version 410 core

in vec3 vv3color;
uniform int i = 0;
out vec4 fragColor;
void main() 
{
	if(i == 0) fragColor = vec4(sin(gl_FragCoord.xyz/10), 1.0);
	else{
		if(mod(gl_FragCoord.x ,20) <= 17 && mod(gl_FragCoord.y ,20) <= 7){
			fragColor = vec4(1, 0 , 0, 1.0);
		}else{ 
			fragColor = vec4(1, 1 , 1, 1.0);
		}
	
	}
}



