
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
#include "mesh.h"
#include "model.h"

#include "load_tex.h"

void do_movement();
void key_callback (GLFWwindow*,int,int,int,int);
void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
unsigned int loadCubeMap(std::vector<std::string> faces);

Camera my_cam(glm::vec3(0.0, 0.0, 0.0));
bool keys[1024] = {false};

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);


#ifdef FULL_SCREEN
	GLFWmonitor * monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * mode = glfwGetVideoMode(monitor);
	GLFWwindow * window = glfwCreateWindow(mode->width, mode->height, "OpenGL Learning", monitor, nullptr);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#else
	GLFWwindow * window = glfwCreateWindow(800, 600, "OpenGL Learning", nullptr, nullptr);
#endif

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
  	Shader stensilShader( "./shaders/shader.vert", "./shaders/stencil.frag"); 
	Shader screenShader("./shaders/screen.vert", "./shaders/screen.frag");
	Shader skyboxShader("./shaders/skybox.vert", "./shaders/skybox.frag");
	Shader reflectShader("./shaders/reflect.vert", "./shaders/reflect.frag");
	Shader refractShader("./shaders/refract.vert", "./shaders/refract.frag");

	GLuint uboTransform;
	glGenBuffers(1, &uboTransform);
	glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*3, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboTransform);


	GLfloat screen_vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f
	};


	glm::vec3 lightPositions[] = {
		glm::vec3(1.003572f, 1.105885f, -2.825360f),
		glm::vec3(-2.258501f, -0.766950f, -2.328455f),
		glm::vec3(0.698238f, -0.064507f, -4.464025f),
		glm::vec3(-0.820750f, 1.761344f, -3.465433f),
		glm::vec3(-1.309989f, 0.969520f, -7.091900f)
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	std::vector<std::string> faces({
		"./textures/skybox/right.jpg",
		"./textures/skybox/left.jpg",
		"./textures/skybox/top.jpg",
		"./textures/skybox/bottom.jpg",
		"./textures/skybox/front.jpg",
		"./textures/skybox/back.jpg"
	});
	// std::vector<std::string> faces({
	// 	"./textures/Yokohama3/posx.jpg",
	// 	"./textures/Yokohama3/negx.jpg",
	// 	"./textures/Yokohama3/posy.jpg",
	// 	"./textures/Yokohama3/negy.jpg",
	// 	"./textures/Yokohama3/posz.jpg",
	// 	"./textures/Yokohama3/negz.jpg"
	// });

	unsigned int cubemapTexture = loadCubeMap(faces);

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glBindVertexArray(0);


	GLuint screen_vao, screen_vbo;
	glGenVertexArrays(1, &screen_vao);
	glGenBuffers(1, &screen_vbo);
	glBindVertexArray(screen_vao);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glBindVertexArray(0);
	
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint textureColorBuffer; 
//	textureColorBuffer = loadTexture("./textures/container.jpg");
	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
	
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout<<"ERROR::FRAMEBUFFER:: framebuffer is not complete"<< std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	ourShader.use();



	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);



	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/10.0f, "spotLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "spotLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "spotLight.specular");
	ourShader.setFloat(1.0f, "spotLight.constatnt");
	ourShader.setFloat(0.22f, "spotLight.linear");
	ourShader.setFloat(0.2f, "spotLight.quadratic");

	ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
	ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
	ourShader.setFloat(glm::cos(glm::radians(90.0f)), "spotLight.cutOff");
	ourShader.setFloat(glm::cos(glm::radians(100.0f)), "spotLight.outerCutOff");

	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/1.0f, "dirLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "dirLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "dirLight.specular");
	ourShader.setVec4(glm::vec4(1.0f, -1.0f, 0.0f, 0.0f), "dirLight.direction");


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



	Model tower_frame("./3D_models/manipulator/Component18.stl", 3);//tower frame
	Model tower_box("./3D_models/manipulator/Component31.stl", 3);//tower box
	Model carrige("./3D_models/manipulator/Component1_reduce.stl", 3); // carrige
	Model rail("./3D_models/manipulator/Component129.stl", 3);//rail
	Model bond_wrist("./3D_models/manipulator/Component26.stl", 3);//bond wrist
	Model bond_carrige("./3D_models/manipulator/Component19_orient.stl", 3);//bond carrige
	Model bond_rail("./3D_models/manipulator/Component21_orient.stl", 3);//bond rail
	Model column_rail("./3D_models/manipulator/Component20.stl", 3);//column rail
	Model column_carrige("./3D_models/manipulator/Component40.stl", 3);//column carrige
	Model bond_handler_middle("./3D_models/manipulator/Component5_3.stl", 3);//bond handler middle
	Model bond_handlre_left("./3D_models/manipulator/Component5_1.stl", 3);//bond handlre left
	Model bond_handler_right("./3D_models/manipulator/Component5_2.stl", 3);//bond handler right



	glEnable(GL_MULTISAMPLE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_CULL_FACE);


	GLfloat timestamp = glfwGetTime();
	int time_cnt = 0;

	glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
	float frame_level = 13*3000.0;
	float max_bcar_level = 1500.0;
	float max_ccar_level = 350.0;
	struct ManipulatorConfig{
		float rail;
		float tower;
		float bpant;
		float cpant;
		float bcar;
		float ccar;
		float wrist;
		float brot;
	};
	struct ManipulatorConfig m1, m2, m3;
	m1.rail = 10000.0;
	m1.tower = 30.0;
	m1.bpant = 3000.0;
	m1.cpant = 2000.0;
	m1.bcar = 1000.0;
	m1.ccar = 500.0;
	m1.wrist = 20.0;
	m1.brot = 60.0;

	m2.rail = 5000.0;
	m2.tower = 30.0;
	m2.bpant = 0.0;
	m2.cpant = 0.0;
	m2.bcar = 0.0;
	m2.ccar = 0.0;
	m2.wrist = 0.0;
	m2.brot = 0.0;

	m3.rail = 12000.0;
	m3.tower = 30.0;
	m3.bpant = 3000.0;
	m3.cpant = 2000.0;
	m3.bcar = 1000.0;
	m3.ccar = 500.0;
	m3.wrist = 20.0;
	m3.brot = 60.0;


	glm::mat4 mat_world = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
	mat_world = glm::rotate(mat_world, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 mat_A  = glm::translate(mat_world, glm::vec3(0.0, 0.0, frame_level));
	//rail position on frame
	glm::mat4 mat_B1 = glm::translate(mat_A, glm::vec3(2100.0, 6760.0, -400.0));
	glm::mat4 mat_B2 = glm::translate(mat_A, glm::vec3(200.0, -245.0, -400.0));
	glm::mat4 mat_B3 = glm::translate(mat_A, glm::vec3(2100.0, -7250.0, -400.0));

	glm::mat4 mat_C1 = glm::translate(mat_B1, glm::vec3(m1.rail, 0.0, -400.0));
	glm::mat4 mat_C2 = glm::translate(mat_B2, glm::vec3(m2.rail, 0.0, -400.0));
	glm::mat4 mat_C3 = glm::translate(mat_B3, glm::vec3(m3.rail, 0.0, -400.0));
	glm::mat4 mat_D1 = glm::rotate(mat_C1, glm::radians(m1.tower), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_D2 = glm::rotate(mat_C2, glm::radians(m2.tower), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_D3 = glm::rotate(mat_C3, glm::radians(m3.tower), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_E1 = glm::translate(glm::rotate(mat_D1, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
										glm::vec3(m1.bpant, 0.0f, 0.0f));
	glm::mat4 mat_E2 = glm::translate(mat_D1, glm::vec3(m1.cpant, 0.0f, 0.0f));
	glm::mat4 mat_E3 = glm::translate(glm::rotate(mat_D2, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
										glm::vec3(m2.bpant, 0.0f, 0.0f));
	glm::mat4 mat_E4 = glm::translate(mat_D2, glm::vec3(m2.cpant, 0.0f, 0.0f));
	glm::mat4 mat_E5 = glm::translate(glm::rotate(mat_D3, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
										glm::vec3(m2.bpant, 0.0f, 0.0f));
	glm::mat4 mat_E6 = glm::translate(mat_D3, glm::vec3(m3.cpant, 0.0f, 0.0f));

	glm::mat4 mat_F1 = glm::translate(mat_E1, glm::vec3(627.0, 0.0, -max_bcar_level + m1.bcar));
	glm::mat4 mat_F2 = glm::translate(mat_E2, glm::vec3(0.0, 0.0, -max_ccar_level + m1.ccar));
	glm::mat4 mat_F3 = glm::translate(mat_E3, glm::vec3(627.0, 0.0, -max_bcar_level + m2.bcar));
	glm::mat4 mat_F4 = glm::translate(mat_E4, glm::vec3(0.0, 0.0, -max_ccar_level + m2.ccar));
	glm::mat4 mat_F5 = glm::translate(mat_E5, glm::vec3(627.0, 0.0, -max_bcar_level + m3.bcar));
	glm::mat4 mat_F6 = glm::translate(mat_E6, glm::vec3(0.0, 0.0, -max_ccar_level + m3.ccar));
	glm::mat4 mat_G1 = glm::rotate(mat_F1, glm::radians(m1.wrist), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_G2 = glm::rotate(glm::translate(mat_F2, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_G3 = glm::rotate(mat_F3, glm::radians(m1.wrist), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_G4 = glm::rotate(glm::translate(mat_F4, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_G5 = glm::rotate(mat_F5, glm::radians(m1.wrist), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_G6 = glm::rotate(glm::translate(mat_F6, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 mat_H1 = mat_world;
	glm::mat4 mat_H2 = mat_world;
	glm::mat4 mat_H3 = mat_world;
	glm::mat4 mat_I1 = mat_world;
	glm::mat4 mat_I2 = mat_world;
	glm::mat4 mat_I3 = mat_world;

	float stride = 3.514e+03;
	glm::vec3 column_offset = glm::vec3(-5.610e+03, 507.50f, 5.033e+03); 
	glm::vec3 hor_bond_offset = glm::vec3(0, 127.0f, -550.0); 
	glm::vec3 tilt_bond_offset = glm::vec3(0, 600.0f, -465.0); 

	std::vector<glm::mat4> column_matrices;
	std::vector<glm::mat4> hor_bond_matrices;
	std::vector<glm::mat4> tilt_bond_matrices;

	for(int floor = -13; floor < 13; floor++)
		for(int x = 0; x < 4; x++)
			for(int y = 0; y< 4; y++){


				if(y < 3){
					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset - hor_bond_offset);
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::red_plastic);
					// horizontal_bond.Draw(ourShader);
					hor_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, -tilt_bond_offset);
					model = glm::rotate(model, glm::radians(63.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::green_plastic);
					// tilted_bond.Draw(ourShader);
					tilt_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, glm::vec3(0.0, 0.0, 2*tilt_bond_offset.z + stride) );
					model = glm::translate(model, -tilt_bond_offset );
					model = glm::rotate(model, glm::radians(117.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::green_plastic);
					// tilted_bond.Draw(ourShader);
					tilt_bond_matrices.push_back(model);
				}

				if(x < 3){
					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -hor_bond_offset);
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::red_plastic);
					// horizontal_bond.Draw(ourShader);
					hor_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -tilt_bond_offset);
					model = glm::rotate(model, glm::radians(63.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));

					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::green_plastic);
					// tilted_bond.Draw(ourShader);
					tilt_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, glm::vec3(2*tilt_bond_offset.z + stride, 0.0, 0.0) );
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -tilt_bond_offset );
					model = glm::rotate(model, glm::radians(117.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));

					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::green_plastic);
					// tilted_bond.Draw(ourShader);
					tilt_bond_matrices.push_back(model);
				}


				model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
				model = glm::translate(model, glm::vec3(x*stride, -(floor+1)*3000.0, y*stride) - column_offset);
				model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
				column_matrices.push_back(model);
		//		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		//		ourShader.setMaterial(Material::yellow_plastic);
			//	column.Draw(ourShader);
			}

	Model tilted_bond("./3d_models/stl_components/tilted_bond.stl", tilt_bond_matrices);
	Model horizontal_bond("./3d_models/stl_components/horizontal_bond.stl", hor_bond_matrices);
	Model column("./3d_models/stl_components/column_light.stl", column_matrices);
	Model main_frame("./3d_models/stl_components/main_frame.stl");

	while(!glfwWindowShouldClose(window)){
		time_cnt ++;
		if(time_cnt % 1000 == 0){
			std::cout<<"FPS: "<<1000.0/(glfwGetTime() - timestamp)<<std::endl;
			timestamp = glfwGetTime();
			time_cnt = 0;
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ourShader.use();
		glEnable(GL_DEPTH_TEST);
		glfwPollEvents();
		do_movement();
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));
		if(keys[GLFW_KEY_B]){
			ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
			ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
		}
		view = my_cam.getMatrix();
		ourShader.setMat4(view, "view");
		ourShader.setFloat(glfwGetTime(), "time");

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width/(float)height, 0.01f, 200.0f);
		ourShader.setMat4(proj, "proj");

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);



		ourShader.use();

		glUniform1i(obj_type_mode, -1);
		ourShader.setMaterial(Material::green_plastic);

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0)));

		static int int_time = 0;
		if(round(glfwGetTime()*16)>int_time){
			int_time = round(glfwGetTime()*16);
			column.setMatrixByID(1, mat_G2);
			column.setMatrixByID(2, mat_G4);
			column.setMatrixByID(3, mat_G6);
//			column.setMatrixByID(1, column_matrices[int_time%column_matrices.size()]);
		}
		ourShader.setMaterial(Material::white_rubber);
		column.Draw(ourShader, column_matrices.size());
//		column.Draw(ourShader, (int)glfwGetTime());


		ourShader.setMaterial(Material::red_plastic);
		horizontal_bond.Draw(ourShader, hor_bond_matrices.size());
//		horizontal_bond.Draw(ourShader, (int)(glfwGetTime()*2));


		ourShader.setMaterial(Material::green_plastic);
		tilted_bond.Draw(ourShader, tilt_bond_matrices.size());
//		tilted_bond.Draw(ourShader, (int)(glfwGetTime()*4));


		ourShader.setMaterial(Material::chrome);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_A));
		main_frame.Draw(ourShader);

		// ourShader.setMaterial(Material::yellow_plastic);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B1));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B2));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B3));
		// rail.Draw(ourShader);


		ourShader.setMaterial(Material::green_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_C1));
		carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_C2));
		carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_C3));
		carrige.Draw(ourShader);

		ourShader.setMaterial(Material::yellow_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D1));
		tower_frame.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D2));
		tower_frame.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D3));
		tower_frame.Draw(ourShader);

		ourShader.setMaterial(Material::white_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D1));
		tower_box.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D2));
		tower_box.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_D3));
		tower_box.Draw(ourShader);

		ourShader.setMaterial(Material::green_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E1));
		bond_rail.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E3));
		bond_rail.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E5));
		bond_rail.Draw(ourShader);

		ourShader.setMaterial(Material::red_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F1));
		bond_carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F3));
		bond_carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F5));
		bond_carrige.Draw(ourShader);

		ourShader.setMaterial(Material::green_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_G1));
		bond_wrist.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_G3));
		bond_wrist.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_G5));
		bond_wrist.Draw(ourShader);		


		ourShader.setMaterial(Material::green_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E2));
		column_rail.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E4));
		column_rail.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_E6));
		column_rail.Draw(ourShader);

		ourShader.setMaterial(Material::red_plastic);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F2));
		column_carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F4));
		column_carrige.Draw(ourShader);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_F6));
		column_carrige.Draw(ourShader);



		skyboxShader.use();
		skyboxShader.setMat4(proj, "proj");
		skyboxShader.setMat4(glm::mat4(glm::mat3(view)), "view");
		glDepthMask(GL_FALSE);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);


		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// screenShader.use();
		// glDisable(GL_DEPTH_TEST);
		// glClear(GL_COLOR_BUFFER_BIT);
		// glBindVertexArray(screen_vao);
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		// glDrawArrays(GL_TRIANGLES, 0, 6);
		// glBindVertexArray(0);
	
		glfwSwapBuffers(window);

	}
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

unsigned int loadCubeMap(std::vector<std::string> faces){
	GLuint cubeTextureID;
	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);

	int width, height, nrChannels;
	for(unsigned int i = 0; i < faces.size(); i++){
		unsigned char * data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if(data){
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}else{
			std::cout<<"Cubemap texture failed to load at path: "<<faces[i]<<std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubeTextureID;
}

