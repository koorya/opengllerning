#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_pos;
out vec4 vertexColor;
out vec2 texPos;
uniform float ourAspect;// y/x


uniform mat4 model;//помещает обект на сцену
uniform mat4 view; //ориентирует всю сцену для наблюдателя
uniform mat4 proj; // переводит сцену в 2D

void main(){

   vec4 pos =  proj * view * model * vec4(position, 1.0f) ;
   //pos = vec3(pos.y, -pos.x, pos.z);
   gl_Position = pos;
   vertexColor = vec4(color, 1.0f);

   texPos = tex_pos;
}