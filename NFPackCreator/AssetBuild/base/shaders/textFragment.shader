#version 330 core

out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

in vec2 texCoord;

void main() {
	vec4 temp = vec4(1.0, 1.0, 1.0, texture(text, texCoord).r);
	color = vec4(textColor.xyz, 1.0) * temp;
}
