#version 330 core

out vec4 color;
in vec4 vertexNormal;
in vec2 texPos;


uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform int isLight;

void main(){

	color = vec4(lightColor, 1.0f);

	if(isLight == 0){
		float ambientStrenght = 0.1f;
		vec3 ambient = ambientStrenght * lightColor;
		vec3 result = ambient * objColor;


		color = vec4(result, 1.0f);
	}

}