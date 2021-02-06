#version 400 core
in vec4 aPos;
in vec4 aNrm;

out vec4 vColor;

uniform vec3 objectColor;
uniform vec3 ambient;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 aLightPos;
uniform mat3 aModel;
uniform mat4 aMVP;

void main(void)
{
    float specularStrength = 0.5;
    //vec3 norm = aNrm.xyz;
    vec3 Normal = mat3(transpose(inverse(aModel))) * aNrm.xyz;
    vec3 lightDir = normalize(aLightPos - aPos.xyz);
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 viewDir = normalize(viewPos - aPos.xyz);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * objectColor;
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vColor = vec4((ambient + diffuse + specular) * objectColor, 1);
	gl_Position = aMVP * aPos;
}
