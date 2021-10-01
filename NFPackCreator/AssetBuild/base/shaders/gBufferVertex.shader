#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec3 tangent;
layout(location = 4) in int matN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;
out mat3 tbn;
flat out int matNum;

void main() {
	vec4 world = model * vec4(pos, 1.0);
	fragPos = world.xyz;
	texCoord = texCoords;
	mat3 normalMat = transpose(inverse(mat3(model)));
	vec3 t = normalize(normalMat * tangent);
	vec3 n = normalize(normalMat * normals);
	normal = n;
	vec3 b = cross(n, t);
	tbn = mat3(t, b, n);
	matNum = matN;

	gl_Position = proj * view * world;
}