#version 330 core

in vec2 texCoord;
in vec3 normals;
in vec3 fragPos;
in vec4 fragPosLightSpace[16];

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
	sampler2D directionalDepthTex;
	samplerCube pointDepthTex;
};

uniform Camera camera;
uniform Material material;
uniform Light light[100];
uniform int numberOfLights;
uniform bool isContinued;
uniform float farPlane;

out vec4 outColor;

float calcShadowDirectional(int lightNum, vec4 fragPosLight, vec3 no, vec3 lDir) {
	vec3 fp = fragPosLight.xyz / fragPosLight.w;
	fp = fp * 0.5 + 0.5;
	float current = fp.z;
	float bias = max(0.05 * (1.0 - dot(no, lDir)), 0.005);
	float shad = 0.0f;
	vec2 texSize = 1.0 / textureSize(light[lightNum].directionalDepthTex, 0);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(light[lightNum].directionalDepthTex, fp.xy + vec2(x, y) * texSize).r;
			shad += current - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shad /= 9.0;
	if (current > 1.0)
		shad = 0.0;
	return shad;
}

vec3 offsets[20] = vec3[](
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float calcShadowPoint(int lightNum, vec3 no, vec3 lDir) {
	vec3 fragLight = fragPos - light[lightNum].pos;
	float current = length(fragLight);
	float bias = 0.15;
	float closest = 0.0f;
	float shad = 0.0f;
	int samples = 20;
	float viewDist = length(camera.pos - fragPos);
	float disk = (1.0 + (viewDist / farPlane)) / 50.0f;
	for (int i = 0; i < samples; i++) {
		closest = texture(light[lightNum].pointDepthTex, fragLight + offsets[i] * disk).r;
		closest *= farPlane;
		if (current - bias > closest)
			shad += 1.0f;
	}
	shad /= float(samples);
	return shad;
}

void main() {
	vec3 color = vec3(0.0);

	vec3 matDiff;
	if (material.hasDiffuseTex)
		matDiff = texture(material.diffuseTexture, texCoord).rgb;
	else
		matDiff = material.diffuseColor;

	vec3 matSpec = vec3(1.0);
	if (material.hasSpecTex)
		matSpec = texture(material.specularTexture, texCoord).rgb;

	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * matDiff;
	if (!isContinued)
		color += ambient;
	for (int i = 0; i < numberOfLights; i++) {

		if (light[i].type == 1) {
			vec3 lightDir = normalize(light[i].pos - fragPos);
			vec3 norm = normalize(normals);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light[i].color * diff * matDiff * (light[i].strength / 2.0f);

			vec3 viewDir = normalize(camera.pos - fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specPower);
			vec3 specular = light[i].color * spec * matSpec * (light[i].strength / 2.0f);

			float shadow = calcShadowDirectional(i, fragPosLightSpace[i], norm, lightDir);
			color += (diffuse + specular) * (1.0 - shadow);
			continue;
		}
		if (light[i].type == 2) {
			vec3 lightDir = normalize(light[i].pos - fragPos);
			vec3 norm = normalize(normals);
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light[i].color * diff * matDiff * light[i].strength;

			vec3 viewDir = normalize(camera.pos - fragPos);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specPower);
			vec3 specular = light[i].color * spec * matSpec * (light[i].strength / 2.5f);

			float length = length(light[i].pos - fragPos);
			float att = clamp(10.0 / length, 0.0, 1.0) * light[i].strength;

			float shadow = calcShadowPoint(i, norm, lightDir);
			color += ((diffuse + specular) * (1.0 - shadow) * att);
			continue;
		}
	}
	outColor = vec4(color, 1.0);
}
