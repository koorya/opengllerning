
#ifndef CL_KERNEL_CONTAINER
#define CL_KERNEL_CONTAINER

#include <CL/opencl.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


typedef struct {
	cl_mem vbo_memobj;
	cl_mem ebo_memobj;
	cl_mem cl_memobj;
	unsigned int faces_cnt;
	unsigned int max_inst_cnt;
	unsigned int mat4_ptr;
} mesh_cl_data;

class Ray{
	public:
		Ray();
		cl_float3 origin;
		cl_float3 dir;
};

class RayTarget{
	public:
	virtual float computeRay(const Ray * ray, int inst_cntinst_cnt = 1);
};

class clKernelsContainer{
	public:
		clKernelsContainer();
		~clKernelsContainer();
		unsigned int addModel(unsigned int VBO, unsigned int EBO, unsigned int mat4_ptr_offset_index, unsigned int faces_cnt, unsigned int inst_cnt);
		float computeRay(unsigned int mesh_cl_ptr, int inst_cnt, const Ray * ray);

	private:

	cl_context context;
	cl_command_queue command_queue;
	cl_kernel intersect_kernel;
	cl_kernel min_kernel;
	cl_kernel translate_kernel;

	std::vector<mesh_cl_data> mem_objects;
	


};


#endif

