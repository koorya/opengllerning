#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Material{
	emerald = 0,
	jade,
	obsidian,
	pearl,
	ruby,
	turquoise,
	brass,
	bronze,
	chrome,
	copper,
	gold,
	silver,
	black_plastic,
	cyan_plastic,
	green_plastic,
	red_plastic,
	white_plastic,
	yellow_plastic,
	black_rubber,
	cyan_rubber,
	green_rubber,
	red_rubber,
	white_rubber,
	yellow_rubber
};
class Shader{
public:
	GLuint Program;

	Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

	void use();
	void setVec3(glm::vec3 value, const GLchar * name);
	void setVec4(glm::vec4 value, const GLchar * name);
	void setFloat(GLfloat value, const GLchar * name);
	void setMaterial(Material mat);
	
};


#endif
