#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

void main() {
	vec4 world = model * vec4(pos, 1.0);
	fragPos = world.xyz;
	texCoord = texCoords;
	mat3 normalMat = transpose(inverse(mat3(model)));
	normal - normalMat * normals;

	gl_Position = proj * view * world;
}