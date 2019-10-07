#version 330 core

out vec4 color;
in vec3 vertexNormal;
in vec3 fragPos;


uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

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
		
		vec3 result = (0.0f*ambient + diffuse) *objColor;		
;

		color = vec4(result, 1.0f);
	}

}