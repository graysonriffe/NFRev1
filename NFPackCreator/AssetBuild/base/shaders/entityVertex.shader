#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightSpaceMat[16];
uniform int numMats;

out vec2 texCoord;
out vec3 normals;
out vec3 fragPos;
out vec4 fragPosLightSpace[16];

void main() {
	texCoord = texCoords;
	normals = mat3(transpose(inverse(model))) * normal;
	fragPos = vec3(model * vec4(pos, 1.0));
	for (int i = 0; i < numMats; i++) {
		fragPosLightSpace[i] = lightSpaceMat[i] * vec4(fragPos, 1.0);
	}
	gl_Position = proj * view * vec4(fragPos, 1.0);
}