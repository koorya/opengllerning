#version 330 core

out vec4 color;
in vec4 vertexColor;
in vec2 texPos;
uniform vec4 ourColor;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main(){
//   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);

	float gamma = 2.2f;
	

	color = mix(texture(ourTexture1, texPos), texture(ourTexture2, vec2( texPos.x, 1.0f - texPos.y)), 0.2f);
}