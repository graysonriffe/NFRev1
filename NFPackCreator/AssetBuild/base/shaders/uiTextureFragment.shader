#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform float opacity;

out vec4 color;

void main() {
	vec4 texColor = texture(tex, texCoord);

	color = vec4(texColor.rgb, texColor.a * opacity);
}
