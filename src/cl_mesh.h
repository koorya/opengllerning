
#ifndef CL_MESH
#define CL_MESH

#include "mesh.h"
#include "cl_kernel_container.h"
#include <CL/opencl.h>

class ClMesh: public Mesh, public RayTarget{
	public:
	
	ClMesh(clKernelsContainer * cl_kernel_cont, aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4, bool print_vert = false);
	float computeRay(const Ray * ray, int inst_cnt = 1);

	private:

	clKernelsContainer * cl_kernel_cont;
	unsigned int cl_kernel_ptr;
	
};

#endif
