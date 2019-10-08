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

class Shader{
public:
    GLuint Program;

    Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

    void use();
    void setVec3(glm::vec3 value, const GLchar * name);
};


#endif
