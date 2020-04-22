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
#include "rangefinder.h"

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
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

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



	std::cout<<"before clKernelsContainer"<<std::endl;
	clKernelsContainer my_cl_cont = clKernelsContainer();
	std::cout<<"after clKernelsContainer"<<std::endl;

	


	screen = new Screen();

	screen->initialize(window, true);



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
	Shader skyboxShader("../shaders/skybox.vert", "../shaders/skybox.frag");

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
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	ourShader.setVec3(glm::vec3(0x10 / 255.0f, 0x10 / 255.0f, 0x10 / 255.0f) / 1.0f, "dirLight[0].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[0].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[0].specular");
	ourShader.setVec4(glm::vec4(1.0f, 0.0f, -1.0f, 0.0f), "dirLight[0].direction");

	ourShader.setVec3(glm::vec3(0x10 / 255.0f, 0x10 / 255.0f, 0x10 / 255.0f) / 1.0f, "dirLight[1].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[1].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[1].specular");
	ourShader.setVec4(glm::vec4(-0.5f, -0.5f, -1.0f, 0.0f), "dirLight[1].direction");

	ourShader.setVec3(glm::vec3(0x10 / 255.0f, 0x10 / 255.0f, 0x10 / 255.0f) / 1.0f, "dirLight[1].ambient");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[1].diffuse");
	ourShader.setVec3(glm::vec3(0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f), "dirLight[1].specular");
	ourShader.setVec4(glm::vec4(-0.5f, 0.5f, -1.0f, 0.0f), "dirLight[2].direction");


	GLuint viewPosLoc = glGetUniformLocation(ourShader.Program, "viewPos");


	glfwMakeContextCurrent(window);

#define light_model_
#ifdef light_model

	Model girl = Model("../3D_models/sphere.obj", 1);
	Model nanosuit = Model("../3D_models/sphere.obj", 1);
	
	

//	Model main_frame("../3D_models/stl_components/main_frame.stl", 1);
//	Model main_frame("../3D_models/stend_mm_inventor_blender.obj", 1);
	Model my_sphere("../3D_models/sphere.obj", 1);
	Model my_dr_cube("../3D_models/cube_cil.obj", 1);
//	print_vert = true;
	Model my_cube("../3D_models/sphere.obj", 1, &my_cl_cont);
//	print_vert = false;
	 Model main_frame_cl("../3D_models/sphere.obj", 1, &my_cl_cont);
	 Model main_frame_gl("../3D_models/sphere.obj", 1);
//	Model main_frame("../3D_models/obj/manipulator/Pc2.obj", 1, &my_cl_cont);
	Model cassete_cl("../3D_models/sphere.obj", 1, &my_cl_cont);
	Model cassete_gl("../3D_models/sphere.obj", 1);

//	Model cassete("../3D_models/obj/manipulator/Pc1.obj", 1);
	Model tower_frame("../3D_models/sphere.obj", 3);				 //tower frame
	
	Model tower_box("../3D_models/sphere.obj", 3, &my_cl_cont);					 //tower box
	Model carrige("../3D_models/sphere.obj", 3);				 // carrige
	Model rail("../3D_models/sphere.obj", 3);						 //rail
	Model bond_wrist("../3D_models/sphere.obj", 3);						 //bond wrist
	Model bond_carrige("../3D_models/sphere.obj", 3);						 //bond carrige
	Model bond_rail("../3D_models/sphere.obj", 3);			 //bond rail
	Model column_rail("../3D_models/sphere.obj", 3);				 //column rail
	Model column_carrige("../3D_models/sphere.obj", 3);				 //column carrige
	// Model bond_handler_middle("../3D_models/obj/manipulator/Bond Handler Middle.obj", 3); //bond handler middle
	// Model bond_handler_left("../3D_models/obj/manipulator/Bond Handler Left.obj", 3);	 //bond handlre left
	// Model bond_handler_right("../3D_models/obj/manipulator/Bond Handler Right.obj", 3);   //bond handler right
	Model bond_handler_middle("../3D_models/sphere.obj", 3, &my_cl_cont);   //bond handler right
//	Model bond_handler_middle("../3D_models/cube_cil.obj", 3, context, kernel);   //bond handler right

	Model pb1("../3D_models/sphere.obj", 3); //bond pantograph section 1
	Model pb2("../3D_models/sphere.obj", 3); //bond pantograph section 2
	Model pb3("../3D_models/sphere.obj", 3); //bond pantograph section 3
	Model pb4("../3D_models/sphere.obj", 3); //bond pantograph section 4

	Model pc1("../3D_models/sphere.obj", 3); //column pantograph section 1
	Model pc2("../3D_models/sphere.obj", 3); //column pantograph section 2

#else

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
	Model cassete_cl("../3D_models/правильная кассета.stl", 1, &my_cl_cont);
	Model cassete_gl("../3D_models/правильная кассета.obj", 1);

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
	Model bond_handler_middle("../3D_models/obj/manipulator/zaklepochnik1.obj", 3, &my_cl_cont);   //bond handler right
//	Model bond_handler_middle("../3D_models/cube_cil.obj", 3, context, kernel);   //bond handler right

	Model pb1("../3D_models/obj/manipulator/Pb1.obj", 3); //bond pantograph section 1
	Model pb2("../3D_models/obj/manipulator/Pb2.obj", 3); //bond pantograph section 2
	Model pb3("../3D_models/obj/manipulator/Pb3.obj", 3); //bond pantograph section 3
	Model pb4("../3D_models/obj/manipulator/Pb4.obj", 3); //bond pantograph section 4

	Model pc1("../3D_models/obj/manipulator/Pc1.obj", 3); //column pantograph section 1
	Model pc2("../3D_models/obj/manipulator/Pc2.obj", 3); //column pantograph section 2


#endif



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

	guiRemoteManipulator gui_man = guiRemoteManipulator(screen, 1);
//	guiManipulator gui_man = guiManipulator(screen);

	glfwSetWindowPos(window, 50, 100);


	Manipulator *m_mat[3] = {&gui_man, NULL, NULL};

	glfwMakeContextCurrent(window);

	ConstructionContainer constr_container = ConstructionContainer(&my_cl_cont);

	m_mat[0]->container = &constr_container;
//	m_mat[1]->container = &constr_container;
//	m_mat[2]->container = &constr_container;


	MainFrame f_mat;

	GUICassete my_cassete(screen, &constr_container);
	my_cassete.fillUp();
	my_cassete.updateMatrices(&(f_mat.rail2));
	m_mat[0]->cassete = &my_cassete;
//	m_mat[1]->cassete = &my_cassete;
//	m_mat[2]->cassete = &my_cassete;

	bool trig = false;


	std::chrono::seconds time(1);

	bool clflag = false;

	screen->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Minimum, 0, 0));
	screen->performLayout();



	guiRangefindersContainer my_rf_cont = guiRangefindersContainer(screen);
	my_rf_cont.addRangefinder(&(m_mat[0]->rangefinder1));
	my_rf_cont.addRangefinder(&(m_mat[0]->rangefinder2));
	my_rf_cont.addRangefinder(&(m_mat[0]->rangefinder3));
	my_rf_cont.addRangefinder(&(m_mat[0]->rangefinder4));
//	my_rf_cont.addRangefinder(&(m_mat[0]->rangefinder5));

	my_rf_cont.addTarget(&cassete_cl);
	my_rf_cont.addTarget(&constr_container);
	my_rf_cont.addTarget(&main_frame_cl);

	glfwShowWindow(window);

	while (!glfwWindowShouldClose(window))
	{




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

		constr_container.updateMatrices();



		glfwMakeContextCurrent(window);

		//m_mat[1]->doStep();
		// m_mat[0]->driverSM(glfwGetTime()/10.0);
		// m_mat[0]->updateManipConfig();

		// m_mat[2]->driverSM(glfwGetTime()/10.0);
		// m_mat[2]->updateManipConfig();



clflag = true;

		if(clflag){
		//	float ret = -1.0f;
			my_rf_cont.computeRays();
		//	cl_t = ret;
			


		//	textBox->setValue(std::to_string(cl_t));

			clflag = false;
		}


		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));


		view = my_cam.getMatrix();
		view = view * glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		ourShader.setFloat(glfwGetTime(), "time");
		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width / (float)height, 0.01f, 200.0f);
		proj = proj * glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));

		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glStencilMask(0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		ourShader.use();

		constr_container.drawElements(ourShader);

		main_frame_gl.setMatrixByID(0, f_mat.A);
		main_frame_cl.setMatrixByID(0, f_mat.A);
		main_frame_gl.Draw(ourShader, 1);
		cassete_gl.setMatrixByID(0, f_mat.A);
		cassete_cl.setMatrixByID(0, f_mat.A);

		cassete_gl.Draw(ourShader, 1);

		my_rf_cont.Draw(ourShader);

		carrige.setMatrixByID(0, m_mat[0]->C);
	//	carrige.setMatrixByID(1, m_mat[1]->C);
	//	carrige.setMatrixByID(2, m_mat[2]->C);
		carrige.Draw(ourShader, 1);

		tower_frame.setMatrixByID(0, m_mat[0]->D);
	//	tower_frame.setMatrixByID(1, m_mat[1]->D);
	//	tower_frame.setMatrixByID(2, m_mat[2]->D);
		tower_frame.Draw(ourShader, 1);

		tower_box.setMatrixByID(0, m_mat[0]->D);
	//	tower_box.setMatrixByID(1, m_mat[1]->D);
	//	tower_box.setMatrixByID(2, m_mat[2]->D);
		tower_box.Draw(ourShader, 1);

		//pantograph
		pb1.setMatrixByID(0, m_mat[0]->pb1);
	//	pb1.setMatrixByID(1, m_mat[1]->pb1);
	//	pb1.setMatrixByID(2, m_mat[2]->pb1);
		pb1.Draw(ourShader, 1);

		pb2.setMatrixByID(0, m_mat[0]->pb2);
	//	pb2.setMatrixByID(1, m_mat[1]->pb2);
	//	pb2.setMatrixByID(2, m_mat[2]->pb2);
		pb2.Draw(ourShader, 1);

		pb3.setMatrixByID(0, m_mat[0]->pb3);
	//	pb3.setMatrixByID(1, m_mat[1]->pb3);
	//	pb3.setMatrixByID(2, m_mat[2]->pb3);
		pb3.Draw(ourShader, 1);

		pb4.setMatrixByID(0, m_mat[0]->pb4);
	//	pb4.setMatrixByID(1, m_mat[1]->pb4);
	//	pb4.setMatrixByID(2, m_mat[2]->pb4);
		pb4.Draw(ourShader, 1);

		bond_rail.setMatrixByID(0, m_mat[0]->E1);
	//	bond_rail.setMatrixByID(1, m_mat[1]->E1);
	//	bond_rail.setMatrixByID(2, m_mat[2]->E1);
		bond_rail.Draw(ourShader, 1);

		bond_carrige.setMatrixByID(0, m_mat[0]->F1);
	//	bond_carrige.setMatrixByID(1, m_mat[1]->F1);
	//	bond_carrige.setMatrixByID(2, m_mat[2]->F1);
		bond_carrige.Draw(ourShader, 1);

		bond_wrist.setMatrixByID(0, m_mat[0]->G1);
	//	bond_wrist.setMatrixByID(1, m_mat[1]->G1);
	//	bond_wrist.setMatrixByID(2, m_mat[2]->G1);
		bond_wrist.Draw(ourShader, 1);

		bond_handler_middle.setMatrixByID(0, m_mat[0]->H);
	//	bond_handler_middle.setMatrixByID(1, m_mat[1]->H);
	//	bond_handler_middle.setMatrixByID(2, m_mat[2]->H);
		bond_handler_middle.Draw(ourShader, 1);



		//pantograph column
		pc1.setMatrixByID(0, m_mat[0]->pc1);
	//	pc1.setMatrixByID(1, m_mat[1]->pc1);
	//	pc1.setMatrixByID(2, m_mat[2]->pc1);
		pc1.Draw(ourShader, 1);

		pc2.setMatrixByID(0, m_mat[0]->pc2);
	//	pc2.setMatrixByID(1, m_mat[1]->pc2);
	//	pc2.setMatrixByID(2, m_mat[2]->pc2);
		pc2.Draw(ourShader, 1);

		column_rail.setMatrixByID(0, m_mat[0]->E2);
	//	column_rail.setMatrixByID(1, m_mat[1]->E2);
	//	column_rail.setMatrixByID(2, m_mat[2]->E2);
		column_rail.Draw(ourShader, 1);

		column_carrige.setMatrixByID(0, m_mat[0]->F2);
	//	column_carrige.setMatrixByID(1, m_mat[1]->F2);
	//	column_carrige.setMatrixByID(2, m_mat[2]->F2);
		column_carrige.Draw(ourShader, 1);



		skyboxShader.use();
		skyboxShader.setMat4(proj, "proj");
		skyboxShader.setMat4(glm::mat4(glm::mat3(view)), "view");
		glDepthMask(GL_FALSE);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);



        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
bool mouse_pressed_m = false;

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
	}else if(mouse_pressed_m){
		my_cam.processMouseMovementMiddle(-xoffset, -yoffset);
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
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        mouse_pressed_m = true;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        mouse_pressed_m = false;
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








