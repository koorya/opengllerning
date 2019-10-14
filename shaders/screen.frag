#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

float offset = 1.0 / 300.0;
vec2 offsets[9] = {
	vec2(-offset, offset),
	vec2(0.0, offset),
	vec2(offset, offset),

	vec2(-offset, 0.0),
	vec2(0.0, 0.0),
	vec2(offset, 0.0),

	vec2(-offset, -offset),
	vec2(0.0, -offset),
	vec2(offset, -offset)
};

float kernel_blur [9] = float [](
	1.0/16.0,  2.0/16.0,  1.0/16.0,
	1.0/16.0,  4.0/16.0,  2.0/16.0,
	1.0/16.0,  2.0/16.0,  1.0/16.0
);


float kernel_contrast [9] = float [](
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);

void main(){
	float kernel[] = kernel_blur;
	vec3 samleTex[9];
	for(int i = 0; i < 9; i++){
		samleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += samleTex[i]*kernel[i];
		
	FragColor = vec4(col, 1.0f);
	
}
