#version 330


// based initially on the Blinn-Phong shader because I think it looks nice

in vec3 normal;
in vec3 fragPosition;
uniform float focusDistance;

out vec3 color;

uniform vec3 viewPos = vec3(0, 0, 0);

uniform vec3 objectColor = vec3(1, 0, 0);

uniform vec3 lightPos = vec3(0, 1, 0);
uniform vec3 lightColor = vec3(1, 1, 1);

float ambientPower = 0.1;
float diffusePower = 0.5;
float specularPower = 0.4;



float near = 0.1; 
float far  = 100.0; 

// from LearnOpenGL.com
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
    vec3 ambient = ambientPower * lightColor;

    vec3 lightDir = normalize(lightPos - fragPosition);
    float NdotL = max(dot(normal, lightDir), 0); 

    vec3 diffuse =  diffusePower * NdotL * lightColor;

    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 halfDir = normalize(viewDir + lightDir);
    float NdotH = max(dot(normal, halfDir), 0.0); 

    vec3 specular = specularPower * pow(NdotH, 8) * lightColor;
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	

    color = (ambient + diffuse + specular) * objectColor;
}
