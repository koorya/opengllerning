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
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex) + 17*sizeof(float), vertices.data(), GL_STATIC_DRAW);
	
	glm::mat4 some_mat4 = glm::mat4(1.0);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), sizeof(float)*16, glm::value_ptr(some_mat4));

	float some_float = 0.0;
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex) + sizeof(float)*16, sizeof(float), &some_float);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(vertices.size()*sizeof(Vertex) + sizeof(float)*16));
	glVertexAttribDivisor(3, 1000000);

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(vertices.size()*sizeof(Vertex)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(vertices.size()*sizeof(Vertex) + 4*sizeof(float)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(vertices.size()*sizeof(Vertex) + 8*sizeof(float)));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(vertices.size()*sizeof(Vertex) + 12*sizeof(float)));
	glVertexAttribDivisor(4, 1000000);
	glVertexAttribDivisor(5, 1000000);
	glVertexAttribDivisor(6, 1000000);
	glVertexAttribDivisor(7, 1000000);


	glBindVertexArray(0);
}

Mesh::Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <float> instance_float)
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
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 3*sizeof(aiVector3D) ) + instance_float.size()*sizeof(float) + sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	if(glGetError() == GL_OUT_OF_MEMORY)
		std::cout<<"ERROR::MESH:: GL_OUT_OF_MEMORY "<<mesh->mNumVertices * ( 3*sizeof(aiVector3D) ) + instance_float.size()*sizeof(float)<<std::endl;

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNormals);
	if(mesh->mTextureCoords[0]){
		glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 2*sizeof(aiVector3D) ), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mTextureCoords[0]);
	}
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (3*sizeof(aiVector3D)), instance_float.size()*sizeof(float), instance_float.data());
	glm::mat4 some_mat4 = glm::mat4(1.0);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (3*sizeof(aiVector3D)) + instance_float.size()*sizeof(float), sizeof(float)*16, glm::value_ptr(some_mat4));


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (sizeof(aiVector3D))));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (2*sizeof(aiVector3D))));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))));
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))+instance_float.size()*sizeof(float)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))+instance_float.size()*sizeof(float) + 4*sizeof(float)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))+instance_float.size()*sizeof(float) + 8*sizeof(float)));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float)*16, 
							(void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))+instance_float.size()*sizeof(float) + 12*sizeof(float)));
	glVertexAttribDivisor(4, 1000000);
	glVertexAttribDivisor(5, 1000000);
	glVertexAttribDivisor(6, 1000000);
	glVertexAttribDivisor(7, 1000000);


	glBindVertexArray(0);
}


Mesh::Mesh(aiMesh * mesh, const aiScene * scene, std::vector<Texture> textures, std::vector <glm::mat4> instance_mat4){
	std::cout << "MESH CONSTRUCTOR WITH MAT4 INSTANCE CALLED"<<std::endl;
	//return;
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
	glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 3*sizeof(aiVector3D) ) + instance_mat4.size()*sizeof(glm::mat4) + sizeof(float), NULL, GL_STATIC_DRAW);
	if(glGetError() == GL_OUT_OF_MEMORY)
		std::cout<<"ERROR::MESH:: GL_OUT_OF_MEMORY "<<mesh->mNumVertices * ( 3*sizeof(aiVector3D) ) + instance_mat4.size()*sizeof(glm::mat4)<<std::endl;

	glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mVertices);
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mNormals);
	if(mesh->mTextureCoords[0]){
		glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * ( 2*sizeof(aiVector3D) ), mesh->mNumVertices * (sizeof(aiVector3D)), mesh->mTextureCoords[0]);
	}
	this->mat4_ptr_offset = mesh->mNumVertices * (3*sizeof(aiVector3D));
	for(unsigned int i = 0; i < instance_mat4.size(); i++)
		glBufferSubData(GL_ARRAY_BUFFER, this->mat4_ptr_offset + i*sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(instance_mat4[i]));

	float some_float = 0.0;
	glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * (3*sizeof(aiVector3D)) + sizeof(glm::mat4)*instance_mat4.size(), sizeof(float), &some_float);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (sizeof(aiVector3D))));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(aiVector3D), (void*)(mesh->mNumVertices * (2*sizeof(aiVector3D))));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D))+ sizeof(glm::mat4)*instance_mat4.size()));
	glVertexAttribDivisor(3, 1000000);//это тупо - нужно просто отключить атрибут при рисовании. Включать его нужно тоже только при отрисовке.

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D)) + 0*sizeof(float)));
	glVertexAttribDivisor(4, 1);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D)) + 4*sizeof(float)));
	glVertexAttribDivisor(5, 1);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D)) + 8*sizeof(float)));
	glVertexAttribDivisor(6, 1);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(mesh->mNumVertices * (3*sizeof(aiVector3D)) + 12*sizeof(float)));
	glVertexAttribDivisor(7, 1);



	glBindVertexArray(0);
}

void Mesh::setMatrixByID(unsigned int id, glm::mat4 matrix){
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, this->mat4_ptr_offset + id*sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matrix));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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






















