#version 330 core

uniform mat4 modelView;
in vec4 vPosition;
out vec3 texCoord;

void main() {

	gl_Position = modelView * vPosition;
	texCoord = vPosition.xyz;
}