#version 330 core

in vec2 texCoord;

uniform sampler2D text;
uniform vec3 textColor;

out vec4 color;

void main() {
	vec4 temp = vec4(1.0, 1.0, 1.0, texture(text, texCoord).r);
	color = vec4(textColor.xyz, 1.0) * temp;
}
