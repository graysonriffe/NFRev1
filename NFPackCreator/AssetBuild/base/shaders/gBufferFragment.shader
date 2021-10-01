#version 330 core

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in mat3 tbn;
flat in int matNum;

uniform bool hasDiffuseTex[32];
uniform sampler2D diffuseTexture[32];
uniform vec3 diffuseColor[32];
uniform bool hasSpecTex[32];
uniform sampler2D specularTexture[32];
uniform float specPower[32];
uniform bool hasNormTex[32];
uniform sampler2D normalTexture[32];

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec3 diffuse;
layout(location = 3) out vec3 specular;

void main() {
	pos = fragPos;
	normals = hasNormTex[matNum] ? normalize(tbn * (texture(normalTexture[matNum], texCoord).xyz * 2.0 - 1.0)) : normal;
	diffuse = hasDiffuseTex[matNum] ? texture(diffuseTexture[matNum], texCoord).rgb : diffuseColor[matNum];
	specular.r = specPower[matNum];
	specular.g = hasSpecTex[matNum] ? texture(specularTexture[matNum], texCoord).r : 1.0;
	specular.b = 1.0;
}