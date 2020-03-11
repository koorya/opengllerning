#define GLM_ENABLE_EXPERIMENTAL
//#define FULL_SCREEN

//#include <GL/glew.h>
#include <glad/glad.h>
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

#include "manipulator.h"
#include "remote_manipulator.h"
#include "GUI_Manipulator.h"
#include "ConstructionContainer.h"
#include "GUI_MainFrame.h"
#include "cassete.h"

#include <nanogui/nanogui.h>

#include "cl_kernel_container.h"


void do_movement();
void key_callback(GLFWwindow *, int, int, int, int);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadCubeMap(std::vector<std::string> faces);

void init_open_cl(cl_context &context, cl_command_queue &command_queue, cl_kernel &kernel);

Camera my_cam(glm::vec3(0.0, 0.0, 0.0));
bool keys[1024] = {false};

using namespace nanogui;

Screen *screen = nullptr;
bool print_vert = false;

int main(int argc, char * argv[])
{


	glfwInit();



	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	bool FULL_SCREEN = false;
	int mon_numb = 0;
	if(argc>1){
		if(std::string(argv[1])=="--fullscreen")
			 FULL_SCREEN = true;
		if(argc>2){
			mon_numb = std::stoi(std::string(argv[2]));
		}
	}
	GLFWwindow *window;
	if(FULL_SCREEN){
		int mon_cnt;
		GLFWmonitor **monitor = glfwGetMonitors(&mon_cnt);
		if(mon_cnt <= mon_numb)
			mon_numb = 0;
		const GLFWvidmode *mode = glfwGetVideoMode(monitor[mon_numb]);
		window = glfwCreateWindow(mode->width, mode->height, "OpenGL Learning", monitor[mon_numb], nullptr);
		glfwSetWindowMonitor(window, monitor[mon_numb], 0, 0, mode->width, mode->height, mode->refreshRate);
	}else{
		window = glfwCreateWindow(1200, 800, "Демо манипулятора с ручным управлением", nullptr, nullptr);
	}
	glfwShowWindow(window);


	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	
	clKernelsContainer my_cl_cont = clKernelsContainer();



	screen = new Screen();

	screen->initialize(window, true);


//	glfwSetKeyCallback(window, key_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetMouseButtonCallback(window, MouseButtonCallback);

//	glfwSetScrollCallback(window, scroll_callback);

    glfwSetCursorPosCallback(window,
            [](GLFWwindow *, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);
			mouse_callback(NULL, x, y);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *, int button, int action, int modifiers) {
            if(!screen->mouseButtonCallbackEvent(button, action, modifiers) || 
				(action == GLFW_RELEASE))
				MouseButtonCallback(NULL, button, action, modifiers);
			
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow *, int key, int scancode, int action, int mods) {
            if(!screen->keyCallbackEvent(key, scancode, action, mods))
				key_callback(NULL, key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow *, unsigned int codepoint) {
            screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow *, int count, const char **filenames) {
            screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow *, double x, double y) {
            screen->scrollCallbackEvent(x, y);
			scroll_callback(NULL, x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow *, int width, int height) {
            screen->resizeCallbackEvent(width, height);
        }
    );






	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);



	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	Shader ourShader("../shaders/shader.vert", "../shaders/shader.frag");
	Shader stensilShader("../shaders/shader.vert", "../shaders/stencil.frag");
//	Shader screenShader("../shaders/screen.vert", "../shaders/screen.frag");
	Shader skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag");
	Shader reflectShader("../shaders/reflect.vert", "../shaders/reflect.frag");
	Shader refractShader("../shaders/refract.vert", "../shaders/refract.frag");

	GLuint uboTransform;
	glGenBuffers(1, &uboTransform);
	glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboTransform);

	GLfloat screen_vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f};

	glm::vec3 lightPositions[] = {
		glm::vec3(1.003572f, 1.105885f, -2.825360f),
		glm::vec3(-2.258501f, -0.766950f, -2.328455f),
		glm::vec3(0.698238f, -0.064507f, -4.464025f),
		glm::vec3(-0.820750f, 1.761344f, -3.465433f),
		glm::vec3(-1.309989f, 0.969520f, -7.091900f)};

	float skyboxVertices[] = {
		// positions
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f};

	std::vector<std::string> faces({"../textures/skybox/right.jpg",
									"../textures/skybox/left.jpg",
									"../textures/skybox/top.jpg",
									"../textures/skybox/bottom.jpg",
									"../textures/skybox/front.jpg",
									"../textures/skybox/back.jpg"});
	// std::vector<std::string> faces({
	// 	"../textures/Yokohama3/posx.jpg",
	// 	"../textures/Yokohama3/negx.jpg",
	// 	"../textures/Yokohama3/posy.jpg",
	// 	"../textures/Yokohama3/negy.jpg",
	// 	"../textures/Yokohama3/posz.jpg",
	// 	"../textures/Yokohama3/negz.jpg"
	// });

	unsigned int cubemapTexture = loadCubeMap(faces);

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glBindVertexArray(0);

	GLuint screen_vao, screen_vbo;
	glGenVertexArrays(1, &screen_vao);
	glGenBuffers(1, &screen_vbo);
	glBindVertexArray(screen_vao);
	glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glBindVertexArray(0);

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint textureColorBuffer;
	//	textureColorBuffer = loadTexture("../textures/container.jpg");
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ourShader.use();

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mat_world = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
   	mat_world = glm::rotate(mat_world, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
//		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_world)); //model to world transform
	model = mat_world;

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f) / 10.0f, "spotLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "spotLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "spotLight.specular");
	ourShader.setFloat(1.0f, "spotLight.constatnt");
	ourShader.setFloat(0.22f, "spotLight.linear");
	ourShader.setFloat(0.2f, "spotLight.quadratic");

	ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
	ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
	ourShader.setFloat(glm::cos(glm::radians(90.0f)), "spotLight.cutOff");
	ourShader.setFloat(glm::cos(glm::radians(100.0f)), "spotLight.outerCutOff");

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f) / 1.0f, "dirLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight.specular");
	ourShader.setVec4(glm::vec4(1.0f, -1.0f, 0.0f, 0.0f), "dirLight.direction");

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0x0F / 255.0f) / 10.0f, "pointLights[0].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0x0F / 255.0f), "pointLights[0].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0x0F / 255.0f), "pointLights[0].specular");
	ourShader.setFloat(1.0f, "pointLights[0].constatnt");
	ourShader.setFloat(0.22f, "pointLights[0].linear");
	ourShader.setFloat(0.2f, "pointLights[0].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[0], 1.0f), "pointLights[0].position");

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f) / 10.0f, "pointLights[1].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "pointLights[1].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "pointLights[1].specular");
	ourShader.setFloat(1.0f, "pointLights[1].constatnt");
	ourShader.setFloat(0.22f, "pointLights[1].linear");
	ourShader.setFloat(0.2f, "pointLights[1].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[1], 1.0f), "pointLights[1].position");

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f) / 10.0f, "pointLights[2].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "pointLights[2].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "pointLights[2].specular");
	ourShader.setFloat(1.0f, "pointLights[2].constatnt");
	ourShader.setFloat(0.22f, "pointLights[2].linear");
	ourShader.setFloat(0.2f, "pointLights[2].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[2], 1.0f), "pointLights[2].position");

	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0x0F / 255.0f, 0xFF / 255.0f) / 10.0f, "pointLights[3].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0x0F / 255.0f, 0xFF / 255.0f), "pointLights[3].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0x0F / 255.0f, 0xFF / 255.0f), "pointLights[3].specular");
	ourShader.setFloat(1.0f, "pointLights[3].constatnt");
	ourShader.setFloat(0.22f, "pointLights[3].linear");
	ourShader.setFloat(0.2f, "pointLights[3].quadratic");
	ourShader.setVec4(glm::vec4(lightPositions[3], 1.0f), "pointLights[3].position");

	GLuint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");
	GLuint obj_type_mode = glGetUniformLocation(ourShader.Program, "isLight");


	glfwMakeContextCurrent(window);

	Model girl = Model("../3D_models/slim girl/slim girl.obj", 1);
	Model nanosuit = Model("../3D_models/nanosuit/nanosuit.obj", 1);
	
	

//	Model main_frame("../3D_models/stl_components/main_frame.stl", 1);
//	Model main_frame("../3D_models/stend_mm_inventor_blender.obj", 1);
	Model my_sphere("../3D_models/sphere.obj", 1);
	Model my_dr_cube("../3D_models/cube_cil.obj", 1);
//	print_vert = true;
	Model my_cube("../3D_models/cube_cil.obj", 1, &my_cl_cont);
//	print_vert = false;
	 Model main_frame_cl("../3D_models/пустой стенд.stl", 1, &my_cl_cont);
	 Model main_frame_gl("../3D_models/пустой стенд.obj", 1);
//	Model main_frame("../3D_models/obj/manipulator/Pc2.obj", 1, &my_cl_cont);
	Model cassete_cl("../3D_models/пустая кассета.stl", 1, &my_cl_cont);
	Model cassete_gl("../3D_models/пустая кассета.obj", 1);

//	Model cassete("../3D_models/obj/manipulator/Pc1.obj", 1);
	Model tower_frame("../3D_models/obj/manipulator/Component18.obj", 3);				 //tower frame
	
	Model tower_box("../3D_models/obj/manipulator/Component31.obj", 3, &my_cl_cont);					 //tower box
	Model carrige("../3D_models/obj/manipulator/carrige.obj", 3);				 // carrige
	Model rail("../3D_models/obj/manipulator/Component129.obj", 3);						 //rail
	Model bond_wrist("../3D_models/obj/manipulator/Wrist.obj", 3);						 //bond wrist
	Model bond_carrige("../3D_models/obj/manipulator/bcar.obj", 3);						 //bond carrige
	Model bond_rail("../3D_models/obj/manipulator/Component21 orient.obj", 3);			 //bond rail
	Model column_rail("../3D_models/obj/manipulator/Component20.obj", 3);				 //column rail
	Model column_carrige("../3D_models/obj/manipulator/Component40.obj", 3);				 //column carrige
	// Model bond_handler_middle("../3D_models/obj/manipulator/Bond Handler Middle.obj", 3); //bond handler middle
	// Model bond_handler_left("../3D_models/obj/manipulator/Bond Handler Left.obj", 3);	 //bond handlre left
	// Model bond_handler_right("../3D_models/obj/manipulator/Bond Handler Right.obj", 3);   //bond handler right
	Model bond_handler_middle("../3D_models/obj/manipulator/zaklepochnik.obj", 3, &my_cl_cont);   //bond handler right
//	Model bond_handler_middle("../3D_models/cube_cil.obj", 3, context, kernel);   //bond handler right

	Model pb1("../3D_models/obj/manipulator/Pb1.obj", 3); //bond pantograph section 1
	Model pb2("../3D_models/obj/manipulator/Pb2.obj", 3); //bond pantograph section 2
	Model pb3("../3D_models/obj/manipulator/Pb3.obj", 3); //bond pantograph section 3
	Model pb4("../3D_models/obj/manipulator/Pb4.obj", 3); //bond pantograph section 4

	Model pc1("../3D_models/obj/manipulator/Pc1.obj", 3); //column pantograph section 1
	Model pc2("../3D_models/obj/manipulator/Pc2.obj", 3); //column pantograph section 2





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

	fillUpSPProgramsArray();

	BotManipulator l_m = BotManipulator(&glfwGetTime);
	BotManipulator m_m = BotManipulator(&glfwGetTime);
	BotManipulator r_m = BotManipulator(&glfwGetTime);
	BondLocation bl;
	for (int i = 1; i <= 4; i++)
	{

		bl.section = i;
		if (i > 1)
		{
			bl.name = 11;
			m_m.bond_list.push_back(bl);
			bl.name = 10;
			m_m.bond_list.push_back(bl);
			bl.name = 9;
			m_m.bond_list.push_back(bl);
			bl.name = 7;
			m_m.bond_list.push_back(bl);
			bl.name = 8;
			m_m.bond_list.push_back(bl);
			bl.name = 6;
			m_m.bond_list.push_back(bl);
		}
		bl.name = 5;
		m_m.bond_list.push_back(bl);
		bl.name = 4;
		m_m.bond_list.push_back(bl);
		bl.name = 3;
		m_m.bond_list.push_back(bl);
		bl.name = 2;
		m_m.bond_list.push_back(bl);
		bl.name = 1;
		m_m.bond_list.push_back(bl);
	}

	for (int i = 1; i <= 4; i++)
	{
		bl.section = i;
		if (i > 1)
		{
			bl.name = 11;
			l_m.bond_list.push_back(bl);
			bl.name = 9;
			l_m.bond_list.push_back(bl);
			bl.name = 8;
			l_m.bond_list.push_back(bl);
		}
		bl.name = 5;
		l_m.bond_list.push_back(bl);
		bl.name = 13;
		l_m.bond_list.push_back(bl);
	}

	for (int i = 1; i <= 4; i++)
	{
		bl.section = i;
		if (i > 1)
		{
			bl.name = 10;
			r_m.bond_list.push_back(bl);
			bl.name = 7;
			r_m.bond_list.push_back(bl);
			bl.name = 6;
			r_m.bond_list.push_back(bl);
		}
		bl.name = 4;
		r_m.bond_list.push_back(bl);
		bl.name = 12;
		r_m.bond_list.push_back(bl);
	}



	//  RemoteManipulator remote_man = RemoteManipulator(1);
//	  RemoteManipulator remote_man1 = RemoteManipulator(2);
//	  RemoteManipulator remote_man2 = RemoteManipulator(3);




	guiManipulator gui_man = guiManipulator(screen);
	
//	guiManipulator gui_man1 = guiManipulator();
//	guiManipulator gui_man2 = guiManipulator();


	glfwSetWindowPos(window, 50, 100);

//	 glfwSetWindowPos(gui_man.glfwWindow(), 900, 50);
	// glfwSetWindowPos(gui_man1.glfwWindow(), 1200, 50);
	// glfwSetWindowPos(gui_man2.glfwWindow(), 1500, 50);

	Manipulator *m_mat[3] = {&gui_man, NULL, NULL};
//	Manipulator *m_mat[3] = {&remote_man, &remote_man1, &remote_man2};
	m_mat[0]->config.rail.value = 3000.0;
//	m_mat[2]->config.rail.value = 6000.0;

	glfwMakeContextCurrent(window);
	ConstructionContainer constr_container = ConstructionContainer(&my_cl_cont);

	m_mat[0]->container = &constr_container;
//	m_mat[1]->container = &constr_container;
//	m_mat[2]->container = &constr_container;

//	guiMainFrame f_mat;

	MainFrame f_mat;

	GUICassete my_cassete(screen, &constr_container);
//	my_cassete.fillUp();
	my_cassete.updateMatrices(&(f_mat.rail2));
	m_mat[0]->cassete = &my_cassete;
//	m_mat[1]->cassete = &my_cassete;
//	m_mat[2]->cassete = &my_cassete;

	bool trig = false;

	my_cam.Direction = glm::vec3(0.755312, -0.197657, -0.624849);
	my_cam.Position = glm::vec3(-6.934844, -1.400352, 6.606244);

	std::chrono::seconds time(1);

	bool clflag = false;

	cl_float3 my_cl_origin = {1.0f, -1500.0f, 0.0f};
	cl_float3 my_cl_dir = {0.7f, 0.06f, -0.2f};
	float cl_t = 0;
	glm::mat4 sphere_mat = glm::scale(f_mat.World, glm::vec3(1.0f));
	my_sphere.setMatrixByID(0, sphere_mat);
	my_dr_cube.setMatrixByID(0, f_mat.World);

//	GUIWindow ray_gui(screen);

    using namespace nanogui;

    Window *ray_gui_window = new Window(screen, "ray");
    ray_gui_window->setPosition(Vector2i(15, 15));
    ray_gui_window->setLayout(new GroupLayout());

	nanogui::TextBox * textBox = new TextBox(ray_gui_window);

    textBox->setEditable(false);
    textBox->setDefaultValue("0.0");

    textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");

    textBox->setFixedSize(Vector2i(90, 25));
    textBox->setValue("0");
    textBox->setUnits("mm");


	nanogui::Button * button = new nanogui::Button((nanogui::Window*)ray_gui_window, "rayup");
	button->setCallback([&]{
		//this->fillUp();
		clflag = true;
	//	my_cl_dir.v4[0] += 0.001f;
	});
	std::pair<float, float> limits(-1.0f, 1.0f);
	std::pair<float, float> limits_2(-5000.0f, 5000.0f);

	SliderWithText xSliderWithText((nanogui::Widget*)(ray_gui_window), "rayx", std::ref(limits), ".", std::ref(my_cl_dir.v4[0]));
	SliderWithText ySliderWithText((nanogui::Widget*)(ray_gui_window), "rayy", std::ref(limits), ".", std::ref(my_cl_dir.v4[1]));
	SliderWithText zSliderWithText((nanogui::Widget*)(ray_gui_window), "rayz", std::ref(limits), ".", std::ref(my_cl_dir.v4[2]));

	SliderWithText xoSliderWithText((nanogui::Widget*)(ray_gui_window), "orx", std::ref(limits_2), ".", std::ref(my_cl_origin.v4[0]));
	SliderWithText yoSliderWithText((nanogui::Widget*)(ray_gui_window), "ory", std::ref(limits_2), ".", std::ref(my_cl_origin.v4[1]));
	SliderWithText zoSliderWithText((nanogui::Widget*)(ray_gui_window), "orz", std::ref(limits_2), ".", std::ref(my_cl_origin.v4[2]));


	screen->performLayout();

	Shader ray_shader("../shaders/ray.vert", "../shaders/ray.frag");
	float my_ray_vert[] = {	0.0f, 0.0f, 0.0f,
							0.5f, 5000.5f, 0.0f};
	GLuint my_ray_vao;
	GLuint my_ray_vbo;
	glGenVertexArrays(1, &my_ray_vao);
	glGenBuffers(1, &my_ray_vbo);
	glBindVertexArray(my_ray_vao);
	glBindBuffer(GL_ARRAY_BUFFER, my_ray_vbo);
	glBufferData(GL_ARRAY_BUFFER, 3*2*sizeof(float), my_ray_vert, GL_STATIC_DRAW);

//	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*2*sizeof(float), my_ray_vert);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	while (!glfwWindowShouldClose(window))
	{



			
			my_ray_vert[0] = my_cl_origin.v4[0];
			my_ray_vert[1] = my_cl_origin.v4[1];
			my_ray_vert[2] = my_cl_origin.v4[2];

			my_ray_vert[3] = my_cl_origin.v4[0] + 50000*my_cl_dir.v4[0];
			my_ray_vert[4] = my_cl_origin.v4[1] + 50000*my_cl_dir.v4[1];
			my_ray_vert[5] = my_cl_origin.v4[2] + 50000*my_cl_dir.v4[2];

			glBindBuffer(GL_ARRAY_BUFFER, my_ray_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 3*2*sizeof(float), my_ray_vert);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		


//		std::this_thread::sleep_for(time);

		glfwMakeContextCurrent(window);

//		std::cout<<some_gui_var<<std::endl;
		time_cnt++;
		if (time_cnt % 500 == 0)
		{
			std::cout << "FPS: " << 500.0 / (glfwGetTime() - timestamp) << std::endl;
			timestamp = glfwGetTime();
			time_cnt = 0;
		}

		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		ourShader.use();
		glEnable(GL_DEPTH_TEST);
		glfwPollEvents();
		do_movement();

		if (keys[GLFW_KEY_T])
			trig = true;


		// if (trig)
//		f_mat.doStep();
		m_mat[0]->B = f_mat.rail2;
	//	m_mat[1]->B = f_mat.rail2;
	//	m_mat[2]->B = f_mat.rail2;

		my_cassete.updateMatrices(&(f_mat.rail2));
		my_cassete.doStep();

		m_mat[0]->doStep();
		// m_mat[1]->doStep();
		// m_mat[2]->doStep();

		glfwMakeContextCurrent(window);

		//m_mat[1]->doStep();
		// m_mat[0]->driverSM(glfwGetTime()/10.0);
		// m_mat[0]->updateManipConfig();

		// m_mat[2]->driverSM(glfwGetTime()/10.0);
		// m_mat[2]->updateManipConfig();

		m_mat[0]->calculateMatrices();
	//	m_mat[1]->calculateMatrices();
	//	m_mat[2]->calculateMatrices();


clflag = true;
			glm::mat4 ray_mat = m_mat[0]->rangefinder;
			glm::vec3 my_glm_origin;
			my_glm_origin = ray_mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			my_cl_origin.v4[0] = my_glm_origin.x;
			my_cl_origin.v4[1] = my_glm_origin.y;
			my_cl_origin.v4[2] = my_glm_origin.z;

			glm::vec3 my_glm_dir;
			my_glm_dir = ray_mat * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
			my_cl_dir.v4[0] = my_glm_dir.x;
			my_cl_dir.v4[1] = my_glm_dir.y;
			my_cl_dir.v4[2] = my_glm_dir.z;

		if(clflag){
			float ret = -1.0f;
			float _ret;
			_ret = cassete_cl.computeRay(my_cl_origin, my_cl_dir, 5);
			if(	_ret > 0 && ( ret < 0 || ret > _ret ))
				ret = _ret;
			_ret = constr_container.computeRay(my_cl_origin, my_cl_dir);
			if(	_ret > 0 && ( ret < 0 || ret > _ret ))
				ret = _ret;
			_ret = main_frame_cl.computeRay(my_cl_origin, my_cl_dir);
			if(	_ret > 0 && ( ret < 0 || ret > _ret ))
				ret = _ret;
			cl_t = ret;
			


			textBox->setValue(std::to_string(cl_t));

			glm::vec3 sphere_trnsl = glm::vec3((float)my_cl_origin.v4[0], (float)my_cl_origin.v4[1], (float)my_cl_origin.v4[2]) +
			glm::vec3((float)my_cl_dir.v4[0], (float)my_cl_dir.v4[1], (float)my_cl_dir.v4[2])*cl_t;

			sphere_mat = glm::translate(f_mat.World, sphere_trnsl);
			my_sphere.setMatrixByID(0, sphere_mat);

			clflag = false;
		}


		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));

		ourShader.setVec4(glm::vec4(my_cam.Position + glm::vec3(1.0, 0.0, 1.0), 1.0f), "spotLight.position");
		ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");

		view = my_cam.getMatrix();
		//ourShader.setMat4(m_mat[0]->I, "view");
	//	ourShader.setMat4(view, "view");
		ourShader.setFloat(glfwGetTime(), "time");

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width / (float)height, 0.01f, 200.0f);
	//	ourShader.setMat4(proj, "proj");

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		// glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(glm::rotate(m_mat[0]->I,
		// 																							glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0))));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		ourShader.use();

		glUniform1i(obj_type_mode, -1);
		ourShader.setMaterial(Material::green_plastic);

//		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f))); //model to identity

		// column.setMatrixByID(1, m_mat[0]->G2);
		// column.setMatrixByID(2, m_mat[1]->G2);
		// column.setMatrixByID(3, m_mat[2]->G2);

		constr_container.updateMatrices();
		constr_container.drawElements(ourShader);

		ourShader.setMaterial(Material::silver);
		main_frame_gl.setMatrixByID(0, f_mat.A);
		main_frame_cl.setMatrixByID(0, f_mat.A);
		main_frame_gl.Draw(ourShader, 1);
		cassete_gl.setMatrixByID(0, f_mat.A);
		cassete_cl.setMatrixByID(0, f_mat.A);

		cassete_gl.Draw(ourShader, 1);

		my_sphere.Draw(ourShader, 1);
//		my_cube.Draw(ourShader, 1);
		// ourShader.setMaterial(Material::yellow_plastic);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B1));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B2));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B3));
		// rail.Draw(ourShader);
/////
		ourShader.setMaterial(Material::green_plastic);
		carrige.setMatrixByID(0, m_mat[0]->C);
	//	carrige.setMatrixByID(1, m_mat[1]->C);
	//	carrige.setMatrixByID(2, m_mat[2]->C);
		carrige.Draw(ourShader, 1);

		ourShader.setMaterial(Material::black_plastic);
		tower_frame.setMatrixByID(0, m_mat[0]->D);
	//	tower_frame.setMatrixByID(1, m_mat[1]->D);
	//	tower_frame.setMatrixByID(2, m_mat[2]->D);
		tower_frame.Draw(ourShader, 1);

		ourShader.setMaterial(Material::white_plastic);
		tower_box.setMatrixByID(0, m_mat[0]->D);
	//	tower_box.setMatrixByID(1, m_mat[1]->D);
	//	tower_box.setMatrixByID(2, m_mat[2]->D);
		tower_box.Draw(ourShader, 1);

		//pantograph
		ourShader.setMaterial(Material::yellow_plastic);
		pb1.setMatrixByID(0, m_mat[0]->pb1);
	//	pb1.setMatrixByID(1, m_mat[1]->pb1);
	//	pb1.setMatrixByID(2, m_mat[2]->pb1);
		pb1.Draw(ourShader, 1);

		// ourShader.setMaterial(Material::green_plastic);
		pb2.setMatrixByID(0, m_mat[0]->pb2);
	//	pb2.setMatrixByID(1, m_mat[1]->pb2);
	//	pb2.setMatrixByID(2, m_mat[2]->pb2);
		pb2.Draw(ourShader, 1);

		// ourShader.setMaterial(Material::green_plastic);
		pb3.setMatrixByID(0, m_mat[0]->pb3);
	//	pb3.setMatrixByID(1, m_mat[1]->pb3);
	//	pb3.setMatrixByID(2, m_mat[2]->pb3);
		pb3.Draw(ourShader, 1);

		// ourShader.setMaterial(Material::green_plastic);
		pb4.setMatrixByID(0, m_mat[0]->pb4);
	//	pb4.setMatrixByID(1, m_mat[1]->pb4);
	//	pb4.setMatrixByID(2, m_mat[2]->pb4);
		pb4.Draw(ourShader, 1);

		ourShader.setMaterial(Material::green_plastic);
		bond_rail.setMatrixByID(0, m_mat[0]->E1);
	//	bond_rail.setMatrixByID(1, m_mat[1]->E1);
	//	bond_rail.setMatrixByID(2, m_mat[2]->E1);
		bond_rail.Draw(ourShader, 1);

		ourShader.setMaterial(Material::red_plastic);
		bond_carrige.setMatrixByID(0, m_mat[0]->F1);
	//	bond_carrige.setMatrixByID(1, m_mat[1]->F1);
	//	bond_carrige.setMatrixByID(2, m_mat[2]->F1);
		bond_carrige.Draw(ourShader, 1);

		ourShader.setMaterial(Material::green_plastic);
		bond_wrist.setMatrixByID(0, m_mat[0]->G1);
	//	bond_wrist.setMatrixByID(1, m_mat[1]->G1);
	//	bond_wrist.setMatrixByID(2, m_mat[2]->G1);
		bond_wrist.Draw(ourShader, 1);

		ourShader.setMaterial(Material::yellow_plastic);
		bond_handler_middle.setMatrixByID(0, m_mat[0]->H);
	//	bond_handler_middle.setMatrixByID(1, m_mat[1]->H);
	//	bond_handler_middle.setMatrixByID(2, m_mat[2]->H);
		bond_handler_middle.Draw(ourShader, 1);

		ourShader.setMaterial(Material::red_plastic);
//		bond_handler_left.setMatrixByID(0, m_mat[0]->H);
//		bond_handler_left.setMatrixByID(1, m_mat[1]->H);
//		bond_handler_left.setMatrixByID(2, m_mat[2]->H);
//		bond_handler_left.Draw(ourShader, 1);

		ourShader.setMaterial(Material::red_plastic);
//		bond_handler_right.setMatrixByID(0, m_mat[0]->H);
//		bond_handler_right.setMatrixByID(1, m_mat[1]->H);
//		bond_handler_right.setMatrixByID(2, m_mat[2]->H);
//		bond_handler_right.Draw(ourShader, 1);

		//pantograph column
		ourShader.setMaterial(Material::yellow_plastic);
		pc1.setMatrixByID(0, m_mat[0]->pc1);
	//	pc1.setMatrixByID(1, m_mat[1]->pc1);
	//	pc1.setMatrixByID(2, m_mat[2]->pc1);
		pc1.Draw(ourShader, 1);

		// ourShader.setMaterial(Material::green_plastic);
		pc2.setMatrixByID(0, m_mat[0]->pc2);
	//	pc2.setMatrixByID(1, m_mat[1]->pc2);
	//	pc2.setMatrixByID(2, m_mat[2]->pc2);
		pc2.Draw(ourShader, 1);

		ourShader.setMaterial(Material::green_plastic);
		column_rail.setMatrixByID(0, m_mat[0]->E2);
	//	column_rail.setMatrixByID(1, m_mat[1]->E2);
	//	column_rail.setMatrixByID(2, m_mat[2]->E2);
		column_rail.Draw(ourShader, 1);

		ourShader.setMaterial(Material::red_plastic);
		column_carrige.setMatrixByID(0, m_mat[0]->F2);
	//	column_carrige.setMatrixByID(1, m_mat[1]->F2);
	//	column_carrige.setMatrixByID(2, m_mat[2]->F2);
		column_carrige.Draw(ourShader, 1);



		ray_shader.use();
		glBindVertexArray(my_ray_vao);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

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


        glBindBuffer(GL_UNIFORM_BUFFER, 0);
//        screen->drawContents();
        screen->drawWidgets();
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
		glBindBuffer(GL_UNIFORM_BUFFER, uboTransform);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);


		// check OpenGL error
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "OpenGL error: " << err << std::endl;
		}
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}

void do_movement()
{
	GLfloat currentFrame = glfwGetTime();
	static GLfloat lastFrame = 0.0f;
	GLfloat deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (keys[GLFW_KEY_W])
		my_cam.processKeyboard(Camera_movement::FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		my_cam.processKeyboard(Camera_movement::BACKWARD, deltaTime);
	if (keys[GLFW_KEY_D])
		my_cam.processKeyboard(Camera_movement::RIGHT, deltaTime);
	if (keys[GLFW_KEY_A])
		my_cam.processKeyboard(Camera_movement::LEFT, deltaTime);
	if (keys[GLFW_KEY_X])
		my_cam.processKeyboard(Camera_movement::UP, deltaTime);
	if (keys[GLFW_KEY_Z])
		my_cam.processKeyboard(Camera_movement::DOWN, deltaTime);
}

bool mouse_pressed = false;
bool mouse_pressed_r = false;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	static GLfloat lastX = xpos;
	static GLfloat lastY = ypos;

	GLfloat xoffset = lastX - xpos;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	if(mouse_pressed){
		if(mouse_pressed_r)
			my_cam.processMouseMovement(-xoffset, -yoffset);
		else
			my_cam.processMouseMovementTranslate(-xoffset, -yoffset);
	}
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        mouse_pressed = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        mouse_pressed = false;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouse_pressed_r = true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        mouse_pressed_r = false;
}


void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	my_cam.processMouseScroll(yoffset);
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
	if (action == GLFW_PRESS)
		keys[key] = true;
	if (action == GLFW_RELEASE)
	{
		keys[key] = false;
		if (key == GLFW_KEY_B)
		{
			std::cout << "pos " << glm::to_string(my_cam.Position) << std::endl;
			std::cout << "dir " << glm::to_string(my_cam.Direction) << std::endl;
		}
	}
}

unsigned int loadCubeMap(std::vector<std::string> faces)
{
	GLuint cubeTextureID;
	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubeTextureID;
}








