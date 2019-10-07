#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <math.h>

#include "shader.h"

void key_callback (GLFWwindow*,int,int,int,int);
GLfloat mix_param = 0.5f;
GLuint mix_param_uniform;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "OpenGL Learning", nullptr, nullptr);
	if (window == nullptr){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		std::cout << "GLEW init is not OK" << std::endl;
		return -1;
	}
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetKeyCallback(window, key_callback);

	Shader ourShader( "./shaders/shader.vert", "./shaders/shader.frag");
	 
	mix_param_uniform = glGetUniformLocation(ourShader.Program, "mixParam");

	int twidth, theight;
	unsigned char * image = SOIL_load_image("./textures/container.jpg", &twidth, &theight, 0, SOIL_LOAD_RGB);
	if (!image)
		std::cout<<"LOAD IMAGE FAILED"<<std::endl;
	GLuint texture[2];
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	image = SOIL_load_image("./textures/awesomeface.png", &twidth, &theight, 0, SOIL_LOAD_RGB);
	if (!image)
		std::cout<<"LOAD IMAGE FAILED"<<std::endl;
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLfloat x = 0.5f, y = 0.5f, size = 0.5f; 
	GLfloat vertices[] = {
		//position         		//colors          	//texture coord
		-0.5f, -0.5f,  0.5f,  		0.6f, 0.4f, 0.5f, 	x - size, y - size,  		//left bot
		-0.5f,  0.5f,  0.5f,		0.1f, 0.7f, 0.0f, 	x - size, y + size,  		//left top
		 0.5f,  0.5f,  0.5f,  		0.4f, 0.8f, 0.0f, 	x + size, y + size,  		//right top
		 0.5f, -0.5f,  0.5f, 		0.4f, 0.4f, 0.0f, 	x + size, y - size,			//right bot

		-0.5f, -0.5f, -0.5f,  		0.6f, 0.4f, 0.5f, 	x + size, y + size,  		//left bot
		-0.5f,  0.5f, -0.5f,		0.1f, 0.7f, 0.0f, 	x + size, y - size,  		//left top
		 0.5f,  0.5f, -0.5f,  		0.4f, 0.8f, 0.0f, 	x - size, y - size,  		//right top
		 0.5f, -0.5f, -0.5f, 		0.4f, 0.4f, 0.0f, 	x - size, y + size			//right bot
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0,

		1, 5, 2,
		5, 2, 6,

		5, 6, 4,
		6, 4, 7,

		7, 0, 4,
		7, 0, 3,

		3, 2, 6,
		3, 6, 7,

		0, 1, 5,
		0, 4, 5
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(  0.0f,  0.0f,  0.0f),
		glm::vec3(  2.0f,  5.0f, -15.0f),
		glm::vec3( -1.5f, -2.2f, -2.5f),
		glm::vec3( -3.8f, -2.0f, -12.3f),
		glm::vec3(  2.4f, -0.4f, -3.5f),
		glm::vec3( -1.7f,  3.0f, -7.5f),
		glm::vec3(  1.3f, -2.0f, -2.5f),
		glm::vec3(  1.5f,  2.0f, -2.5f),
		glm::vec3(  1.5f,  0.2f, -1.5f),
		glm::vec3( -1.3f,  1.0f, -1.5f)
	};

	GLuint VBO[2], EBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);//pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat))); //colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat))); //texture coodr
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)-0*sizeof(GLuint), indices+0, GL_STATIC_DRAW);

	glBindVertexArray(0);


	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ourShader.use();

	GLuint aspect = glGetUniformLocation(ourShader.Program, "ourAspect");
	glUniform1f(aspect, (float)height/(float)width);

	GLuint model_loc = glGetUniformLocation(ourShader.Program, "model");
	GLuint view_loc = glGetUniformLocation(ourShader.Program, "view");
	GLuint proj_loc = glGetUniformLocation(ourShader.Program, "proj");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);




	GLuint color_uniform = glGetUniformLocation(ourShader.Program, "ourColor");
	GLfloat red_color = 0.0f;

	glUniform4f(color_uniform, red_color, 0.0f, 0.0f, 1.0f);
	glUniform1f(mix_param_uniform, mix_param);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(GL_TEXTURE_2D, texture[1]);



	glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
	glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);

	glEnable(GL_DEPTH_TEST);
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		red_color = sin(glfwGetTime()) / 2 + 0.5f;
		glUniform4f(color_uniform, red_color, 0.0f, 0.0f, 1.0f);

		
		glBindVertexArray(VAO[0]);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		view = glm::lookAt(camPos, camPos + camDir, camUp);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view) );

		proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.01f, 100.0f);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj) );

		for(int i = 10; i--;){
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			if(i % 3 == 0)
				model = glm::rotate(model, (float)glfwGetTime(),  glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glm::radians(20.0f*i),  glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model) );

			glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, EBO);

	glfwTerminate();
	return 0;
}

void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode){
	if(action == GLFW_PRESS)
		if(key == GLFW_KEY_ESCAPE){
			glfwSetWindowShouldClose(window, GL_TRUE);
		}else if(key == GLFW_KEY_UP){
			mix_param += 0.1;
			glUniform1f(mix_param_uniform, mix_param);
		}else if(key == GLFW_KEY_DOWN){
			mix_param -= 0.1;
			glUniform1f(mix_param_uniform, mix_param);
		}
}


