#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoords;

uniform mat4 lightSpaceMat[16];
uniform int numMats;
uniform sampler2D gBPos;

out vec2 texCoord;
out vec4 fragPosLightSpace[16];

void main() {
	texCoord = texCoords;
	vec3 fragPos = texture(gBPos, texCoord).xyz;
	for (int i = 0; i < numMats; i++) {
		fragPosLightSpace[i] = lightSpaceMat[i] * vec4(fragPos, 1.0);
	}
	gl_Position = vec4(pos, 0.0, 1.0);
}