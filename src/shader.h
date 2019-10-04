#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

class Shader{
public:
    GLuint Program;

    Shader(const GLchar * vertexPath, const GLchar * fragmentPath);

    void use();
};


#endif
