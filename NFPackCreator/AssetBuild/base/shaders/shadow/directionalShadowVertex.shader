#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 lightSpace;
uniform mat4 model;

out vec3 texCoord;

void main() {
	gl_Position = lightSpace * model * vec4(pos, 1.0);
}