#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 vertexNormal;


uniform mat4 model;//помещает обект на сцену
uniform mat4 view; //ориентирует всю сцену для наблюдателя
uniform mat4 proj; // переводит сцену в 2D

void main(){

	vec4 pos =  proj * view * model * vec4(position, 1.0f) ;
	gl_Position = pos;
	vertexNormal = vec4(normal, 0.0f);
}