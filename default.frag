#version 330 core

#define delta 0.5

in vec3 worldPosition;
in vec3 localPosition;
in vec3 normal;
in vec3 tangent;
in vec2 uv;
in vec4 vertColor;
in float depth;
flat in int integer;
in vec3 preprocessWorldPosition;
in vec3 tilePosition;
in vec3 tileUp;

out vec4 fragColor;

uniform vec3 lightDirection = vec3(1.0, 1.0, 1.0);
uniform mat4 objectMatrix = mat4(1.0);
uniform mat4 cameraMatrix = mat4(1.0);
uniform mat4 shadowMatrix = mat4(1.0);
uniform sampler2D shadowmap;
uniform sampler2D noiseMap;
uniform vec3 cameraPosition = vec3(0.0);
uniform float highlight = 0.0;
uniform float time;
uniform float cameraWH;

const float atmosphereTop = 0.2;
const int fogIterations = 100;
const float lightbugRadius = 0.2;
const vec4 moonColor = vec4(0.2, 0.2, 0.23, 1.0);
const vec4 sunColor = vec4(1.0, 0.97, 0.96, 1.0);
const vec4 ambientColor = vec4(1.2, 1.1, 1.3, 1.0);

float inLight(vec3 position) {
	vec4 shadowmapCoordinate = shadowMatrix * vec4(position, 1.0);
	shadowmapCoordinate /= shadowmapCoordinate.w;
	shadowmapCoordinate += 1.0;
	shadowmapCoordinate /= 2.0;
	float answer = 0.0;
	float shadowDelta = 0.0001;
	float distortionStep = shadowDelta;
	int sampleCounter = 0;
	if (shadowmapCoordinate.z < texture(shadowmap, shadowmapCoordinate.xy).r + shadowDelta) answer += 1.0;
	if (shadowmapCoordinate.z < texture(shadowmap, 
		shadowmapCoordinate.xy + vec2(distortionStep, distortionStep)).r + shadowDelta) answer += 1.0;
	if (shadowmapCoordinate.z < texture(shadowmap, 
		shadowmapCoordinate.xy + vec2(distortionStep, -distortionStep)).r + shadowDelta) answer += 1.0;
	if (shadowmapCoordinate.z < texture(shadowmap, 
		shadowmapCoordinate.xy + vec2(-distortionStep, distortionStep)).r + shadowDelta) answer += 1.0;
	if (shadowmapCoordinate.z < texture(shadowmap, 
		shadowmapCoordinate.xy + vec2(-distortionStep, -distortionStep)).r + shadowDelta) answer += 1.0;
	return answer / 5.0;
}

vec3 decubicate(vec3 vector) {
	vec3 absed = vec3(abs(vector.x), abs(vector.y), abs(vector.z));
	if (absed.x >= absed.y && absed.x >= absed.z) {
		return vec3(absed.y, absed.z, absed.x);
	}
	if (absed.y >= absed.x && absed.y >= absed.z) {
		return vec3(absed.x, absed.z, absed.y);
	}
	if (absed.z >= absed.y && absed.z >= absed.x) {
		return vec3(absed.x, absed.y, absed.z);
	}
}

vec4 circularize(vec4 position) {
	return vec4(position.xyz / pow(length(position.xyz), 0.13), position.w);
}

float fogDencity(vec3 worldPosition) {
	return 0;
	vec3 absoluted = decubicate(worldPosition);
	float surfaceHeight = (absoluted.z * 2.0 - 1.0) / atmosphereTop;
	return pow(0.01, surfaceHeight) * 0.1;
}

const float ambientLight = 0.5;

float operateLightbug(vec3 lightbugPosition, vec3 pointNormal, float specularity) {
	float lightbugLight = 1.3 * pow(0.016, distance(lightbugPosition, localPosition) / lightbugRadius);

	vec4 lightbugWorld = circularize(objectMatrix * vec4(lightbugPosition, 1.0));
	vec3 reflectedLightbug = worldPosition + reflect(lightbugWorld.xyz - worldPosition, pointNormal);
	vec4 projectedRLB = cameraMatrix * vec4(reflectedLightbug, 1.0);
	projectedRLB /= projectedRLB.w;
	vec4 projectedLBPosition = (cameraMatrix * lightbugWorld);
	projectedLBPosition /= projectedLBPosition.w;

	vec4 projectedPosition = (cameraMatrix * vec4(worldPosition, 1.0));
	projectedPosition /= projectedPosition.w;

	float scale = length(vec3(objectMatrix * vec4(1.0, 0.0, 0.0, 0.0)));
	projectedRLB.x *= cameraWH;
	projectedLBPosition.x *= cameraWH;
	projectedPosition.x *= cameraWH;
	if (distance(projectedRLB.xy, projectedPosition.xy) < 0.01 * scale) {
		lightbugLight += 3.0 * specularity;
	}
	if (distance(projectedLBPosition.xy, projectedPosition.xy) < 0.01 * scale && projectedLBPosition.z < projectedPosition.z) {
		lightbugLight += 3.0;
	}
	return lightbugLight;
}

void main() {
	float noiseValue = texture(noiseMap, vec2(vertColor.x + tilePosition.x, vertColor.z + tilePosition.y + tilePosition.z)).r;
	vec3 newNormal = normal;
	vec3 normalDelta = vec3(objectMatrix * vec4(texture(noiseMap, vec2(vertColor.x, vertColor.z) * 7.0).r - 0.5,
			0.0, texture(noiseMap, vec2(vertColor.x, vertColor.z) * -10.0).r - 0.5, 0.0) * 0.3);
	if (vertColor.a < 0.999) {
		newNormal += normalDelta;
		newNormal = normalize(newNormal);
	}
	float diffuseLight = max(0.0, dot(normalize(lightDirection), newNormal) * (1.0 - ambientLight)); 
	float moonLight = max(0.0, dot(normalize(-lightDirection), newNormal) * (1.0 - ambientLight)); 
	float specularLight = 0.0;
	vec3 cameraVector = normalize(cameraPosition - worldPosition);
	vec3 medianVector = normalize(cameraVector + normalize(lightDirection));
	float specularFactor = 0.0;
	if (noiseValue * vertColor.a > 0.5 && vertColor.a < 0.999) {
		specularFactor = noiseValue;
	}
	specularLight = pow(max(0.0, dot(medianVector, newNormal)), 60.0) * specularFactor * 0.2;
	vec4 trailColor = vec4(0.9, 0.6, 0.3, 1.0);
	trailColor.rgb *= 1.0 - specularFactor * 0.13;
	vec4 topsColor = vec4(0.3, 0.6, 0.3, 1.0);
	vec4 bottomsColor = vec4(0.1, 0.1, 0.16, 1.0) / 3.0;
	vec4 wallColor = mix(bottomsColor, topsColor, 0.2 + pow(vertColor.y, 0.5) * 0.8);
	float mixCoefficient = pow(clamp(vertColor.a + noiseValue * 0.5 * (1.0 - vertColor.a), 0.0, 1.0), 5.0);
	vec4 finalColor = mix(wallColor, trailColor, mixCoefficient);
	float borderAmount = pow(min(1.0, min(1.0 - abs(vertColor.x), 1.0 - abs(vertColor.z)) * 5.0), 0.7);
	vec4 paperColor = mix(vec4(0.8, 0.7, 0.6, 1.0), vec4(1.0, 1.0, 1.0, 1.0), highlight);
	finalColor = mix(paperColor, finalColor, borderAmount);
	if (vertColor.a >= 0.999) {
		finalColor = vertColor;
	}
	float deltaLight = 0.005;
	float lightAmount = inLight(worldPosition);
	vec4 environmentColor = vec4(1.2, 1.1, 1.3, 1.0);
	//environmentColor += vec4(sin(time * 5.0) + 1.0, cos(time * 10.0) + 1.0, cos(time * 3.0) + 1.0, 1.0) / 3.0;

	//float sumFog = 0.0;
	//for (int i = 0; i < fogIterations; i++) {
	//	vec3 position = mix(cameraPosition, preprocessWorldPosition, float(i) / fogIterations);
	//	sumFog += fogDencity(position) * inLight(position);
	//}
	//sumFog *= distance(cameraPosition, preprocessWorldPosition) / fogIterations;
	 
	float lightbugLight = 0.0;
	vec3 lightbugColor = vec3(1.0, 0.9, 0.5);
	vec3 lightbugPosition = vec3(
		sin(time * 0.5 + (tilePosition.x + tilePosition.z) * 7) * 0.4,
		0.14 + sin(time * 0.4 + (tilePosition.x + tilePosition.y) * 7) * 0.01,
		cos(time * 0.3 + (tilePosition.y + tilePosition.z) * 7) * 0.4
	);
	lightbugLight += operateLightbug(lightbugPosition, normalize(tileUp + normalDelta), specularFactor * 0.6);

	lightbugPosition = vec3(
		sin(time * 0.3 + (tilePosition.x + tilePosition.z) * 70) * 0.4,
		0.14 + sin(time * 0.4 + (tilePosition.x + tilePosition.y) * 3) * 0.01,
		cos(time * 0.7 + (tilePosition.y + tilePosition.z) * 70) * 0.4
	);
	lightbugLight += operateLightbug(lightbugPosition, normalize(tileUp + normalDelta), specularFactor * 0.6);

	lightbugLight *= sqrt(max(0.0, -dot(lightDirection, tileUp)));

	fragColor = mix(finalColor * environmentColor * ambientLight
		+ finalColor * sunColor * diffuseLight * lightAmount 
		+ finalColor * moonColor * moonLight * (1.0 - lightAmount)
		+ sunColor * specularLight * lightAmount + vec4(lightbugColor * lightbugLight, 0.0), 
		vec4(0.8, 0.79, 0.8, 1.0), fogDencity(preprocessWorldPosition));

	//fragColor = vec4(integer / (6.0 * 9.0));
}