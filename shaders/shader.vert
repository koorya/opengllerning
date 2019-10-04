#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_pos;
out vec4 vertexColor;
out vec2 texPos;
uniform float ourAspect;// y/x
uniform vec3 addPos;
void main(){
   vec3 pos = position + addPos;
   //pos = vec3(pos.y, -pos.x, pos.z);
   gl_Position = vec4(pos.x*ourAspect, pos.yz, 1.0);
   vertexColor = vec4(color, 1.0f);
   texPos = tex_pos;
}