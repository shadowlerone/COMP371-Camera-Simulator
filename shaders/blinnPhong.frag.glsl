#version 330

in vec3 normal;
in vec3 fragPosition;
struct PointLight {
    vec3 position;
        float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 color;

uniform vec3 viewPos = vec3(0, 0, 0);

uniform int isLight = 0;

uniform float exposure = 1.0;

uniform vec3 objectColor = vec3(1, 0, 0);

uniform vec3 lightPos = vec3(0, 1, 0);
uniform PointLight pointLights[2];

uniform vec3 lightColor = vec3(1, 1, 1);
in vec2 TexCoords;
uniform sampler2D texture_diffuse;
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPosition);

    vec3 diffuse;
    // vec4 r = texture(texture_diffuse, TexCoords);
    vec3 r = objectColor;
    // result = vec3(0,0,0);
    // NOTE: This if-statement can be replaced with a color ramp image
    // for (int i = 0; i < 2; i++){
    //     vec3 lightDir = normalize(pointLights[i].position - fragPosition);
    //     float NdotL = max(dot(normal, lightDir), 0); 
    //         if(NdotL > 0.9)
    //         {
    //             diffuse = vec3(1, 1, 1);
    //         } else if (NdotL > 0.75) {
    //             diffuse = vec3(0.75, 0.75, 0.75);
    //         } else if (NdotL > 0.5) {
    //             diffuse = vec3(0.5, 0.5, 0.5);
    //         } else {
    //         diffuse = vec3(0.1, 0.1, 0.1);
    //         }
    //         r += vec4(diffuse,0);
    // }
    
	if (isLight ==0) {
    	for(int i = 0; i < 2; i++)
        	r += CalcPointLight(pointLights[i], norm, fragPosition, viewDir);    
    color = texture(texture_diffuse, TexCoords) * vec4(r,1);// * objectColor;

	} else {
		color = vec4(r,1);
	}


    // color = texture(texture_diffuse, TexCoords) * vec4(r,1);// * objectColor;
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 18);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
    // combine results
    vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff; //* vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec; // * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// #version 330

// struct PointLight {
//     vec3 position;
    
//     float constant;
//     float linear;
//     float quadratic;
	
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// in vec3 normal;
// in vec4 fragPosition;
// in vec2 TexCoords;

// out vec4 color;
// // out vec4 FragColor;
// uniform sampler2D texture_diffuse1;

// #define NR_POINT_LIGHTS 2
// uniform PointLight pointLights[NR_POINT_LIGHTS];

// uniform vec3 viewPos = vec3(0, 0, 0);

// uniform vec3 objectColor = vec3(0.5, 0.5, 0.5);

// uniform vec3 lightPos = vec3(0, 1, 0);
// uniform vec3 lightColor = vec3(1, 1, 1);

// float ambientPower = 0.1;
// float diffusePower = 0.5;
// float specularPower = 0.4;
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

// void main()
// {
//     // properties
// //     vec3 norm = normalize(normal);
// //     vec3 viewDir = normalize(viewPos - fragPosition);
    
// //     // == =====================================================
// //     // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
// //     // For each phase, a calculate function is defined that calculates the corresponding color
// //     // per lamp. In the main() function we take all the calculated colors and sum them up for
// //     // this fragment's final color.
// //     // == =====================================================
// //     // phase 1: directional lighting
// //     vec3 result = CalcDirLight(dirLight, norm, viewDir);
// //     vec3 result = vec3(0.0);
// //     // phase 2: point lights
// //     for(int i = 0; i < NR_POINT_LIGHTS; i++)
// //         result += CalcPointLight(pointLights[i], norm, fragPosition, viewDir);    
// //     // phase 3: spot light
// //    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
// //     color = vec4(result, 1.0);
//     // color = texture(texture_diffuse1, TexCoords);
//     color = vec4(1,1,1,1)

// }
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
//     // attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//     // combine results
//     vec3 ambient = light.ambient;// * vec3(texture(material.diffuse, TexCoords));
//     vec3 diffuse = light.diffuse; // * diff * vec3(texture(material.diffuse, TexCoords));
//     vec3 specular = light.specular;// * spec * vec3(texture(material.specular, TexCoords));
//     ambient *= attenuation;
//     diffuse *= attenuation;
//     specular *= attenuation;
//     return (ambient + diffuse + specular);
// }