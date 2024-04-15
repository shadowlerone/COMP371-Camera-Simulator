#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
// uniform bool hdr;
// uniform float exposure;

void main()
{             
    // const float gamma = 2.2;
    FragColor = vec4(texture(hdrBuffer, TexCoords).rgb,1);
	return;
}