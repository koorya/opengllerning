#include "model.h"

Model::Model(char * path){
	loadModel(path);
}

void Model::Draw(Shader shader){
	for(unsigned int i = 0; i < meshes.size(); i++){
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string path){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout<<"ERROR::ASSIMP::"<< import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode * node, const aiScene * scene){
	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}


Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene){

	std::vector <Vertex> vect_vertices;
	std::vector <unsigned int> vect_indices;
	std::vector <Texture> vect_textures;


	for(unsigned int i = 0; i < mesh->mNumVertices; i++){
		Vertex vertex;
		vertex.Position = glm::vec3 (mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3 (mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if(mesh->mTextureCoords[0]){
			vertex.TexCoords = glm::vec2 (mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}else{
			vertex.TexCoords = glm::vec2 (0.0f, 0.0f);
		}
		vect_vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++){
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
			vect_indices.push_back(face.mIndices[j]);
	}

	Mesh my_mesh(vect_vertices, vect_indices, vect_textures);

	return my_mesh;
}

