
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

Camera my_cam(glm::vec3(43.476055, -8.379433, -1.566181));
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

	GLfloat vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

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
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(  -1.0f,  0.0f,  -0.3f),
		glm::vec3(  -2.0f,  0.0f, -2.0f),
		glm::vec3( -1.5f, -2.2f, -2.5f),
		glm::vec3( -3.8f, -2.0f, 3.3f),
		glm::vec3(  2.4f, -0.4f, -3.5f),
		glm::vec3( -1.7f,  3.0f, -7.5f),
		glm::vec3(  1.3f, -2.0f, -2.5f),
		glm::vec3(  1.5f,  2.0f, -2.5f),
		glm::vec3(  1.5f,  0.2f, -1.5f),
		glm::vec3( -1.3f,  1.0f, -1.5f),
		glm::vec3(  1.5f,  4.3f, -2.5f),
		glm::vec3(  2.2f,  0.2f, -1.5f),
		glm::vec3( -1.3f,  -1.0f, -3.5f)
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

	// std::vector<std::string> faces({
	// 	"./textures/skybox/right.jpg",
	// 	"./textures/skybox/left.jpg",
	// 	"./textures/skybox/top.jpg",
	// 	"./textures/skybox/bottom.jpg",
	// 	"./textures/skybox/front.jpg",
	// 	"./textures/skybox/back.jpg"
	// });
	std::vector<std::string> faces({
		"./textures/Yokohama3/posx.jpg",
		"./textures/Yokohama3/negx.jpg",
		"./textures/Yokohama3/posy.jpg",
		"./textures/Yokohama3/negy.jpg",
		"./textures/Yokohama3/posz.jpg",
		"./textures/Yokohama3/negz.jpg"
	});

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

	std::vector <Vertex> vect_vertices;
	for(unsigned int i = 0; i < sizeof(vertices)/sizeof(*vertices); i += 8){
		Vertex vertex;
		vertex.Position = glm::vec3 (vertices[i], vertices[i+1], vertices[i+2]);
		vertex.Normal = glm::vec3 (vertices[i+3], vertices[i+4], vertices[i+5]);
		vertex.TexCoords = glm::vec2 (vertices[i+6], vertices[i+7]);
		vect_vertices.push_back(vertex);
	}
	std::vector <unsigned int> vect_indices;
	for (unsigned int i = 0; i < sizeof(vertices)/sizeof(*vertices)/8; i++){
		vect_indices.push_back(i);
	}
	std::vector <Texture> vect_textures;
	Mesh my_mesh(vect_vertices, vect_indices, vect_textures);

	std:: vector<Model> models;

	models.push_back(Model("3d_models/slim girl/slim girl.obj"));
	models.push_back(Model("./3d_models/nanosuit/nanosuit.obj"));

//	models.push_back(Model("./3D_models/manipulator/Component18.stl"));
	models.push_back(Model("./3D_models/manipulator/Component31.stl"));
	models.push_back(Model("./3D_models/manipulator/Component1_reduce.stl"));
	models.push_back(Model("./3D_models/manipulator/Component129.stl"));
	models.push_back(Model("./3D_models/manipulator/Component26.stl"));
	models.push_back(Model("./3D_models/manipulator/Component19.stl"));
	models.push_back(Model("./3D_models/manipulator/Component21.stl"));
	models.push_back(Model("./3D_models/manipulator/Component20.stl"));
	models.push_back(Model("./3D_models/manipulator/Component40.stl"));
	models.push_back(Model("./3D_models/manipulator/Component5_3.stl"));
	models.push_back(Model("./3D_models/manipulator/Component5_1.stl"));
	models.push_back(Model("./3D_models/manipulator/Component5_2.stl"));

	std::vector<float> rad_vect;
	int ROCK_CNT = 100;
	for(int i = 0; i < ROCK_CNT; i++)
		rad_vect.push_back((float)(std::rand()%200000 + 50000)/1000.0);
	Model rock("./3D_models/rock/rock.obj", rad_vect);



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
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));

			glUniform1i(obj_type_mode, i);
			my_mesh.Draw(ourShader);
		}

		glUniform1i(obj_type_mode, -1);
		ourShader.setMaterial(Material::green_plastic);

		for(unsigned int i = 0; i < models.size(); i++){
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model = glm::rotate(model, (float)glm::radians(20.0f*i),  glm::vec3(0.0f, 1.0f, 0.0f));
			if(i == 0){
				
				model = glm::scale(model, glm::vec3(0.02f));
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
				models[i].Draw(ourShader);				


				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);

				stensilShader.use();
				stensilShader.setFloat(1.0f, "scale");

				// glDisable(GL_DEPTH_TEST);
				models[i].Draw(stensilShader);
				// glEnable(GL_DEPTH_TEST);

				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0x00);

				ourShader.use();

			}else if (i == 1){
				model = glm::scale(model, glm::vec3(0.2f));
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
				models[i].Draw(ourShader);
				model = glm::scale(model, glm::vec3(0.2f));
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
				rock.Draw(ourShader, ROCK_CNT);
				
			}else{
				ourShader.setMaterial(static_cast<Material> (i));

				model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.002f));
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
				models[i].Draw(ourShader);
			}
		}

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0)));

		static int int_time = 0;
		if(round(glfwGetTime()*16)>int_time){
			int_time = round(glfwGetTime()*16);
			column.setMatrixByID(1, column_matrices[int_time%column_matrices.size()]);
		}
		ourShader.setMaterial(Material::yellow_plastic);
		column.Draw(ourShader, 2);
//		column.Draw(ourShader, (int)glfwGetTime());


		ourShader.setMaterial(Material::red_plastic);
//		horizontal_bond.Draw(ourShader, hor_bond_matrices.size());
//		horizontal_bond.Draw(ourShader, (int)(glfwGetTime()*2));


		ourShader.setMaterial(Material::green_plastic);
//		tilted_bond.Draw(ourShader, tilt_bond_matrices.size());
//		tilted_bond.Draw(ourShader, (int)(glfwGetTime()*4));


		model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
		model = glm::translate(model, glm::vec3(0.0, 13*3000.0, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		ourShader.setMaterial(Material::white_plastic);
		main_frame.Draw(ourShader);

		// reflectShader.use();
		// reflectShader.setVec3(my_cam.Position, "cameraPos");
		// model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0, 0.0, 0.0));
		// model = glm::scale(model, glm::vec3(0.02f));
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		// models[0].Draw(reflectShader);

		// refractShader.use();
		// refractShader.setVec3(my_cam.Position, "cameraPos");
		// model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0, 0.0, 0.0));
		// model = glm::rotate(model, (float)glm::radians(20.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
		// model = glm::scale(model, glm::vec3(0.02f));
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		// glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		// models[0].Draw(refractShader);


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

