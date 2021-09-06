#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform float opacity;

out vec4 color;

void main() {
	vec3 texColor = texture(tex, texCoord).rgb;

	color = vec4(texColor.rgb, opacity);
}
