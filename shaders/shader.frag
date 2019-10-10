#version 330 core

out vec4 color;
in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texPos;

struct SpotLight{
	vec4 direction;
	vec4 position;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constatnt;
	float linear;
	float quadratic;
};

struct PointLight{
	vec4 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constatnt;
	float linear;
	float quadratic;
};
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

uniform int isLight;

uniform SpotLight spotLight;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);

vec3 calcDirLightTex(DirLight light, vec3 norm, vec3 viewDir);
vec3 calcPointLightTex(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLightTex(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);

vec3 calcMaterial(vec3 norm, vec3 viewDir);
vec3 calcTextured(vec3 norm, vec3 viewDir);


void main(){
	if(isLight >= 0 ){
		int i = isLight;
		color = vec4(max(max(pointLights[i].ambient, pointLights[i].diffuse), pointLights[i].specular), 1.0f);
		return;
	}


	vec3 norm = normalize(vertexNormal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = vec3(0.0f);

	if(material.textured == 1){
		result += calcTextured(norm, viewDir);
	}else{
		result += calcMaterial(norm, viewDir);
	}

	color = vec4(result, 1.0f);
//	color = vec4(1.0f);
}

vec3 calcMaterial(vec3 norm, vec3 viewDir){
	vec3 result = calcDirLight(dirLight, norm, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calcPointLight(pointLights[i], norm, fragPos, viewDir);
	}
	result += calcSpotLight(spotLight, norm, fragPos, viewDir);
	return result;
}

vec3 calcTextured(vec3 norm, vec3 viewDir){
	vec3 result = calcDirLightTex(dirLight, norm, viewDir);
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calcPointLightTex(pointLights[i], norm, fragPos, viewDir);
	}
	result += calcSpotLightTex(spotLight, norm, fragPos, viewDir);
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
vec3 calcPointLightTex(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(light.position.xyz - fragPos);
	float attenuation = 1.0f;
	float distance = length(light.position.xyz - fragPos);
	attenuation = 1.0f/(light.constatnt + (light.linear * distance)+ light.quadratic * distance * distance );

	vec3 ambient = texture(material.texture_diffuse1, texPos).rgb * light.ambient;
	result += ambient*attenuation;

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * texture(material.texture_diffuse1, texPos).rgb * light.diffuse;
	
	result += diffuse*attenuation;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(texture(material.texture_specular1, texPos).rgb) * light.specular * spec;

	result += specular*attenuation;
	
	return result;
}

vec3 calcSpotLightTex(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(light.position.xyz - fragPos);
	float attenuation = 1.0f;
	float distance = length(light.position.xyz - fragPos);
	attenuation = 1.0f/(light.constatnt + (light.linear * distance)+ light.quadratic * distance * distance );

	vec3 ambient = texture(material.texture_diffuse1, texPos).rgb * light.ambient;
	result += ambient*attenuation;
	float theta = dot(lightDir, normalize(-light.direction.xyz));

	float epsilon = light.cutOff - light.outerCutOff;
	float intensivity = clamp((theta - light.outerCutOff)/epsilon, 0.0f, 1.0f);

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * texture(material.texture_diffuse1, texPos).rgb * light.diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(texture(material.texture_specular1, texPos).rgb) * light.specular * spec;

	result += (diffuse + specular)*attenuation*intensivity;
	result += texture(material.emissionTex, texPos).rgb*diff*attenuation*intensivity;	

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
vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(light.position.xyz - fragPos);
	float attenuation = 1.0f;
	float distance = length(light.position.xyz - fragPos);
	attenuation = 1.0f/(light.constatnt + (light.linear * distance)+ light.quadratic * distance * distance );

	vec3 ambient = material.ambient * light.ambient;
	result += ambient*attenuation;

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * material.diffuse * light.diffuse;
	
	result += diffuse*attenuation;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(material.specular) * light.specular * spec;

	result += specular*attenuation;
	
	return result;
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir){
	vec3 result = vec3(0.0f);
	vec3 lightDir;
	lightDir = normalize(light.position.xyz - fragPos);
	float attenuation = 1.0f;
	float distance = length(light.position.xyz - fragPos);
	attenuation = 1.0f/(light.constatnt + (light.linear * distance)+ light.quadratic * distance * distance );

	vec3 ambient = material.ambient * light.ambient;
	result += ambient*attenuation;
	float theta = dot(lightDir, normalize(-light.direction.xyz));

	float epsilon = light.cutOff - light.outerCutOff;
	float intensivity = clamp((theta - light.outerCutOff)/epsilon, 0.0f, 1.0f);

	float diff = max(0.0f, dot(norm, lightDir));
	vec3 diffuse = diff * material.diffuse * light.diffuse;

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = length(material.specular) * light.specular * spec;

	result += (diffuse + specular)*attenuation*intensivity;
	result += material.emission * diff * attenuation*intensivity;	

	return result;
}

