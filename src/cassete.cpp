
#include "cassete.h"

CasseteCell::CasseteCell(ConstructionContainer * container): noempty(false), container(container){

}

void CasseteCell::attachBond(struct BondLocation bond){
	if(!noempty){
		noempty = true;
		container->attachBond(&matr, bond);
	}
}

bool CasseteCell::getBond(const glm::mat4 * matr){
	std::cout<<"CasseteCell::getBond"<<std::endl;
	glm::vec4 r1(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 r2(0.0f, 0.0f, 0.0f, 1.0f);
	r1 = this->matr * r1;
	r2 = (*matr) * r2;
	r1 -= r2;
	std::cout<<"glm::dot(r1, r1) "<<glm::dot(r1, r1)<<std::endl;
	if(glm::dot(r1, r1) < 5000 && noempty){
		container->reattach(&(this->matr), matr);
		noempty = false;
	}
	return noempty;
}

Cassete::Cassete(ConstructionContainer * container){
	for(int i = 0; i<8; i++){
		places[i] = new CasseteCell(container);
	}
}
Cassete::~Cassete(){
	for(int i = 0; i<8; i++){
		delete places[i];
	}
}

void Cassete::fillUp(){
	struct BondLocation bond;
	bond.name = 2; //tilted_bond
	bond.name = 1; //horizontal_bond
	for (int i=0; i<8; i++){
		places[i]->attachBond(bond);
	}
}

int Cassete::getBond(const glm::mat4 * matr){
	int i;
	for(int i = 7; i>=0; i--){
		if(places[i]->noempty){
			return places[i]->getBond(matr);
		}
	}
	return 0;
}

void Cassete::updateMatrices(const glm::mat4 * parent){
	float param[5];
	param[0] = 190 - 2255.0; //вдоль рельсов
	param[1] = -197;
	param[2] = -3024;//-3087;//3496, 3433
	param[3] = 155;
	param[4] = 400;
	glm::mat4 origin = * parent;
	origin = glm::translate(origin, glm::vec3(param[0], param[1], param[2]));
	for (int i = 0; i < 8; i++){
		places[i]->matr = glm::translate(origin, glm::vec3(param[3]*(i/2), param[4]*(i%2), 0.0f));
	}
}



GUICassete::GUICassete(nanogui::Screen * screen, ConstructionContainer * container) : GUIWindow(screen), Cassete(container){

    using namespace nanogui;

    Window *window = new Window(screen, "Cassete");
    window->setPosition(Vector2i(15, 15));
    window->setLayout(new GroupLayout());

	nanogui::Button * button = new nanogui::Button((nanogui::Window*)window, "FillUp");
	button->setCallback([&]{
		this->fillUp();
	});
	screen->performLayout();
	
}

GUICassete::~GUICassete() {

}


void GUICassete::doStep(){
	this->syncValues();
}