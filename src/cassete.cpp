
#include "cassete.h"

CasseteCell::CasseteCell(ConstructionContainer * container): noempty(false), container(container){

}

void CasseteCell::attachBond(struct BondLocation bond){
	if(!noempty){
		noempty = true;
		container->attachBond(&matr, bond);
	}
}

void CasseteCell::getBond(const glm::mat4 * matr){
	std::cout<<"CasseteCell::getBond"<<std::endl;
	container->reattach(&(this->matr), matr);
	noempty = false;
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
		std::cout<<i<<std::endl;
		if(places[i]->noempty){
			places[i]->getBond(matr);
			break;
		}
	}
	return 0;
}

void Cassete::updateMatrices(const glm::mat4 * parent){
	float param[5];
	param[0] = 190;
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