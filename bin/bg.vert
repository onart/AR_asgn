#version 330

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;

out vec2 tc;

void main(){
	tc=texcoord;
	gl_Position=vec4(position,1,1);
}