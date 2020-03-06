#version 420 core

layout (location = 0) in vec3 position;


layout (std140, binding = 2) uniform transformBlock{
	mat4 model;
	mat4 proj;
	mat4 view;
};


void main(){



	vec4 pos = model * vec4(position, 1.0f) ;

	gl_Position =  proj * view * pos;

  //  gl_Position = vec4(position.x, position.y, position.z, 1.0f);
}
