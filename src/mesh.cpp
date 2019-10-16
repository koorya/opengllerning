#include "mesh.h"

#include <glm/gtx/string_cast.hpp>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->indices = indices;
	this->textures = textures;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));

	glBindVertexArray(0);
}

Mesh::Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures)
{
	this->textures = textures;



	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	for (unsigned int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 3*sizeof(aiVector3D) ), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNormals);
	if(mesh->mTextureCoords[0]){
		glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 2*sizeof(aiVector3D) ), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mTextureCoords[0]);
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (sizeof(aiVector3D))));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (2*sizeof(aiVector3D))));

	glBindVertexArray(0);
}


void Mesh::Draw(Shader shader){
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for(unsigned int i = 0; i < textures.size(); i++){

		glActiveTexture(GL_TEXTURE0 + i);
		std::string texname;
		if ( textures[i].type ==TextureType::DIFFUSE ){
			texname = "material.texture_diffuse";
			texname +=  std::to_string(diffuseNr++);
		}
		if ( textures[i].type ==TextureType::SPECULAR ){
			texname = "material.texture_specular";
			texname +=  std::to_string(specularNr++);
		}
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		shader.setInt(i,  texname.c_str());
	}
	glActiveTexture(GL_TEXTURE0);

	if(textures.size() > 0)
		shader.setInt(1, "material.textured");
	else
		shader.setInt(0, "material.textured");

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void Mesh::Draw(Shader shader, unsigned int count){
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	for(unsigned int i = 0; i < textures.size(); i++){

		glActiveTexture(GL_TEXTURE0 + i);
		std::string texname;
		if ( textures[i].type ==TextureType::DIFFUSE ){
			texname = "material.texture_diffuse";
			texname +=  std::to_string(diffuseNr++);
		}
		if ( textures[i].type ==TextureType::SPECULAR ){
			texname = "material.texture_specular";
			texname +=  std::to_string(specularNr++);
		}
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		shader.setInt(i,  texname.c_str());
	}
	glActiveTexture(GL_TEXTURE0);

	if(textures.size() > 0)
		shader.setInt(1, "material.textured");
	else
		shader.setInt(0, "material.textured");

	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, count);
	glBindVertexArray(0);

}






















