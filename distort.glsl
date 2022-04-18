#version 330 core

vec2 distort(vec2 vector) {
	float distortionAmount = pow(length(vector), 0.9);
	return vector / distortionAmount;
}