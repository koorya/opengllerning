#ifndef MODEL_H
#define MODEL_H


#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "cl_mesh.h"
#include "cl_kernel_container.h"


class Model: public RayTarget{
public:
	Model(const char * path, std::vector <glm::mat4> instance_mat4, clKernelsContainer * cl_kernel_cont=NULL);
	Model(const char * path, unsigned int max_inst_cnt, clKernelsContainer * cl_kernel_cont=NULL);
	
	void Draw(Shader shader, unsigned int count);
	void setMatrixByID(unsigned int id, glm::mat4 matrix);
	float computeRay(const Ray * ray, int inst_cntinst_cnt = 1);

	std::vector<ClMesh> meshes;
private:
	std::string directory;
	std::string path;
	std::vector <Texture> loaded_textures;

	std::vector<glm::mat4> mat4_vect;
	void loadModel(std::string path);
	int processNode(aiNode * node, const aiScene * scene);
	ClMesh processMesh(aiMesh * mesh, const aiScene * scene);

	clKernelsContainer * cl_kernel_cont;

	std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType tex_type);
};

GLuint loadTexture(const char * file_path);

class ModelContainer{
public:
	ModelContainer(const char * path, unsigned int max_cnt);
	unsigned int getNextID();
	void setMatrixByID(unsigned int id, glm::mat4 matrix);
	void Draw(Shader shader);
private:
	unsigned int current_id;
	unsigned int max_cnt;
	Model * model;

};


#endif