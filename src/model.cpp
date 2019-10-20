#include "model.h"
#include "load_tex.h"


Model::Model(const char * path, std::vector<float> rad_vect){
	this->rad_vect = rad_vect;


	loadModel(path);
}

Model::Model(const char * path, std::vector <glm::mat4> instance_mat4){
	this->mat4_vect = instance_mat4;

	loadModel(path);
}

void Model::Draw(Shader shader){
	for(unsigned int i = 0; i < meshes.size(); i++){
		meshes[i].Draw(shader);
	}
}
void Model::Draw(Shader shader, unsigned int count){
	for(unsigned int i = 0; i < meshes.size(); i++){
		meshes[i].Draw(shader, count);
	}
}
void Model::setMatrixByID(unsigned int id, glm::mat4 matrix){
	for(int i = 0; i < meshes.size(); i++)
		meshes[i].setMatrixByID(id, matrix);
}

void Model::loadModel(std::string path){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs );//| aiProcess_SplitLargeMeshes);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout<<"ERROR::ASSIMP::"<< import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	int vert_cnt = processNode(scene->mRootNode, scene);
	std::cout<<path<<" vertices count "<<vert_cnt<<std::endl;

}

int Model::processNode(aiNode * node, const aiScene * scene){
	int vert_cnt = 0;
	for(unsigned int i = 0; i < node->mNumMeshes; i++){
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
		vert_cnt += mesh->mNumVertices;
	}
	for(unsigned int i = 0; i < node->mNumChildren; i++){
		vert_cnt += processNode(node->mChildren[i], scene);
	}
	return vert_cnt;
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene){

	std::vector<Texture> textures;

	if(mesh->mMaterialIndex >= 0){
		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector <Texture> diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
		std::vector <Texture> specularMap = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
	}
	if(this->mat4_vect.size() != 0){
		std::cout << "mat4 constr"<<std::endl;
		return Mesh(mesh, scene, textures, mat4_vect);
	}
	return Mesh(mesh, scene, textures, rad_vect);

}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType tex_type){
	std::vector <Texture> textures;
	for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		std::string full_path = directory;
		full_path += "/";
		full_path += str.C_Str();

		bool skip = false;
		for(unsigned int i = 0; i < loaded_textures.size(); i++){
			if(std::strcmp(loaded_textures[i].path.c_str(), full_path.c_str())==0){
				skip = true;
				textures.push_back(loaded_textures[i]);
				break;
			}
		}
		if(!skip){
			texture.id = loadTexture(full_path.c_str());

			texture.type = tex_type;
			texture.path = full_path;
			textures.push_back(texture);
			loaded_textures.push_back(texture);
		}
	} 
	return textures;
}


