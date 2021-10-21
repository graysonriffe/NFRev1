#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 view;
uniform mat4 proj;

out vec3 texCoord;

void main() {
	texCoord = pos;

	vec4 vertexPos = proj * view * vec4(pos, 1.0);
	gl_Position = vertexPos.xyww;
}