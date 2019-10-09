
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
GLuint loadTexture(const char * file_path);

Camera my_cam(glm::vec3(0.942951f, -2.274822f, -0.723970f));
bool keys[1024] = {false};

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
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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

	glm::vec3 lightPositions[] = {
		glm::vec3(1.003572f, 1.105885f, -2.825360f),
		glm::vec3(-2.258501f, -0.766950f, -2.328455f),
		glm::vec3(0.698238f, -0.064507f, -4.464025f),
		glm::vec3(-0.820750f, 1.761344f, -3.465433f),
		glm::vec3(-1.309989f, 0.969520f, -7.091900f)
	};

	glm::vec3 lightPos = glm::vec3(-0.5f, 0.0f, -4.0f);

	GLuint VBO[2], VAO[2];
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);//pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat))); //normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat))); //texture
	glEnableVertexAttribArray(2);

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


	ourShader.setMaterial(Material::jade);

//	ourShader.setVec3(glm::vec3(0xD6/255.0f, 0x70/255.0f, 0xD6/255.0f)/50.0f, "spotLight.ambient");
//	ourShader.setVec3(glm::vec3(0xD6/255.0f, 0x70/255.0f, 0xD6/255.0f), "spotLight.diffuse");
//	ourShader.setVec3(glm::vec3(0xD6/255.0f, 0x70/255.0f, 0xD6/255.0f), "spotLight.specular");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/10.0f, "spotLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "spotLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "spotLight.specular");
	ourShader.setFloat(1.0f, "spotLight.constatnt");
	ourShader.setFloat(0.22f, "spotLight.linear");
	ourShader.setFloat(0.2f, "spotLight.quadratic");

	ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
	ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
	ourShader.setFloat(glm::cos(glm::radians(12.5f)), "spotLight.cutOff");
	ourShader.setFloat(glm::cos(glm::radians(17.5f)), "spotLight.outerCutOff");

	ourShader.setVec3(glm::vec3(0x00/255.0f, 0x00/255.0f, 0x00/255.0f)/10.0f, "dirLight.ambient");
	ourShader.setVec3(glm::vec3(0x00/255.0f, 0x00/255.0f, 0x00/255.0f), "dirLight.diffuse");
	ourShader.setVec3(glm::vec3(0x00/255.0f, 0x00/255.0f, 0x00/255.0f), "dirLight.specular");
	ourShader.setVec4(glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f), "dirLight.direction");


	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0x0F/255.0f)/10.0f, "pointLights[0].ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0x0F/255.0f), "pointLights[0].diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0x0F/255.0f), "pointLights[0].specular");
	ourShader.setFloat(1.0f, "pointLights[0].constatnt");
	ourShader.setFloat(0.22f, "pointLights[0].linear");
	ourShader.setFloat(0.2f, "pointLights[0].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[0], 1.0f), "pointLights[0].position");

	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/10.0f, "pointLights[1].ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "pointLights[1].diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "pointLights[1].specular");
	ourShader.setFloat(1.0f, "pointLights[1].constatnt");
	ourShader.setFloat(0.22f, "pointLights[1].linear");
	ourShader.setFloat(0.2f, "pointLights[1].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[1], 1.0f), "pointLights[1].position");

	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/10.0f, "pointLights[2].ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "pointLights[2].diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "pointLights[2].specular");
	ourShader.setFloat(1.0f, "pointLights[2].constatnt");
	ourShader.setFloat(0.22f, "pointLights[2].linear");
	ourShader.setFloat(0.2f, "pointLights[2].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[2], 1.0f), "pointLights[2].position");

	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0x0F/255.0f, 0xFF/255.0f)/10.0f, "pointLights[3].ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0x0F/255.0f, 0xFF/255.0f), "pointLights[3].diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0x0F/255.0f, 0xFF/255.0f), "pointLights[3].specular");
	ourShader.setFloat(1.0f, "pointLights[3].constatnt");
	ourShader.setFloat(0.22f, "pointLights[3].linear");
	ourShader.setFloat(0.2f, "pointLights[3].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[3], 1.0f), "pointLights[3].position");


	GLuint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");


	

	GLuint obj_type_mode = glGetUniformLocation(ourShader.Program, "isLight");
 
	float lightrad = glm::length(lightPos);

	glEnable(GL_DEPTH_TEST);

	GLuint texture[] = {
		loadTexture("./textures/container2.png"),
		loadTexture("./textures/container2_specular.png"),
		loadTexture("./textures/matrix.jpg"),
		loadTexture("./textures/lighting_maps_specular_color.png"),
		loadTexture("./textures/awesomeface.png"),
		loadTexture("./textures/container.jpg"),
	};

	glUniform1i(glGetUniformLocation(ourShader.Program, "material.diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(ourShader.Program, "material.specularTex"), 1);
	glUniform1i(glGetUniformLocation(ourShader.Program, "material.emissionTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture[1]);	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture[2]);	


	GLfloat timestamp = glfwGetTime();
	int time_cnt = 0;
	while(!glfwWindowShouldClose(window)){
		time_cnt ++;
		if(time_cnt % 1000 == 0){
//			std::cout<<"FPS: "<<1000.0/(glfwGetTime() - timestamp)<<std::endl;
			timestamp = glfwGetTime();
			time_cnt = 0;
		}
		glfwPollEvents();
		do_movement();
//		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));
		
//		lightPos = glm::vec3( sin(glfwGetTime())*lightrad, lightPos.y, cos(glfwGetTime())*lightrad);
//		ourShader.setVec4(glm::vec4(lightPos, 1.0f), "light.position");

		if(keys[GLFW_KEY_B]){
			ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
			ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
		}

		//glDrawArrays(GL_TRIANGLES, 0, 6);

		view = my_cam.getMatrix();
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view) );

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width/(float)height, 0.01f, 100.0f);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj) );

		for(int i = 0; i < 4; i++){
			glm::vec3 lightpos = lightPositions[i];
			float t = glfwGetTime()/(i+1);
			lightpos.x += glm::sin(sin(7*t)+sin(i*t*0.5f));
			lightpos.y += glm::cos(5*t)-cos(2*t)+-cos(i*t);
			lightpos.z += glm::cos(t*3.7f/(i*i+1))*glm::sin(t*4.2f/(i+1));

			std::string uniformname = "pointLights["+std::to_string(i)+"].position";
			ourShader.setVec4(	glm::vec4(lightpos, 1.0f), uniformname.c_str());

			model = glm::translate(glm::mat4(1.0f), lightpos);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model) );
			glUniform1i(obj_type_mode, i);

			glBindVertexArray(VAO[0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glUniform1i(obj_type_mode, -1);

		ourShader.setMaterial(Material::green_plastic);

		for(int i = 10; i--;){
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			if(i % 4 == 0)
				model = glm::translate(model, glm::vec3(glm::sin(glfwGetTime()*3), 0.0f, 0.0f));
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
	if(action == GLFW_RELEASE){
		keys[key] = false;
		if(key == GLFW_KEY_B){
			std::cout<<glm::to_string(my_cam.Position)<<std::endl;
		}
	}
}

GLuint loadTexture(const char * file_path){
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


