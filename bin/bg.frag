#version 330

in vec2 tc;
out vec4 fragColor;

uniform sampler2D image;

void main(){
	fragColor=texture(image,tc);
}