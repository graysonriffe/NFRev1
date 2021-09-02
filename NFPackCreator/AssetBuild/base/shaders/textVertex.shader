#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texCoords;

//uniform mat4 model;
uniform mat4 proj;

out vec2 texCoord;

void main() {
	gl_Position = proj * vec4(pos, 0.0, 1.0);
	texCoord = texCoords;
}