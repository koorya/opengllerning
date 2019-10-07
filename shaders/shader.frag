#version 330 core

out vec4 color;
in vec3 vertexNormal;
in vec3 fragPos;


uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform int isLight;

void main(){

	color = vec4(lightColor, 1.0f);

	if(isLight == 0){
		float ambientStrenght = 0.1f;
		vec3 ambient = ambientStrenght * lightColor;


		vec3 norm = normalize(vertexNormal);
		vec3 lightDir = normalize(lightPos - fragPos);
		float diff = max(0.0f, dot(norm, lightDir));
		vec3 diffuse = diff * lightColor;
		
	
		float specularStrenght = 0.5f;
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = specularStrenght*pow(max(dot(viewDir, reflectDir), 0.0f), 32);

		vec3 result = (ambient + diffuse + spec) *objColor;	
		
		color = vec4(result, 1.0f);
	}

}