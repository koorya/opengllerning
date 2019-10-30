#include "ConstructionContainer.h"


ConstructionContainer::ConstructionContainer(){
	tilted_bond_cnt = 48*19;
	horizontal_bond_cnt = 24*19;
	column_cnt = 16*20;

	float stride = 3500.0;;
	glm::vec3 column_offset = glm::vec3(-5.600e+03, 507.50f, 5.005e+03); 
	glm::vec3 hor_bond_offset = glm::vec3(0, 127.0f, -540.0); 
//	glm::vec3 tilt_bond_offset = glm::vec3(0, 600.0f, -425.0); 
	glm::vec3 tilt_bond_offset = glm::vec3(0, 600.0f, -463.0); 

	std::vector<glm::mat4> column_matrices;
	std::vector<glm::mat4> hor_bond_matrices;
	std::vector<glm::mat4> tilt_bond_matrices;
	glm::mat4 model;

	for(int floor = 0; floor < 26; floor++){
		for(int x = 0; x < 4; x++){
			for(int y = 0; y< 4; y++){

				model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
				model = glm::translate(model, glm::vec3(x*stride, -(floor+1)*3000.0, y*stride) - column_offset);
				model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
				column_matrices.push_back(model);

				if(floor == 0)
					continue;

				if(y < 3){
					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset - hor_bond_offset);
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::red_plastic);
					// horizontal_bond.Draw(ourShader);
					hor_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, -tilt_bond_offset);
					model = glm::rotate(model, glm::radians(63.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::translate(model, glm::vec3(0.0f, 0.0f, 30.0f));
					tilt_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, glm::vec3(0.0, 0.0, 2*tilt_bond_offset.z + stride) );
					model = glm::translate(model, -tilt_bond_offset );
					model = glm::rotate(model, glm::radians(117.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::translate(model, glm::vec3(0.0f, 0.0f, 30.0f));
					tilt_bond_matrices.push_back(model);
				}

				if(x < 3){
					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -hor_bond_offset);
					// glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
					// ourShader.setMaterial(Material::red_plastic);
					// horizontal_bond.Draw(ourShader);
					hor_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -tilt_bond_offset);
					model = glm::rotate(model, glm::radians(63.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::translate(model, glm::vec3(0.0f, 0.0f, 30.0f));
					tilt_bond_matrices.push_back(model);

					model = glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
					model = glm::translate(model, glm::vec3(x*stride, -(floor)*3000.0, y*stride) - column_offset);
					model = glm::translate(model, glm::vec3(2*tilt_bond_offset.z + stride, 0.0, 0.0) );
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
					model = glm::translate(model, -tilt_bond_offset );
					model = glm::rotate(model, glm::radians(117.0f), glm::vec3(1.0, 0.0, 0.0));
					model = glm::translate(model, glm::vec3(0.0f, 0.0f, 30.0f));
					tilt_bond_matrices.push_back(model);
				}
			}
		}
	}

	tilted_bond = new Model("./3d_models/stl_components/tilted_bond.stl", tilt_bond_matrices);
	horizontal_bond = new Model("./3d_models/stl_components/horizontal_bond.stl", hor_bond_matrices);
	column = new Model("./3d_models/stl_components/column_light.stl", column_matrices);
}


ConstructionContainer::~ConstructionContainer(){
	delete tilted_bond;
	delete horizontal_bond;
	delete column;
}

void ConstructionContainer::updateMatrices(){
	
	for(auto i = tilted_bond_list.begin(); i != tilted_bond_list.end(); i++){
		tilted_bond->setMatrixByID(i->id, *(i->matr));
	}
	for(auto i = horizontal_bond_list.begin(); i != horizontal_bond_list.end(); i++){
		horizontal_bond->setMatrixByID(i->id, *(i->matr));
	}
	for(auto i = column_list.begin(); i != column_list.end(); i++){
		column->setMatrixByID(i->id, *(i->matr));
	}
}

void ConstructionContainer::drawElements(Shader shader){

	shader.setMaterial(Material::white_rubber);
	column->Draw(shader, column_cnt);

	shader.setMaterial(Material::copper);
	horizontal_bond->Draw(shader, horizontal_bond_cnt);

	shader.setMaterial(Material::green_plastic);
	tilted_bond->Draw(shader, tilted_bond_cnt);

}

void ConstructionContainer::attachBond(const glm::mat4 * matr, struct BondLocation bond){
	Attachment new_att;
	new_att.matr = matr;

	if( (bond.name == 1 ) || 
		(bond.name == 10) ||
		(bond.name == 11) ||
		(bond.name == 12) ||
		(bond.name == 13) ){

		new_att.id = horizontal_bond_cnt++;
		horizontal_bond_list.push_back(new_att);
	}else{
		new_att.id = tilted_bond_cnt++;
		tilted_bond_list.push_back(new_att);
	}
}

void ConstructionContainer::detach(const glm::mat4 * matr){
	
	for(auto i = tilted_bond_list.begin(); i != tilted_bond_list.end(); i++){
		if(i->matr == matr){
			auto tmp = i++;
			tilted_bond_list.erase(tmp);
		}
	}

	for(auto i = horizontal_bond_list.begin(); i != horizontal_bond_list.end(); i++){
		if(i->matr == matr){
			auto tmp = i++;
			horizontal_bond_list.erase(tmp);
		}
	}

    for(auto i = column_list.begin(); i != column_list.end(); i++){
        if(i->matr == matr){
            auto tmp = i++;
            column_list.erase(tmp);
        }
    }	

}

