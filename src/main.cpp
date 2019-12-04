
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

void do_movement();
void key_callback(GLFWwindow *, int, int, int, int);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadCubeMap(std::vector<std::string> faces);

Camera my_cam(glm::vec3(0.0, 0.0, 0.0));
bool keys[1024] = {false};

int main()
{


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);





	// std::thread thr1([](float & float_var){	
	// 	nanogui::init();
	// 	nanogui::ref<ExampleApplication> app = new ExampleApplication(float_var);
	// 	app->drawAll();
	// 	app->setVisible(true);
	// 	nanogui::mainloop();
	// //	nanogui::shutdown();
	// 	}, std::ref(some_gui_var1));
	// thr1.detach();


//	nanogui::ref<ExampleApplication> gm = new ExampleApplication(std::ref(some_gui_var1));



#ifdef FULL_SCREEN
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "OpenGL Learning", monitor, nullptr);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#else
	GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Learning", nullptr, nullptr);
#endif

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
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	Shader ourShader("./shaders/shader.vert", "./shaders/shader.frag");
	Shader stensilShader("./shaders/shader.vert", "./shaders/stencil.frag");
	Shader screenShader("./shaders/screen.vert", "./shaders/screen.frag");
	Shader skyboxShader("./shaders/skybox.vert", "./shaders/skybox.frag");
	Shader reflectShader("./shaders/reflect.vert", "./shaders/reflect.frag");
	Shader refractShader("./shaders/refract.vert", "./shaders/refract.frag");

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

	std::vector<std::string> faces({"./textures/skybox/right.jpg",
									"./textures/skybox/left.jpg",
									"./textures/skybox/top.jpg",
									"./textures/skybox/bottom.jpg",
									"./textures/skybox/front.jpg",
									"./textures/skybox/back.jpg"});
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: framebuffer is not complete" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ourShader.use();

	glm::mat4 model = glm::mat4(1.0f);
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

	Model tower_frame("./3D_models/manipulator/Component18.stl", 3);				 //tower frame
	Model tower_box("./3D_models/manipulator/Component31.stl", 3);					 //tower box
	Model carrige("./3D_models/manipulator/Component1_reduce.stl", 3);				 // carrige
	Model rail("./3D_models/manipulator/Component129.stl", 3);						 //rail
	Model bond_wrist("./3D_models/manipulator/wrist.stl", 3);						 //bond wrist
	Model bond_carrige("./3D_models/manipulator/bcar.stl", 3);						 //bond carrige
	Model bond_rail("./3D_models/manipulator/Component21_orient.stl", 3);			 //bond rail
	Model column_rail("./3D_models/manipulator/Component20.stl", 3);				 //column rail
	Model column_carrige("./3D_models/manipulator/Component40.stl", 3);				 //column carrige
	Model bond_handler_middle("./3D_models/manipulator/bond_handler_middle.stl", 3); //bond handler middle
	Model bond_handler_left("./3D_models/manipulator/bond_handler_left.stl", 3);	 //bond handlre left
	Model bond_handler_right("./3D_models/manipulator/bond_handler_right.stl", 3);   //bond handler right

	Model pb1("./3D_models/stl_components/pb1.stl", 3); //bond pantograph section 1
	Model pb2("./3D_models/stl_components/pb2.stl", 3); //bond pantograph section 2
	Model pb3("./3D_models/stl_components/pb3.stl", 3); //bond pantograph section 3
	Model pb4("./3D_models/stl_components/pb4.stl", 3); //bond pantograph section 4

	Model pc1("./3D_models/stl_components/pc1.stl", 3); //column pantograph section 1
	Model pc2("./3D_models/stl_components/pc2.stl", 3); //column pantograph section 2

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


	float some_gui_var = 0;
	float some_gui_var1 = 0;
	guiManipulator gui_man = guiManipulator();
//	nanogui::init();

//		nanogui::mainloop();
//	guiManipulator gui_man = guiManipulator();
//	ExampleApplication * exap = new ExampleApplication(std::ref(some_gui_var));
	std::thread thr([](guiManipulator & man_ref){	
		nanogui::ref<ExampleApplication> app = new ExampleApplication(man_ref);
		app->drawAll();
		app->setVisible(true);
		 nanogui::mainloop();
	//	nanogui::shutdown();
		}, std::ref(gui_man));
	thr.detach();

	RemoteManipulator remote_man = RemoteManipulator(1);
	RemoteManipulator remote_man1 = RemoteManipulator(2);
	RemoteManipulator remote_man2 = RemoteManipulator(3);

	Manipulator *m_mat[3] = {&remote_man, &gui_man, &remote_man1};
	m_mat[0]->config.rail = 3000.0;
	m_mat[2]->config.rail = 6000.0;

	ConstructionContainer constr_container = ConstructionContainer();

	m_mat[0]->container = &constr_container;
	m_mat[1]->container = &constr_container;
	m_mat[2]->container = &constr_container;

	float frame_level = 0; //1*3000.0 ;//+ 500;
	f_mat.World = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
	f_mat.World = glm::rotate(f_mat.World, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	f_mat.A = glm::translate(f_mat.World, glm::vec3(0.0, 0.0, frame_level));

	//rail position on frame
	m_mat[0]->B = glm::translate(f_mat.A, glm::vec3(350.0, 6760.0, -400.0));
	m_mat[1]->B = glm::translate(f_mat.A, glm::vec3(350.0, -245.0, -400.0));
	m_mat[2]->B = glm::translate(f_mat.A, glm::vec3(350.0, -7250.0, -400.0));

	Model main_frame("./3d_models/stl_components/main_frame.stl", 1);

	bool trig = false;




	while (!glfwWindowShouldClose(window))
	{
//		std::cout<<some_gui_var<<std::endl;
		time_cnt++;
		if (time_cnt % 1000 == 0)
		{
			std::cout << "FPS: " << 1000.0 / (glfwGetTime() - timestamp) << std::endl;
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
			// for (int i = 0; i < 2; i++)
			// {
			// 	m_mat[i]->doStep();
			// }
		m_mat[1]->doStep();
		// m_mat[0]->driverSM(glfwGetTime()/10.0);
		// m_mat[0]->updateManipConfig();

		// m_mat[2]->driverSM(glfwGetTime()/10.0);
		// m_mat[2]->updateManipConfig();

		m_mat[0]->calculateMatrices();
		m_mat[1]->calculateMatrices();
		m_mat[2]->calculateMatrices();

		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));
		if (keys[GLFW_KEY_B])
		{
			ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
			ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
		}
		view = my_cam.getMatrix();
		//ourShader.setMat4(m_mat[0]->I, "view");
		ourShader.setMat4(view, "view");
		ourShader.setFloat(glfwGetTime(), "time");

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width / (float)height, 0.01f, 200.0f);
		ourShader.setMat4(proj, "proj");

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

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1.0f))); //model to identity

		// column.setMatrixByID(1, m_mat[0]->G2);
		// column.setMatrixByID(2, m_mat[1]->G2);
		// column.setMatrixByID(3, m_mat[2]->G2);

		constr_container.updateMatrices();
		constr_container.drawElements(ourShader);

		ourShader.setMaterial(Material::silver);
		main_frame.setMatrixByID(0, f_mat.A);
		main_frame.Draw(ourShader, 1);

		// ourShader.setMaterial(Material::yellow_plastic);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B1));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B2));
		// rail.Draw(ourShader);
		// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(mat_B3));
		// rail.Draw(ourShader);

		ourShader.setMaterial(Material::green_plastic);
		carrige.setMatrixByID(0, m_mat[0]->C);
		carrige.setMatrixByID(1, m_mat[1]->C);
		carrige.setMatrixByID(2, m_mat[2]->C);
		carrige.Draw(ourShader, 3);

		ourShader.setMaterial(Material::black_plastic);
		tower_frame.setMatrixByID(0, m_mat[0]->D);
		tower_frame.setMatrixByID(1, m_mat[1]->D);
		tower_frame.setMatrixByID(2, m_mat[2]->D);
		tower_frame.Draw(ourShader, 3);

		ourShader.setMaterial(Material::white_plastic);
		tower_box.setMatrixByID(0, m_mat[0]->D);
		tower_box.setMatrixByID(1, m_mat[1]->D);
		tower_box.setMatrixByID(2, m_mat[2]->D);
		tower_box.Draw(ourShader, 3);

		//pantograph
		ourShader.setMaterial(Material::yellow_plastic);
		pb1.setMatrixByID(0, m_mat[0]->pb1);
		pb1.setMatrixByID(1, m_mat[1]->pb1);
		pb1.setMatrixByID(2, m_mat[2]->pb1);
		pb1.Draw(ourShader, 3);

		// ourShader.setMaterial(Material::green_plastic);
		pb2.setMatrixByID(0, m_mat[0]->pb2);
		pb2.setMatrixByID(1, m_mat[1]->pb2);
		pb2.setMatrixByID(2, m_mat[2]->pb2);
		pb2.Draw(ourShader, 3);

		// ourShader.setMaterial(Material::green_plastic);
		pb3.setMatrixByID(0, m_mat[0]->pb3);
		pb3.setMatrixByID(1, m_mat[1]->pb3);
		pb3.setMatrixByID(2, m_mat[2]->pb3);
		pb3.Draw(ourShader, 3);

		// ourShader.setMaterial(Material::green_plastic);
		pb4.setMatrixByID(0, m_mat[0]->pb4);
		pb4.setMatrixByID(1, m_mat[1]->pb4);
		pb4.setMatrixByID(2, m_mat[2]->pb4);
		pb4.Draw(ourShader, 3);

		ourShader.setMaterial(Material::green_plastic);
		bond_rail.setMatrixByID(0, m_mat[0]->E1);
		bond_rail.setMatrixByID(1, m_mat[1]->E1);
		bond_rail.setMatrixByID(2, m_mat[2]->E1);
		bond_rail.Draw(ourShader, 3);

		ourShader.setMaterial(Material::red_plastic);
		bond_carrige.setMatrixByID(0, m_mat[0]->F1);
		bond_carrige.setMatrixByID(1, m_mat[1]->F1);
		bond_carrige.setMatrixByID(2, m_mat[2]->F1);
		bond_carrige.Draw(ourShader, 3);

		ourShader.setMaterial(Material::green_plastic);
		bond_wrist.setMatrixByID(0, m_mat[0]->G1);
		bond_wrist.setMatrixByID(1, m_mat[1]->G1);
		bond_wrist.setMatrixByID(2, m_mat[2]->G1);
		bond_wrist.Draw(ourShader, 3);

		ourShader.setMaterial(Material::yellow_plastic);
		bond_handler_middle.setMatrixByID(0, m_mat[0]->H);
		bond_handler_middle.setMatrixByID(1, m_mat[1]->H);
		bond_handler_middle.setMatrixByID(2, m_mat[2]->H);
		bond_handler_middle.Draw(ourShader, 3);

		ourShader.setMaterial(Material::red_plastic);
		bond_handler_left.setMatrixByID(0, m_mat[0]->H);
		bond_handler_left.setMatrixByID(1, m_mat[1]->H);
		bond_handler_left.setMatrixByID(2, m_mat[2]->H);
		bond_handler_left.Draw(ourShader, 3);

		ourShader.setMaterial(Material::red_plastic);
		bond_handler_right.setMatrixByID(0, m_mat[0]->H);
		bond_handler_right.setMatrixByID(1, m_mat[1]->H);
		bond_handler_right.setMatrixByID(2, m_mat[2]->H);
		bond_handler_right.Draw(ourShader, 3);

		//pantograph column
		ourShader.setMaterial(Material::yellow_plastic);
		pc1.setMatrixByID(0, m_mat[0]->pc1);
		pc1.setMatrixByID(1, m_mat[1]->pc1);
		pc1.setMatrixByID(2, m_mat[2]->pc1);
		pc1.Draw(ourShader, 3);

		// ourShader.setMaterial(Material::green_plastic);
		pc2.setMatrixByID(0, m_mat[0]->pc2);
		pc2.setMatrixByID(1, m_mat[1]->pc2);
		pc2.setMatrixByID(2, m_mat[2]->pc2);
		pc2.Draw(ourShader, 3);

		ourShader.setMaterial(Material::green_plastic);
		column_rail.setMatrixByID(0, m_mat[0]->E2);
		column_rail.setMatrixByID(1, m_mat[1]->E2);
		column_rail.setMatrixByID(2, m_mat[2]->E2);
		column_rail.Draw(ourShader, 3);

		ourShader.setMaterial(Material::red_plastic);
		column_carrige.setMatrixByID(0, m_mat[0]->F2);
		column_carrige.setMatrixByID(1, m_mat[1]->F2);
		column_carrige.setMatrixByID(2, m_mat[2]->F2);
		column_carrige.Draw(ourShader, 3);

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

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	static GLfloat lastX = xpos;
	static GLfloat lastY = ypos;

	GLfloat xoffset = lastX - xpos;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	my_cam.processMouseMovement(xoffset, yoffset);
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
			std::cout << glm::to_string(my_cam.Position) << std::endl;
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
