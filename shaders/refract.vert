#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 texture;

out vec3 Normal;
out vec3 Position;

layout (std140, binding = 2) uniform transformBlock{
	mat4 model;
	mat4 proj;
	mat4 view;
};

void main(){

    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0f));
	gl_Position = proj * view * model * vec4(aPos, 1.0f);
}
