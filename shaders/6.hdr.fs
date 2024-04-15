#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D focus;
uniform sampler2D OoFocus;
uniform sampler2D position;

uniform bool hdr;
uniform float exposure;
uniform bool pinhole;
uniform float aperture = 0.000125;
uniform float focus_distance;
const float offset = 1.0 / 300.0;  
float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	vec2 offsets[9] = vec2[](vec2(-offset, offset), // top-left
	vec2(0.0, offset), // top-center
	vec2(offset, offset), // top-right
	vec2(-offset, 0.0),   // center-left
	vec2(0.0, 0.0),   // center-center
	vec2(offset, 0.0),   // center-right
	vec2(-offset, -offset), // bottom-left
	vec2(0.0, -offset), // bottom-center
	vec2(offset, -offset)  // bottom-right    
	);

	float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);
	const float gamma = 2.2;
	vec3 hdrColor = texture(focus, TexCoords).rgb;

	vec3 pos = texture(position, TexCoords).rgb;
	float blur_component = abs(LinearizeDepth(focus_distance) - LinearizeDepth(pos.z)) * aperture;

	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(focus, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
	// FragColor = vec4(OoFocus,1);
	FragColor = vec4(texture(OoFocus, TexCoords).rgb, 1);
	// FragColor = vec4(col,1);
	// FragColor = vec4(texture(focus, TexCoords).rgb,1);
// 
    return;
	if(hdr) {
        // reinhard
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	} else {
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
}