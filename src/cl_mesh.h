
#ifndef CL_MESH
#define CL_MESH

#include "mesh.h"
#include <CL/opencl.h>


class ClMesh: public Mesh{
	public:
	
	ClMesh(cl_context context, cl_kernel kernel, aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4, bool print_vert = false);
	float computeRay(cl_float3 origin, cl_float3 dir, cl_command_queue command_queue, int inst_cntinst_cnt = 1);
	float _computeRay(cl_float3 origin, cl_float3 dir, cl_command_queue command_queue, int inst_cntinst_cnt = 1);

	private:

	cl_context context;
	cl_kernel kernel;

	cl_mem vbo_memobj;
	cl_mem ebo_memobj;
	cl_mem cl_memobj;

	unsigned int faces_cnt;
	
};

#endif
