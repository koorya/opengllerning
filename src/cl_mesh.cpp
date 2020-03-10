
#include "cl_mesh.h"

ClMesh::ClMesh(clKernelsContainer * cl_kernel_cont, aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4, bool print_vert) : Mesh(mesh, scene, textures, instance_mat4, print_vert), cl_kernel_cont(cl_kernel_cont){
	if(cl_kernel_cont == NULL)
		return;

	cl_kernel_ptr = cl_kernel_cont->addModel(VBO, EBO, this->mat4_ptr_offset / sizeof(float), mesh->mNumFaces, instance_mat4.size());

}


float ClMesh::computeRay(cl_float3 origin, cl_float3 dir, int inst_cnt){
	if(cl_kernel_cont == NULL)
		return -1;

	return cl_kernel_cont->computeRay(cl_kernel_ptr, inst_cnt, origin, dir);

}

