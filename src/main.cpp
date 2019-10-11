
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


Camera my_cam(glm::vec3(0.942951f, -2.274822f, -0.723970f));
bool keys[1024] = {false};

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);



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



	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	ourShader.setFloat(glm::cos(glm::radians(12.5f)), "spotLight.cutOff");
	ourShader.setFloat(glm::cos(glm::radians(17.5f)), "spotLight.outerCutOff");

	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f)/10.0f, "dirLight.ambient");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "dirLight.diffuse");
	ourShader.setVec3(glm::vec3(0xFF/255.0f, 0xFF/255.0f, 0xFF/255.0f), "dirLight.specular");
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

//	Model my_model("./3d_models/nanosuit/nanosuit.obj");

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

	GLfloat timestamp = glfwGetTime();
	int time_cnt = 0;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	Shader stensilShader( "./shaders/shader.vert", "./shaders/stencil.frag"); 

	while(!glfwWindowShouldClose(window)){
		time_cnt ++;
		if(time_cnt % 1000 == 0){
			std::cout<<"FPS: "<<1000.0/(glfwGetTime() - timestamp)<<std::endl;
			timestamp = glfwGetTime();
			time_cnt = 0;
		}

		glfwPollEvents();
		do_movement();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
	
		glUniform3fv(viewPosLoc, 1, glm::value_ptr(my_cam.getCamPos()));
		
		if(keys[GLFW_KEY_B]){
			ourShader.setVec4(glm::vec4(my_cam.Position, 1.0f), "spotLight.position");
			ourShader.setVec4(glm::vec4(my_cam.Direction, 0.0f), "spotLight.direction");
		}


		view = my_cam.getMatrix();
		ourShader.setMat4(view, "view");

		proj = glm::perspective(glm::radians(my_cam.getZoom()), (float)width/(float)height, 0.01f, 100.0f);
		ourShader.setMat4(proj, "proj");

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
			ourShader.setMat4(model, "model");
			glUniform1i(obj_type_mode, i);

			my_mesh.Draw(ourShader);
		}
		glUniform1i(obj_type_mode, -1);

		ourShader.setMaterial(Material::green_plastic);
		ourShader.setMaterial(Material::chrome);


		for(unsigned int i = 0; i < models.size(); i++){
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			// if(i % 4 == 0)
			// 	model = glm::translate(model, glm::vec3(glm::sin(glfwGetTime()*3), 0.0f, 0.0f));
			// if(i % 3 == 0)
			// 	model = glm::rotate(model, (float)glfwGetTime(),  glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glm::radians(20.0f*i),  glm::vec3(0.0f, 1.0f, 0.0f));
			if(i == 0){
				model = glm::scale(model, glm::vec3(0.02f));
			}else if (i == 1){
				model = glm::scale(model, glm::vec3(0.2f));
			}else{
				ourShader.setMaterial(static_cast<Material> (i));

				model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.002f));
			}
			ourShader.setMat4(model, "model");
			models[i].Draw(ourShader);
		}

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



