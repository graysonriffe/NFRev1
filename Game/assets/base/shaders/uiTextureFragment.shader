#version 330 core

in vec2 texCoord;

uniform sampler2D tex;
uniform float opacity;

out vec4 outColor;

void main() {
	vec4 texColor = texture(tex, texCoord);
	vec4 color = texColor;
	//TODO: Move this to a post-processing pass too
	float gamma = 2.2;
	color.rgb = pow(color.rgb, vec3(1.0 / gamma));
	outColor = vec4(color.rgb, texColor.a * opacity);
}