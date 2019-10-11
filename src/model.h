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
	Model(char * path);
	void Draw(Shader shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector <Texture> loaded_textures;
	void loadModel(std::string path);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh processMesh(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType tex_type);
};

GLuint loadTexture(const char * file_path);




#endif