
#include "cl_mesh.h"

ClMesh::ClMesh(cl_context context, cl_kernel kernel, aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4, bool print_vert) : Mesh(mesh, scene, textures, instance_mat4, print_vert){
	if(context == NULL || kernel == NULL)
		return;
	int ret;
	this->context = context;
	this->kernel = kernel;
	this->faces_cnt = mesh->mNumFaces;

	vbo_memobj = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY, VBO, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateFromGLBuffer ret "<<ret<<std::endl;
		exit(-1);
	}
	ebo_memobj = clCreateFromGLBuffer(context, CL_MEM_READ_ONLY, EBO, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateFromGLBuffer ret "<<ret<<std::endl;
		exit(-1);
	}
	cl_memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, instance_mat4.size() * mesh->mNumFaces * sizeof(cl_float4), NULL, &ret);
	if (ret != 0){
		std::cout<<"OPEN_CL ERROR | clCreateBuffer ret "<<ret<<std::endl;
		exit(-1);
	}

}

float ClMesh::_computeRay(cl_float3 origin, cl_float3 dir, cl_command_queue command_queue, int inst_cnt){
	return -1;
}
float ClMesh::computeRay(cl_float3 origin, cl_float3 dir, cl_command_queue command_queue, int inst_cnt){
	if(context == NULL || kernel == NULL)
		return -1;

	int ret;

	glFinish();
	ret = clEnqueueAcquireGLObjects(command_queue, 1, &vbo_memobj, 0, NULL, NULL);
	ret = clEnqueueAcquireGLObjects(command_queue, 1, &ebo_memobj, 0, NULL, NULL);


	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&vbo_memobj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&ebo_memobj);


	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&cl_memobj);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_float3), (void*)&origin); //origin
	ret = clSetKernelArg(kernel, 4, sizeof(cl_float3), (void*)&dir); //dir
	unsigned int mat4_ptr_offset = this->mat4_ptr_offset / sizeof(float);
	ret = clSetKernelArg(kernel, 5, sizeof(cl_uint), (void*)&(mat4_ptr_offset)); //mat_ofset


	size_t global_work_size[] = {inst_cnt, faces_cnt};

	ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);


	ret = clFinish(command_queue);

	ret = clEnqueueReleaseGLObjects(command_queue, 1, &vbo_memobj, 0, NULL, NULL);

	ret = clEnqueueReleaseGLObjects(command_queue, 1, &ebo_memobj, 0, NULL, NULL);

	std::vector<cl_float4> res(inst_cnt * faces_cnt);

	ret = clEnqueueReadBuffer(command_queue, cl_memobj, CL_TRUE, 0, inst_cnt * faces_cnt * sizeof(cl_float4), res.data(), 0, NULL, NULL);
//	ret = clEnqueueReadBuffer(command_queue, cl_memobj, CL_TRUE, 0, 100 * sizeof(cl_float4), res, 0, NULL, NULL);


	float ret_f = -1;
//	std::clog<<"mat ofset "<<this->mat4_ptr_offset<<std::endl;
	int j = 0;

	for(int i = 0; i < inst_cnt * faces_cnt; i++){
		if(	res[i].v4[0] >= 0 && res[i].v4[0] <= 1 &&
			res[i].v4[1] >= 0 && res[i].v4[1] <= 1 &&
			res[i].v4[2] >= 0 && res[i].v4[2] <= 1 &&
			res[i].v4[3] > 0){
			if(ret_f<0)
				ret_f = res[i].v4[3];
			if(res[i].v4[3]<ret_f)
				ret_f = res[i].v4[3];
			j++;
//			std::clog<<i<<" ray "<<res[i].v4[0]<<" "<<res[i].v4[1]<<" "<<res[i].v4[2]<<" "<<res[i].v4[3]<<std::endl;
		}
	}
//	std::cout<<"total "<<j<<std::endl;
	return ret_f;
}

