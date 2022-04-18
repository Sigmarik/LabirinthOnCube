#version 330 core

flat in int integer;

out uvec4 fragColor;

void main() {
	fragColor = uvec4(1) * uint(integer);
	// Noice!
}