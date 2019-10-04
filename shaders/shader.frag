#version 330 core

out vec4 color;
in vec4 vertexColor;
uniform vec4 ourColor;
void main(){
   color = vec4(vertexColor.rgb + ourColor.rgb, 1.0f);
}