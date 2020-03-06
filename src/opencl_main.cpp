
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <CL/opencl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>



GLfloat screen_vertices[] = {
	-1.0f, -1.4f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f};

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
	
	GLFWwindow *window;
	window = glfwCreateWindow(800, 600, "Демо манипулятора с ручным управлением", nullptr, nullptr);
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



	cl_platform_id platform_id;
	cl_uint ret_num_platforms;
	/* получить доступные платформы */
	int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	std::cout<<ret<<std::endl;

	cl_context_properties prop[] = {
		CL_CONTEXT_PLATFORM, 	(cl_context_properties) platform_id,
		CL_WGL_HDC_KHR, 		(cl_context_properties) wglGetCurrentDC(),
		CL_GL_CONTEXT_KHR,		(cl_context_properties) wglGetCurrentContext(),
		0
	};

	cl_device_id device_id;
	cl_uint ret_num_devices;
	/* получить доступные устройства */
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
	std::cout<<ret<<std::endl;



	/* создать контекст */
	cl_context context = clCreateContext(prop, 1, &device_id, NULL, NULL, &ret);
	std::cout<<ret<<std::endl;

	/* создаем команду */
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	std::cout<<ret<<std::endl;

	cl_program program = NULL;
	cl_kernel kernel = NULL;

	const char * cl_src_path = "../cl_kernels/first_cl_module.cl";
	std::string cl_src;

	std::ifstream cl_src_file;

	cl_src_file.exceptions(std::ifstream::failbit);
	
	try{
		cl_src_file.open(cl_src_path);
		std::stringstream cl_src_stream;
		cl_src_stream << cl_src_file.rdbuf();
		cl_src_file.close();

		cl_src = cl_src_stream.str();
	}catch(std::ifstream::failure &e){
		std::cout << "ERROR::OPEN_CL::FILE_NOT_SUCCESFULLY_READ EXEPTION IS:" <<e.what()<<  std::endl;
		exit(-1);
	}
	const char * cl_src_code = cl_src.c_str();
	const size_t src_size = cl_src.length();
	program = clCreateProgramWithSource(context, 1, (const char **)&cl_src_code, (const size_t *)&src_size, &ret);
	std::cout<<ret<<std::endl;

	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	std::cout<<ret<<std::endl;

	kernel = clCreateKernel(program, "test", &ret);
	std::cout<<ret<<std::endl;

	/* OpenGL buffers */
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



	cl_mem memobj = NULL;
	int mem_length = 10;
	cl_int mem[10] = {0};

	memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_length*sizeof(cl_int), NULL, &ret);

	typedef cl_mem (*clCreateFromGLBuffer_type)(cl_context     /* context */,
                     cl_mem_flags   /* flags */,
                     cl_GLuint      /* bufobj */,
                     int *          /* errcode_ret */) ;
//	void * funct = clGetExtensionFunctionAddressForPlatform(platform_id, "clCreateFromGLBuffer");

	cl_mem gl_memobj = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, screen_vbo, &ret);
	std::cout<<gl_memobj<<std::endl;
	glFinish();
	ret = clEnqueueAcquireGLObjects(command_queue, 1, &gl_memobj, 0, NULL, NULL);
//	cl_mem gl_memobj = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(screen_vertices), NULL, &ret);

//	ret = clEnqueueWriteBuffer(command_queue, gl_memobj, CL_TRUE, 0, sizeof(screen_vertices), screen_vertices, 0, NULL, NULL);

	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&gl_memobj);
	cl_float3 origin = {1.0f, 2.0f, 3.0f};
	ret = clSetKernelArg(kernel, 2, sizeof(cl_float3), &origin);
	
	size_t global_work_size[] = {9};

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, mem_length * sizeof(cl_int), mem, 0, NULL, NULL);


	for (int i = 0; i<mem_length; i++){
		std::cout<<"mem["<<i<<"] = "<<mem[i]<<std::endl;
	}

	return 0;

}

