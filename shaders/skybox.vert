#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;


void main(){
	TexCoords = aPos;
	mat4 view_ = mat4(mat3(view));
	gl_Position = proj * view_ * vec4(aPos, 1.0f);
	gl_Position = gl_Position.xyww;
}
