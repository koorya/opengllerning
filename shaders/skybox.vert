#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 2) uniform transformBlock{
	mat4 model;	//16	//0
				//16	//16
				//16	//32
				//16	//48
	mat4 proj;	//16	//64
				//16	//80
				//16	//96
				//16	//112
	mat4 view;	//16	//128
				//16	//144
				//16	//160
				//16	//176
};


void main(){
	TexCoords = aPos;
	mat4 view_ = mat4(mat3(view));
	gl_Position = proj * view_ * vec4(aPos, 1.0f);
	gl_Position = gl_Position.xyww;
}
