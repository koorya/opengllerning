
#include "cl_kernel_container.h"

#include <glad/glad.h>


Ray::Ray(){

}

clKernelsContainer::clKernelsContainer(){
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
	context = clCreateContext(prop, 1, &device_id, NULL, NULL, &ret);
	std::cout<<ret<<std::endl;

	/* создаем команду */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	std::cout<<ret<<std::endl;

	cl_program program = NULL;
	intersect_kernel = NULL;

	const char * cl_src_path = "../cl_kernels/ray_triangle_intersect.cl";
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
	std::cout<<"clBuildProgram "<<ret<<std::endl;
	if(ret) 
		exit(-1);
	intersect_kernel = clCreateKernel(program, "test", &ret);
	std::cout<<ret<<std::endl;
	if(ret)
		exit(-1);
	min_kernel = clCreateKernel(program, "minimaze", &ret);
	std::cout<<ret<<std::endl;
	if(ret)
		exit(-1);
	translate_kernel = clCreateKernel(program, "translate_arr", &ret);
	std::cout<<ret<<std::endl;
	if(ret)
		exit(-1);


}

clKernelsContainer::~clKernelsContainer(){

}

unsigned int clKernelsContainer::addModel(unsigned int VBO, unsigned int EBO, unsigned int mat4_ptr_offset_index, unsigned int faces_cnt, unsigned int inst_cnt){
	int ret = 0;
	mesh_cl_data mesh_data;
	mesh_data.faces_cnt = faces_cnt;
	mesh_data.max_inst_cnt = inst_cnt;
	mesh_data.mat4_ptr = mat4_ptr_offset_index;
	mesh_data.vbo_memobj = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY, VBO, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateFromGLBuffer ret "<<ret<<std::endl;
		exit(-1);
	}
	mesh_data.ebo_memobj = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY, EBO, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateFromGLBuffer ret "<<ret<<std::endl;
		exit(-1);
	}
	mesh_data.cl_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, faces_cnt * inst_cnt * sizeof(cl_float), NULL, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateBuffer ret "<<ret<<std::endl;
		exit(-1);
	}
	
	mem_objects.push_back(mesh_data);
	
	return mem_objects.size() - 1;
}


void minimaze(float * arr, unsigned int size);

float clKernelsContainer::computeRay(unsigned int mesh_cl_ptr, int inst_cnt, const Ray * ray){
	mesh_cl_data * m = &(mem_objects[mesh_cl_ptr]);
	if(inst_cnt <= 0)
		return -1.0f;
	if(inst_cnt > m->max_inst_cnt)
		inst_cnt = m->max_inst_cnt;

	int ret;

	glFinish();
	ret = clEnqueueAcquireGLObjects(command_queue, 1, &(m->vbo_memobj), 0, NULL, NULL);
	ret = clEnqueueAcquireGLObjects(command_queue, 1, &m->ebo_memobj, 0, NULL, NULL);


	ret = clSetKernelArg(intersect_kernel, 0, sizeof(cl_mem), (void*)&m->vbo_memobj);
	ret = clSetKernelArg(intersect_kernel, 1, sizeof(cl_mem), (void*)&m->ebo_memobj);


	ret = clSetKernelArg(intersect_kernel, 2, sizeof(cl_mem), (void*)&m->cl_memobj);
	ret = clSetKernelArg(intersect_kernel, 3, sizeof(cl_float3), (void*)&ray->origin); //origin
	ret = clSetKernelArg(intersect_kernel, 4, sizeof(cl_float3), (void*)&ray->dir); //dir


	ret = clSetKernelArg(intersect_kernel, 5, sizeof(cl_uint), (void*)&(m->mat4_ptr)); //mat_ofset



	size_t global_work_size[] = {(size_t)inst_cnt, m->faces_cnt};

	ret = clEnqueueNDRangeKernel(command_queue, intersect_kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);

	ret = clFinish(command_queue);
	ret = clEnqueueReleaseGLObjects(command_queue, 1, &m->vbo_memobj, 0, NULL, NULL);
	ret = clEnqueueReleaseGLObjects(command_queue, 1, &m->ebo_memobj, 0, NULL, NULL);

	int size = inst_cnt * m->faces_cnt;
	int stride;
	int prev_stride = 1;
	int cnt = 512;
	for(stride = 512; size/stride >= 512 || stride == 512; stride *= 512){
		ret = clSetKernelArg(min_kernel, 0, sizeof(cl_mem), (void*)&m->cl_memobj); //массив
		ret = clSetKernelArg(min_kernel, 1, sizeof(cl_uint), (void*)&size); // размер массива для нахождения размера последнего кусочка 
		ret = clSetKernelArg(min_kernel, 2, sizeof(cl_uint), (void*)&stride); //интервал между началами кусочков (нужен, чтобы посчитать offset для кусочка в потоке)
		ret = clSetKernelArg(min_kernel, 3, sizeof(cl_uint), (void*)&prev_stride); // интервал между элементами кусочка (для обработки кусочка в потоке, изначально 1, потом 512, потом 512*512)
		ret = clSetKernelArg(min_kernel, 4, sizeof(cl_uint), (void*)&cnt); // количество элементов в целом кусочке, всегда 512
		size_t global_work_size[] = {(size_t)(size / stride + 1)}; // количество целых кусочков + 1 неполный
		prev_stride = stride;
	//	cnt = 512;
		ret = clEnqueueNDRangeKernel(command_queue, min_kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	}

	ret = clSetKernelArg(translate_kernel, 0, sizeof(cl_mem), (void*)&m->cl_memobj); //массив
	ret = clSetKernelArg(translate_kernel, 1, sizeof(cl_uint), (void*)&prev_stride); //интервал между началами кусочков (нужен, чтобы посчитать offset для кусочка в потоке)
	size_t tr_global_work_size[] = {(size_t)(size / prev_stride + 1)}; //

	ret = clEnqueueNDRangeKernel(command_queue, translate_kernel, 1, NULL, tr_global_work_size, NULL, 0, NULL, NULL);

	std::vector<cl_float> res(size / prev_stride + 1);

	ret = clEnqueueReadBuffer(command_queue, m->cl_memobj, CL_TRUE, 0, (size / prev_stride + 1) * sizeof(cl_float), res.data(), 0, NULL, NULL);

	float ret_f = -1;

	for(int i = 0; i < (size / prev_stride + 1); i++){
		if(res[i] > 0){
			if(ret_f<0)
				ret_f = res[i];
			if(res[i]<ret_f){
				ret_f = res[i];
			}
		}
	}
	return ret_f;
}



