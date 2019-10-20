#ifndef MODEL_H
#define MODEL_H


#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh.h"


class Model{
public:
	Model(const char * path, std::vector<float> rad_vect = std::vector<float>({0.0}));
	Model(const char * path, std::vector <glm::mat4> instance_mat4);
	
	void Draw(Shader shader);
	void Draw(Shader shader, unsigned int count);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector <Texture> loaded_textures;
	std::vector<float> rad_vect;
	std::vector<glm::mat4> mat4_vect;
	void loadModel(std::string path);
	int processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType tex_type);
};

GLuint loadTexture(const char * file_path);




#endif