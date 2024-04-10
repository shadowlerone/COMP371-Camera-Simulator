#version 330 core

layout (location = 0) in vec3 position;

uniform vec2 viewport; //Width and Height of the viewport

void main() {

	// From pixels to 0-1
	vec2 coord = position.xy / viewport;

	// Flip Y so that 0 is top
	coord.y = (1.0-coord.y);

	// Map to NDC -1,+1
	coord.xy = coord.xy * 2.0 - 1.0;

	gl_Position = vec4(coord.xy, 0.0, 1.0);

}