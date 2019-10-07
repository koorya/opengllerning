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
#include "camera.h"

void do_movement();
void key_callback (GLFWwindow*,int,int,int,int);
void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);

Camera my_cam(glm::vec3(1.0f, 1.0f, 3.0f));


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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	Shader ourShader( "./shaders/shader.vert", "./shaders/shader.frag");

	GLfloat vertices[] = {
        -0.5, -0.5, 0.5, 0, 0, 1,
        -0.5, 0.5, 0.5, 0, 0, 1,
        0.5, 0.5, 0.5, 0, 0, 1,

        0.5, 0.5, 0.5, 0, 0, 1,
        0.5, -0.5, 0.5, 0, 0, 1,
        -0.5, -0.5, 0.5, 0, 0, 1,

        -0.5, 0.5, -0.5, 0, 0, -1,
        0.5, 0.5, -0.5, 0, 0, -1,
        -0.5, -0.5, -0.5, 0, 0, -1,

        0.5, 0.5, -0.5, 0, 0, -1,
        -0.5, -0.5, -0.5, 0, 0, -1,
        0.5, -0.5, -0.5, 0, 0, -1,

        -0.5, 0.5, 0.5, 0, 1, 0,
        -0.5, 0.5, -0.5, 0, 1, 0,
        0.5, 0.5, 0.5, 0, 1, 0,

        -0.5, 0.5, -0.5, 0, 1, 0,
        0.5, 0.5, 0.5, 0, 1, 0,
        0.5, 0.5, -0.5, 0, 1, 0,

        0.5, -0.5, -0.5, 0, -1, 0,
        -0.5, -0.5, 0.5, 0, -1, 0,
        -0.5, -0.5, -0.5, 0, -1, 0,

        0.5, -0.5, -0.5, 0, -1, 0,
        -0.5, -0.5, 0.5, 0, -1, 0,
        0.5, -0.5, 0.5, 0, -1, 0,

        0.5, -0.5, 0.5, 1, 0, 0,
        0.5, 0.5, 0.5, 1, 0, 0,
        0.5, 0.5, -0.5, 1, 0, 0,

        0.5, -0.5, 0.5, 1, 0, 0,
        0.5, 0.5, -0.5, 1, 0, 0,
        0.5, -0.5, -0.5, 1, 0, 0,

        -0.5, -0.5, 0.5, -1, 0, 0,
        -0.5, 0.5, 0.5, -1, 0, 0,
        -0.5, 0.5, -0.5, -1, 0, 0,

        -0.5, -0.5, 0.5, -1, 0, 0,
        -0.5, -0.5, -0.5, -1, 0, 0,
        -0.5, 0.5, -0.5, -1, 0, 0
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
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	GLuint VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);//pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat))); //normals
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ourShader.use();


	GLuint model_loc = glGetUniformLocation(ourShader.Program, "model");
	GLuint view_loc = glGetUniformLocation(ourShader.Program, "view");
	GLuint proj_loc = glGetUniformLocation(ourShader.Program, "proj");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);




	GLuint colorLoc = glGetUniformLocation(ourShader.Program, "objColor");
	glm::vec3  obj_color= glm::vec3(1.0f, 0.5f, 0.31f);

	GLuint lightLoc = glGetUniformLocation(ourShader.Program, "lightColor");
	glm::vec3  light_color= glm::vec3(1.0f, 1.0f, 1.0f);

	GLuint lightPosLoc = glGetUniformLocation(ourShader.Program, "lightPos");


	glUniform3fv(colorLoc, 1, glm::value_ptr(obj_color));
	glUniform3fv(lightLoc, 1, glm::value_ptr(light_color));
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    

	GLuint obj_type_mode = glGetUniformLocation(ourShader.Program, "isLight");
 

	glEnable(GL_DEPTH_TEST);
	while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
		do_movement();
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	
		glBindVertexArray(VAO[0]);
		//glDrawArrays(GL_TRIANGLES, 0, 6);

		view = my_cam.getMatrix();
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view) );

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width/(float)height, 0.01f, 100.0f);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj) );


		model = glm::translate(glm::mat4(1.0f), lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model) );
		glUniform1i(obj_type_mode, 1);
		//glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glUniform1i(obj_type_mode, 0);

		for(int i = 10; i--;){
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			if(i % 3 == 0)
				model = glm::rotate(model, (float)glfwGetTime(),  glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glm::radians(20.0f*i),  glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model) );

			//glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);


	glfwTerminate();
	return 0;
}
bool keys[1024] = {false};

void do_movement(){
	GLfloat currentFrame = glfwGetTime();
	static GLfloat lastFrame = 0.0f;
	GLfloat deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if(keys[GLFW_KEY_W])
		my_cam.processKeyboard(Camera_movement::FORWARD, deltaTime);
	if(keys[GLFW_KEY_S])
		my_cam.processKeyboard(Camera_movement::BACKWARD, deltaTime);
	if(keys[GLFW_KEY_D])
		my_cam.processKeyboard(Camera_movement::RIGHT, deltaTime);
	if(keys[GLFW_KEY_A])
		my_cam.processKeyboard(Camera_movement::LEFT, deltaTime);

}

void mouse_callback(GLFWwindow * window, double xpos, double ypos){
	static GLfloat lastX = xpos;
	static GLfloat lastY = ypos;

	GLfloat xoffset = lastX - xpos;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	my_cam.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset){
	my_cam.processMouseScroll(yoffset);
}
void key_callback (GLFWwindow* window, int key, int scancode, int action, int mode){

	if(action == GLFW_PRESS){
		if(key == GLFW_KEY_ESCAPE){
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
	if(action == GLFW_PRESS)
		keys[key] = true;
	if(action == GLFW_RELEASE)
		keys[key] = false;
}


