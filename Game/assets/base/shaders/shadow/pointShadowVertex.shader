#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 model[60];

void main() {
	gl_Position = model[gl_InstanceID] * vec4(pos, 1.0);
}