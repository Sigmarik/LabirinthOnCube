#version 330 core

in vec3 worldPosition;
in vec2 uv;
in vec3 tilePosition;
flat in int integer;

out vec4 fragColor;

uniform mat4 objectMatrix = mat4(1.0);
uniform vec4 skyColor = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 objectColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float time = 0.0;

const float coreRadius = 0.3;
const float bloomMax = 0.9;
const float timeMultip = 0.2;
const float spaceMultip = 20.0;

void main() {
	float dist = length(uv * 2.0 - 1.0);
	float core = coreRadius;
	if (integer % 30 == 0) {
		core *= 1.0 + sin(time * 10.0 * (1.0 - abs(cos(integer * 100.0) * 0.1)) + integer * 200.0) * 0.1;
	}
	if (dist < core) {
		fragColor = objectColor;
	} else {
		float strength = (dist - core) / (1.0 - core);
		vec4 lightColor = objectColor + vec4(
			sin(timeMultip * time + worldPosition.x * spaceMultip), 
			cos(timeMultip * time / 0.3 + worldPosition.y * spaceMultip), 
			sin(timeMultip * time * 0.7 + worldPosition.z * spaceMultip), 0.0) * 0.3;
		fragColor = mix(skyColor, lightColor, bloomMax / pow(200.0, strength));
	}
	//fragColor = vec4(integer / 1000.0);
}