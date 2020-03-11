#include "model.h"
#include "load_tex.h"


Model::Model(const char * path, std::vector <glm::mat4> instance_mat4, clKernelsContainer * cl_kernel_cont): cl_kernel_cont(cl_kernel_cont){
	this->mat4_vect = instance_mat4;

	loadModel(path);
}

Model::Model(const char * path, unsigned int max_inst_cnt, clKernelsContainer * cl_kernel_cont): cl_kernel_cont(cl_kernel_cont){
	
	std::vector <glm::mat4> instance_mat4;
	for (int i = 0; i < max_inst_cnt; i++)
		instance_mat4.push_back(glm::mat4(1.0f));
	this->mat4_vect = instance_mat4;
	
	loadModel(path);
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

float Model::computeRay(const Ray * ray, int inst_cntinst_cnt){
	float ret = -1.0f;
	for(int i = 0; i < meshes.size(); i++){
		float curr_dist = meshes[i].computeRay(ray, inst_cntinst_cnt);
		if(curr_dist > 0){
			if(ret<0)
				ret = curr_dist;
			if(ret > curr_dist)
				ret = curr_dist;
		}
	}
	return ret;
}




void Model::loadModel(std::string path){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs );//| aiProcess_SplitLargeMeshes);
	this->path = path;
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout<<"ERROR::ASSIMP::"<< import.GetErrorString() << std::endl << path << std::endl;
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
extern bool print_vert;
ClMesh Model::processMesh(aiMesh * mesh, const aiScene * scene){

	std::vector<Texture> textures;

	if(mesh->mMaterialIndex >= 0){

		aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
		aiString mat_name;
		if(AI_SUCCESS != material->Get(AI_MATKEY_NAME, mat_name)){
			std::cout<<path<<" NO MATERIAL NAME"<<std::endl;
		}else{
			std::cout<<path<<" | MATERIAL NAME: "<<mat_name.C_Str()<<std::endl;
			aiColor3D diffuse (0.f, 0.f, 0.f);
			if(AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)){
				std::cout<<path<<" NO MATERIAL DIFFUSE COLOR"<<std::endl;
			}else{
				std::cout<<path<<" | DIFFUSE COLOR: "<<diffuse.r<<", "<<diffuse.g<<", "<<diffuse.b<<std::endl;
			}

		}

		std::vector <Texture> diffuseMap = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());
		std::vector <Texture> specularMap = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());
	}

	std::cout << "mat4 constr"<<std::endl;
	return ClMesh(cl_kernel_cont, mesh, scene, textures, mat4_vect, print_vert);


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


