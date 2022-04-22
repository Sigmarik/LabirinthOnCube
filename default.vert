#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexTangent;
layout (location = 3) in vec2 vertexUV;
layout (location = 4) in vec4 vertexColor;
layout (location = 5) in int vertexInteger;

out vec3 worldPosition;
out vec3 localPosition;
out vec2 uv;
out vec3 normal;
out vec3 tangent;
out vec4 vertColor;
out float depth;
flat out int integer;
out vec3 preprocessWorldPosition;
out vec3 tilePosition;
out vec3 tileUp;

uniform mat4 cameraMatrix = mat4(1.0);
uniform mat4 objectMatrix = mat4(1.0);
uniform mat4 objectCenteredMatrix = mat4(1.0);
uniform float time = 0;

vec3 circularize(vec3 position) {
	return position / pow(length(position), 0.13);
}

void main() {
	integer = vertexInteger;
	localPosition = vertexPosition;
	tilePosition = (objectMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
	tileUp = (objectCenteredMatrix * vec4(0.0, 1.0, 0.0, 1.0)).xyz;
	worldPosition = (objectMatrix * vec4(vertexPosition, 1.0)).xyz;
	preprocessWorldPosition = worldPosition;
	if (vertexColor.a > 0.999 && vertexPosition.y > 0.0) {
		float animationSpeed = 1.0;
		float coordMult = 30.0;
		float amplitude = 0.008;
		float scaling = length(vec3(objectCenteredMatrix * vec4(1.0, 0.0, 0.0, 0.0)));
		float coordShift = worldPosition.x + worldPosition.y + worldPosition.z;
		coordShift *= coordMult / scaling;
		vec3 delta = vec3(sin(time * 1.0 * animationSpeed + coordShift), 
		cos(time * 0.33 * animationSpeed + coordShift), 
		cos(time * 1.2 * animationSpeed + coordShift)) * 
		pow(vertexPosition.y / 0.2, 2.0) * amplitude * scaling;
		worldPosition += delta;
	}
	//worldPosition = vertexPosition;
	worldPosition = circularize(worldPosition);
	uv = vertexUV;
	normal = normalize((objectCenteredMatrix * vec4(vertexNormal, 1.0)).xyz);
	tangent = normalize((objectCenteredMatrix * vec4(vertexTangent, 1.0)).xyz);
	vertColor = vertexColor;
	gl_Position = cameraMatrix * vec4(worldPosition, 1.0);
	depth = gl_Position.a;
}