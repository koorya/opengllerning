#include "shader.h"

GLfloat materials[][10] = {
	{0.0215,	0.1745,	0.0215,	0.07568,	0.61424,	0.07568,	0.633,	0.727811,	0.633,	0.6},
	{0.135,	0.2225,	0.1575,	0.54,	0.89,	0.63,	0.316228,	0.316228,	0.316228,	0.1},
	{0.05375,	0.05,	0.06625,	0.18275,	0.17,	0.22525,	0.332741,	0.328634,	0.346435,	0.3},
	{0.25,	0.20725,	0.20725,	1,	0.829,	0.829,	0.296648,	0.296648,	0.296648,	0.088},
	{0.1745,	0.01175,	0.01175,	0.61424,	0.04136,	0.04136,	0.727811,	0.626959,	0.626959,	0.6},
	{0.1,	0.18725,	0.1745,	0.396,	0.74151,	0.69102,	0.297254,	0.30829,	0.306678,	0.1},
	{0.329412,	0.223529,	0.027451,	0.780392,	0.568627,	0.113725,	0.992157,	0.941176,	0.807843,	0.21794872},
	{0.2125,	0.1275,	0.054,	0.714,	0.4284,	0.18144,	0.393548,	0.271906,	0.166721,	0.2},
	{0.25,	0.25,	0.25,	0.4,	0.4,	0.4,	0.774597,	0.774597,	0.774597,	0.6},
	{0.19125,	0.0735,	0.0225,	0.7038,	0.27048,	0.0828,	0.256777,	0.137622,	0.086014,	0.1},
	{0.24725,	0.1995,	0.0745,	0.75164,	0.60648,	0.22648,	0.628281,	0.555802,	0.366065,	0.4},
	{0.19225,	0.19225,	0.19225,	0.50754,	0.50754,	0.50754,	0.508273,	0.508273,	0.508273,	0.4},
	{0.0,	0.0,	0.0,	0.01,	0.01,	0.01,	0.50,	0.50,	0.50,	.25},
	{0.0,	0.1,	0.06,	0.0,	0.50980392,	0.50980392,	0.50196078,	0.50196078,	0.50196078,	.25},
	{0.0,	0.0,	0.0,	0.1,	0.35,	0.1,	0.45,	0.55,	0.45,	.25},
	{0.0,	0.0,	0.0,	0.5,	0.0,	0.0,	0.7,	0.6,	0.6,	.25},
	{0.0,	0.0,	0.0,	0.55,	0.55,	0.55,	0.70,	0.70,	0.70,	.25},
	{0.0,	0.0,	0.0,	0.5,	0.5,	0.0,	0.60,	0.60,	0.50,	.25},
	{0.02,	0.02,	0.02,	0.01,	0.01,	0.01,	0.4,	0.4,	0.4,	.078125},
	{0.0,	0.05,	0.05,	0.4,	0.5,	0.5,	0.04,	0.7,	0.7,	.078125},
	{0.0,	0.05,	0.0,	0.4,	0.5,	0.4,	0.04,	0.7,	0.04,	.078125},
	{0.05,	0.0,	0.0,	0.5,	0.4,	0.4,	0.7,	0.04,	0.04,	.078125},
	{0.05,	0.05,	0.05,	0.5,	0.5,	0.5,	0.7,	0.7,	0.7,	.078125},
	{0.05,	0.05,	0.0,	0.5,	0.5,	0.4,	0.7,	0.7,	0.04,	.078125}
};

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
		std::cout << fragmentPath << std::endl;
	}
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	this->Program = shader_program;
}

Shader::Shader(const GLchar * vertexPath, const GLchar * geometryPath, const GLchar * fragmentPath){
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	
	std::ifstream vShaderFile;
	std::ifstream gShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit);
	gShaderFile.exceptions(std::ifstream::failbit);
	fShaderFile.exceptions(std::ifstream::failbit);
	
	try{
		vShaderFile.open(vertexPath);
		gShaderFile.open(geometryPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, gShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		gShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		geometryCode = gShaderStream.str();
		fragmentCode = fShaderStream.str();

	}catch(std::ifstream::failure &e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ EXEPTION IS:" <<e.what()<<  std::endl;
		exit(-1);
	}
	const GLchar * vShaderCode = vertexCode.c_str();
	const GLchar * gShaderCode = geometryCode.c_str();
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
	GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader, 1, &gShaderCode, NULL);
	glCompileShader(geometry_shader);
	glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(geometry_shader, sizeof(infoLog), NULL, infoLog);
		std::cout << "GEOMETRY SHADER COMPILE FAILED\n" << infoLog << std::endl;
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
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	

	glDeleteShader(vertex_shader);
	glDeleteShader(geometry_shader);
	glDeleteShader(fragment_shader);

	this->Program = shader_program;
}

void Shader::use(){
	glUseProgram(this->Program);
}

void Shader::setVec3(glm::vec3 value, const GLchar * name){
	GLint uniformLoc = glGetUniformLocation(this->Program, name);
	if(uniformLoc != -1)
		glUniform3fv(uniformLoc, 1, glm::value_ptr(value));
}
void Shader::setVec4(glm::vec4 value, const GLchar * name){
	GLint uniformLoc = glGetUniformLocation(this->Program, name);
	if(uniformLoc != -1)
		glUniform4fv(uniformLoc, 1, glm::value_ptr(value));
}
void Shader::setMat4(glm::mat4 value, const GLchar * name){
	GLint uniformLoc = glGetUniformLocation(this->Program, name);
	if(uniformLoc != -1)
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value) );
}
void Shader::setFloat(GLfloat value, const GLchar * name){
	GLint uniformLoc = glGetUniformLocation(this->Program, name);
	if(uniformLoc != -1)
		glUniform1f(uniformLoc, value);
}
void Shader::setInt(GLint value, const GLchar * name){
	GLint uniformLoc = glGetUniformLocation(this->Program, name);
	if(uniformLoc != -1)
		glUniform1i(uniformLoc, value);
}

void Shader::setMaterial(Material mat){
	GLfloat * material = materials[mat];
	this->setVec3(glm::vec3(material[0], material[1], material[2]), "material.ambient");
	this->setVec3(glm::vec3(material[3], material[4], material[5]), "material.diffuse");
	this->setVec3(glm::vec3(material[6], material[7], material[8]), "material.specular");
	this->setFloat(material[9], "material.shininess");
}



