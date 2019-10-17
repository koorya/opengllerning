#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;
layout (location = 3) in float orbit_radius;

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
	if(orbit_radius != 0.0)
		phase = (time+1000.0) * 200 / pow(orbit_radius, 3.0/2.0);
	vec4 pos = model * vec4(position + orbit_radius*vec3(sin(phase), gl_InstanceID/10000.0, cos(phase)) + scale*normal, 1.0f) ;

	fragPos = pos.xyz;
	gl_Position =  proj * view * pos;
	vertexNormal = vec3(model * vec4(normal, 0.0f));
}