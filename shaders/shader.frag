#version 330 core

out vec4 color;
in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texPos;

struct DirLight{
	vec4 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material{
	int textured;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;
	sampler2D emissionTex;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_diffuse2;
	sampler2D texture_specular2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular3;

};

uniform Material material;

uniform vec3 viewPos;


#define NR_DIR_LIGHTS 3
uniform DirLight dirLight[NR_DIR_LIGHTS];

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir);

vec3 calcDirLightTex(DirLight light, vec3 norm, vec3 viewDir);

vec3 calcMaterial(vec3 norm, vec3 viewDir);
vec3 calcTextured(vec3 norm, vec3 viewDir);


void main(){

	vec3 norm = normalize(vertexNormal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = vec3(0.0f);

	if(material.textured == 1){
		result += calcTextured(norm, viewDir);
	}else{
		result += calcMaterial(norm, viewDir);
	}

	color = vec4(result, 1.0f);

}

vec3 calcMaterial(vec3 norm, vec3 viewDir){
	vec3 result = vec3(0);
	int i;
	for(i = 0; i < NR_DIR_LIGHTS; i++) 
		result += calcDirLight(dirLight[i], norm, viewDir);
	return result;
}

vec3 calcTextured(vec3 norm, vec3 viewDir){
	vec3 result = vec3(0);
	int i;
	for(i = 0; i < NR_DIR_LIGHTS; i++) 
		result += calcDirLightTex(dirLight[i], norm, viewDir);
	return result;
}


vec3 calcDirLightTex(DirLight light, vec3 norm, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(-light.direction.xyz);

	vec3 ambient = texture(material.texture_diffuse1, texPos).rgb * light.ambient;
	result += ambient;

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * texture(material.texture_diffuse1, texPos).rgb * light.diffuse;
	result += diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(texture(material.texture_specular1, texPos).rgb) * light.specular * spec;

	result += specular;

	return result;
}


vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(-light.direction.xyz);

	vec3 ambient = material.ambient * light.ambient;
	result += ambient;

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * material.diffuse * light.diffuse;
	result += diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(material.specular) * light.specular * spec;

	result += specular;

	return result;
}
