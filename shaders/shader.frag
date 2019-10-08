#version 330 core

out vec4 color;
in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texPos;

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuseTex;
	sampler2D specularTex;

};

uniform Light light;
uniform Material material;

uniform vec3 viewPos;

uniform int isLight;


void main(){

	color = vec4(max(max(light.ambient, light.diffuse), light.specular), 1.0f);
	if(isLight == 0){
		vec3 ambient = texture(material.diffuseTex, texPos).rgb * light.ambient;


		vec3 norm = normalize(vertexNormal);
		vec3 lightDir = normalize(light.position - fragPos);
		float diff = max(0.0f, dot(norm, lightDir));
		vec3 diffuse = diff * texture(material.diffuseTex, texPos).rgb * light.diffuse;
		
	

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
		vec3 specular = length(texture(material.specularTex, texPos).rgb) * light.specular * spec;

		vec3 result = ambient + diffuse + specular;	
		
		color = vec4(result, 1.0f);

//		color = vec4(texture(myTexture2, texPos));

	}

}