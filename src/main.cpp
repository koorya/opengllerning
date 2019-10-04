#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>

#include "shader.h"

void key_callback (GLFWwindow*,int,int,int,int);

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


	GLfloat vertices[] = {
		0.6f, 0.4f, 0.0f,  
		-0.1f, -0.7f, 0.0f,  
		-0.6f, 0.1f, 0.0f,  
		-0.3f, 0.5f, 0.0f,  
		0.4f, 0.8f, 0.0f,  
		0.4f, -0.4f, 0.0f
	};
	int points_count = sizeof(vertices)/sizeof(GLfloat)/3;

	GLfloat colored_vertices[] = {
		//position         		//colors          
		0.6f, 0.4f, 0.0f,  		0.6f, 0.4f, 0.5f,  
		-0.1f, -0.7f, 0.0f,		0.1f, 0.7f, 0.0f,  
		-0.6f, 0.1f, 0.0f, 		0.6f, 0.1f, 0.0f,  
		-0.3f, 0.5f, 0.0f, 		0.3f, 0.5f, 0.0f,  
		0.4f, 0.8f, 0.0f,  		0.4f, 0.8f, 0.0f,  
		0.4f, -0.4f, 0.0f, 		0.4f, 0.4f, 0.0f
	};
	int colored_points_count = sizeof(vertices)/sizeof(GLfloat)/6;


	GLuint indices[] = {
		0, 1, 2,
		3, 4, 5
	};

	GLuint VBO[2], EBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)/2, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colored_vertices), colored_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)( 3*sizeof(GLfloat) ) );//color
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER,0);



	glBindVertexArray(0);
	
	GLfloat vertices_post[] = {
		-0.1f, -0.7f, 0.0f,  //1
		0.4f, -0.4f, 0.0f,//5
		0.1f, -0.9f, 0.0f
	};

//	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_post), vertices_post, GL_STATIC_DRAW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ourShader.use();

	GLuint aspect = glGetUniformLocation(ourShader.Program, "ourAspect");
	glUniform1f(aspect, 600.0/800.0);

	GLuint additional_pos = glGetUniformLocation(ourShader.Program, "addPos");
	glUniform3f(additional_pos, 0.2f, 0.0f, 0.0f);

	GLuint color_uniform = glGetUniformLocation(ourShader.Program, "ourColor");
	GLfloat red_color = 0.0f;

	glUniform4f(color_uniform, red_color, 0.0f, 0.0f, 1.0f);


	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		glClearColor(0.1f, 0.4f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		red_color = sin(glfwGetTime()) / 2 + 0.5f;
		glUniform4f(color_uniform, red_color, 0.0f, 0.0f, 1.0f);

		
		glUniform3f(additional_pos, red_color, 0.0f, 0.0f);

		glBindVertexArray(VAO[1]);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//		glBindVertexArray(VAO[0]);
//		glDrawArrays(GL_TRIANGLES, 0, points_count/2);

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
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GL_TRUE);
}


