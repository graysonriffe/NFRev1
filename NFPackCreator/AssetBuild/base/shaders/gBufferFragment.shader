#version 330 core

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;
in mat3 tbn;

struct Material {
	bool hasDiffuseTex;
	sampler2D diffuseTexture;
	vec3 diffuseColor;
	bool hasSpecTex;
	sampler2D specularTexture;
	float specPower;
	bool hasNormTex;
	sampler2D normalTexture;
};

uniform Material material;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec3 diffuse;
layout(location = 3) out vec3 specular;

void main() {
	pos = fragPos;

	if (material.hasNormTex) {
		normals = texture(material.normalTexture, texCoord).xyz;
		normals = normals * 2.0 - 1.0;
		normals = normalize(tbn * normals);
	}
	else
		normals = normal;


	if (material.hasDiffuseTex)
		diffuse = texture(material.diffuseTexture, texCoord).rgb;
	else
		diffuse = material.diffuseColor;

	specular.r = material.specPower;
	if (material.hasSpecTex)
		specular.g = texture(material.specularTexture, texCoord).r;
	else
		specular.g = 1.0;
	specular.b = 1.0;
}