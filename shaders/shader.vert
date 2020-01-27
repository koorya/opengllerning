#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;
layout (location = 3) in float orbit_radius;
layout (location = 4) in mat4 instance_mat;
//5, 6, 7 are busy. 8 is free
out vec3 vertexNormal;
out vec3 fragPos;
out vec2 texPos;

layout (std140, binding = 2) uniform transformBlock{
	mat4 model;
	mat4 proj;
	mat4 view;
};

uniform float scale;
uniform float time;

void main(){
	
	texPos = texture;
	float phase;
	mat4 instance_mat_ = mat4(1.0);
	mat4 model_ = instance_mat * model;
	vec4 pos = model_ * vec4(position, 1.0f) ;

	fragPos = pos.xyz;
	gl_Position =  proj * view * pos;
	vertexNormal = vec3(model_ * vec4(normal, 0.0f));
}