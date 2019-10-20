#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "shader.h"
#include <vector>

struct Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

enum TextureType{
	DIFFUSE = 0,
	SPECULAR
};
struct Texture{
	unsigned int id;
	TextureType type;
	std::string path;
};

class Mesh{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
//		Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures);
		Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <float> instance_float = std::vector <float>({0.0}));
		Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4);
		
		void Draw(Shader shader);
		void Draw(Shader shader, unsigned int count);
	private:
		unsigned int VAO, VBO, EBO;

};

#endif

