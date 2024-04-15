#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D prevFrame;
uniform float exposure;

void main()
{
    vec3 result = texture(scene, TexCoords).rgb;
	result += texture(prevFrame, TexCoords).rgb;
    FragColor = vec4(result, 1.0);
}