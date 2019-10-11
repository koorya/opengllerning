#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;

out vec3 vertexNormal;
out vec3 fragPos;
out vec2 texPos;

uniform mat4 model;//помещает обект на сцену
uniform mat4 view; //ориентирует всю сцену для наблюдателя
uniform mat4 proj; // переводит сцену в 2D
uniform float scale;

void main(){

	texPos = texture;
	vec4 pos =  proj * view * model * vec4(position + scale*normal, 1.0f) ;
	fragPos = vec3(model * vec4(position, 1.0f));
	gl_Position = pos;
	vertexNormal = vec3(model * vec4(normal, 0.0f));
}