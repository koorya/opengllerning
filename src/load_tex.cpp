#define GLEW_STATIC

#include "load_tex.h"

#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <iostream>




unsigned int loadTexture(const char * file_path){
	std::cout<<file_path<<std::endl;
	int width, height;
	unsigned char * image = SOIL_load_image(file_path, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == 0)
		std::cout<<"LOAD IMAGE FAILED"<<std::endl;
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

