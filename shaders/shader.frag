#version 330 core

out vec4 color;
in vec4 vertexNormal;
in vec2 texPos;


uniform vec3 objColor;
uniform vec3 lightColor;

uniform int isLight;

void main(){
//   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);

	color = vec4(lightColor, 1.0f);
	if(isLight == 0)
		color = vec4(objColor * lightColor, 1.0f);

}