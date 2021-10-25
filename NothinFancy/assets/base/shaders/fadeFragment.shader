#version 330 core

uniform float opacity;

out vec4 color;

void main() {
	color = vec4(vec3(0.0), opacity);
}
