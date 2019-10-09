#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
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
};

class Mesh{
	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
		void Draw(Shader shader);

	private:
		unsigned int VAO, VBO, EBO;

		void setupMesh();
};

#endif

