#version 330 core

out vec4 color;
in vec4 vertexColor;
in vec2 texPos;
uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main(){
//   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);

	float gamma = 2.2f;
	

	color = texture(ourTexture, texPos) + vertexColor;
}