#version 330 core

out vec4 color;
in vec4 vertexColor;
in vec2 texPos;
uniform vec4 ourColor;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform float mixParam;

void main(){
//   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);

	float gamma = 2.2f;
	
	color = ourColor;
	color.a = 0.5f;
	if((color.r + color.g + color.b)< 0.01){
		color.rgb = vec3(0.8f);
		color.a = 0.05f;
	}

		//discard;

}