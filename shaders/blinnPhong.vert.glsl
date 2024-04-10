#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout (location = 2) in vec2 aTexCoords;

out vec3 normal;
out vec3 fragPosition;
out vec2 TexCoords;
// uniform mat4 transform;

uniform mat4 m = mat4(1.0);
uniform mat4 transform;
uniform mat4 mvp = mat4(1.0);

void main()
{
    gl_Position = mvp *  vec4(pos, 1);

    normal = normalize(mat3(transpose(inverse(m))) * norm);
    TexCoords = aTexCoords;    

    fragPosition = vec3(m * vec4(pos, 1));
}



/*

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
*/
