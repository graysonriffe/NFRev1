#version 330 core

in vec2 texCoord;
in vec3 normals;
in vec3 fragPos;

struct Camera {
	vec3 pos;
};

struct Material {
	bool hasDiffuseTex;
	sampler2D diffuseTexture;
	vec3 diffuseColor;
	bool hasSpecTex;
	sampler2D specularTexture;
	float specPower;
};

struct Light {
	//Directional = 1, Point = 2
	int type;
	vec3 pos;
	vec3 color;
	float strength;
};

uniform Camera camera;
uniform Material material;
uniform Light light[100];
uniform int numberOfLights;

out vec4 outColor;

void main() {
	vec3 color = vec3(0.0);

	vec3 matDiff;
	if (material.hasDiffuseTex)
		matDiff = texture(material.diffuseTexture, texCoord).rgb;
	else
		matDiff = material.diffuseColor;

	vec3 matSpec = vec3(1.0);
	if(material.hasSpecTex)
	matSpec = texture(material.specularTexture, texCoord).rgb;

	for (int i = 0; i < numberOfLights; i++) {
		float ambientStrength = 0.2f;
		vec3 ambient = ambientStrength * matDiff;
		if (i == numberOfLights - 1 && numberOfLights == 1) {
			color += ambient;
			break;
		}

		if (light[i].type == 1) {
			vec3 lightDir = normalize(-light[i].pos);
			vec3 norm = normalize(normals);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light[i].color * (diff * matDiff) * (light[i].strength / 2.0f);

			vec3 viewDir = normalize(camera.pos - fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specPower);
			vec3 specular = light[i].color * spec * matSpec * (light[i].strength / 2.0f);

			color += (ambient + diffuse + specular);
			continue;
		}
		if (light[i].type == 2) {
			vec3 lightDir = normalize(light[i].pos - fragPos);
			vec3 norm = normalize(normals);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light[i].color * (diff * matDiff) * (light[i].strength / 2.0f);

			vec3 viewDir = normalize(camera.pos - fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specPower);
			vec3 specular = light[i].color * spec * matSpec * (light[i].strength / 2.0f);

			float length = length(light[i].pos - fragPos);
			float att = clamp(10.0 / length, 0.0, 1.0) * light[i].strength;

			color += ((ambient + diffuse + specular) * att);
			continue;
		}
	}
	outColor = vec4(color, 1.0);
}
