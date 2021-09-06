#version 330 core

in vec3 texCoord;

uniform samplerCube cm;

out vec4 color;

void main() {
	vec3 tc;
	tc = vec3(-texCoord.x, texCoord.y, texCoord.z);
	if (texCoord.y > 0.99 || texCoord.y < -0.99) {
		tc = vec3(texCoord.x, texCoord.y, texCoord.z);
	}
	color = texture(cm, tc);
}
