#include "shader.h"

Shader::Shader(const GLchar * vertexPath, const GLchar * fragmentPath){
	std::string vertexCode;
	std::string fragmentCode;
	
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit);
	fShaderFile.exceptions(std::ifstream::failbit);
	
	try{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}catch(std::ifstream::failure &e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ EXEPTION IS:" <<e.what()<<  std::endl;
		exit(-1);
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * fShaderCode = fragmentCode.c_str();


	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vShaderCode, NULL);
	glCompileShader(vertex_shader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertex_shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "VERTEX SHADER COMPILE FAILED\n" << infoLog << std::endl;
	}
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fShaderCode, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragment_shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "FRAGMENT SHADER COMPILE FAILED\n" << infoLog << std::endl;
	}
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	this->Program = shader_program;
}

void Shader::use(){
	glUseProgram(this->Program);
}

void Shader::setVec3(glm::vec3 value, const GLchar * name){
	GLuint uniformLoc = glGetUniformLocation(this->Program, name);
	glUniform3fv(uniformLoc, 1, glm::value_ptr(value));
}

