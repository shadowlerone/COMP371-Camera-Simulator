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
    // const float gamma = 2.2;
    // vec3 hdrColor = text(scene, Textcoords).rgb;
    // vec3 prevFrameColor = texture(prevFrame, TexCoords).rgb;

    // hdrColor += prevFrameColor;

    // // vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // // returl = pow(result, vec3(1.0/gamma));
    FragColor = vec4(result, 1.0);
}