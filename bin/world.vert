#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 norm;
layout(location=2) in vec2 texcoord;

out vec3 normal;
out vec2 tc;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	tc=texcoord;
	normal=norm;
	gl_Position=proj*view*model*vec4(position,1);
}