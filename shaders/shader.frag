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
};

uniform Light light;
uniform Material material;

uniform vec3 viewPos;

uniform int isLight;

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

void main(){

	color = vec4(max(max(light.ambient, light.diffuse), light.specular), 1.0f);
	color = vec4(texture(myTexture1, texPos));
	if(isLight == 0){
		vec3 ambient = material.ambient * light.ambient;


		vec3 norm = normalize(vertexNormal);
		vec3 lightDir = normalize(light.position - fragPos);
		float diff = max(0.0f, dot(norm, lightDir));
		vec3 diffuse = diff * material.diffuse * light.diffuse;
		
	

		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 128.0 * material.shininess);
		vec3 specular = material.specular * light.specular * spec;

		vec3 result = ambient + diffuse + specular;	
		
		color = vec4(result, 1.0f);

		color = vec4(texture(myTexture2, texPos));

	}

}