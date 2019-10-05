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
	if((color.r + color.b + color.b)< 0.01)
		discard;

	if (color.a < 0.2f)
		discard;
}