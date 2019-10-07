#version 330 core

out vec4 color;
in vec4 vertexColor;
in vec2 texPos;
uniform vec3 objColor;
uniform vec3 lightColor;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float mixParam;
uniform int isLight;

void main(){
//   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);

	float gamma = 2.2f;
	color = vec4(lightColor, 1.0f);
	if(isLight == 0)
		color = vec4(objColor * lightColor, 1.0f);

}