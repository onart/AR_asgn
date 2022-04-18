#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;

uniform int mode=1;
uniform mat4 homography;
out vec2 tc;

void main(){
	float modef= int(mode==2);
	tc= texcoord*modef+(1-modef)*(1-texcoord);
	vec3 homo=mat3(homography)*vec3(position,1);
	homo.xy/=homo.z;
	homo.z=1;
	gl_Position=vec4(homo,1);
	gl_Position.xy *= modef*(-2)+1;
}