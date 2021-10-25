#version 330 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
	float dist = length(fragPos.xyz - lightPos);
	dist /= farPlane;
	gl_FragDepth = dist;
}