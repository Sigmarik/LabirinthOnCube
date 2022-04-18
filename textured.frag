#version 330 core

in vec2 uv;
in vec3 normal;

uniform vec3 lightDirection = vec3(1.0, 1.0, 1.0);
uniform sampler2D atlas;
uniform vec2 atlasRegionCorner = vec2(0.0, 0.0);
uniform vec2 atlasRegionSize = vec2(1.0);

out vec4 fragColor;

void main() {
	vec2 finalUV = atlasRegionCorner + uv * atlasRegionSize;
	float ambientLight = 0.6;
	float diffuseLight = max(0.0, dot(normalize(lightDirection), normal) * 0.4);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 color = texture2D(atlas, finalUV);
	if (color.a < 0.1) {
		discard;
	}
	fragColor = color;// * (diffuseLight + ambientLight);
	//fragColor = vec4(finalUV, 0.0, 1.0);
}