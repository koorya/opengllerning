#include "GUI_Manipulator.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

class MethodButton : public GUIField{
public:
	MethodButton(nanogui::Window *window, const std::function<void ()> &callback){
	}
	virtual void updateState();
private:
	float * value_var_ptr;
	nanogui::Widget *panel;
	nanogui::Slider *slider;
	nanogui::TextBox *textBox;
};





guiManipulator::guiManipulator(nanogui::Screen * screen) : GUIWindow(screen) {
	std::cout<<"guiManipulator()"<<std::endl;
    using namespace nanogui;

    window = new Window(screen, "manip");
    window->setPosition(Vector2i(15, 15));
    window->setLayout(new GroupLayout());


	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Widget*)(window), "Перемещение вдоль рамы", std::ref(this->config.rail.limits), "mm", std::ref(this->config.rail.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Поворот башни", std::ref(this->config.tower.limits), "°", std::ref(this->config.tower.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Пантограф связи", std::ref(this->config.bpant.limits), "mm", std::ref(this->config.bpant.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Каретка связи", std::ref(this->config.bcar.limits), "mm", std::ref(this->config.bcar.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Вращение связи 1", std::ref(this->config.wrist.limits), "°", std::ref(this->config.wrist.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Вращение связи 2", std::ref(this->config.brot.limits), "°", std::ref(this->config.brot.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Пантограф колонны", std::ref(this->config.cpant.limits), "mm", std::ref(this->config.cpant.value)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)window, "Каретка колонны", std::ref(this->config.ccar.limits), "mm", std::ref(this->config.ccar.value)));



	buttons.push_back(new nanogui::Button(window, "pickUpBond"));
	(*--buttons.end())->setCallback([&]{
		pickUpBond();
	});
	buttons.push_back(new nanogui::Button(window, "mountBond"));
	(*--buttons.end())->setCallback([&]{
		mountBond();
	});
	screen->performLayout();
	
}

guiManipulator::~guiManipulator() {
	delete window;
}


void guiManipulator::doStep(){
	this->syncValues();
	calculateMatrices();

}

guiRemoteManipulator::guiRemoteManipulator(nanogui::Screen * screen, unsigned int manip_id) : guiManipulator(screen), RemoteManipulator(manip_id), database_upd(false) {
	std::cout<<"guiRemoteManipulator()"<<std::endl;
	nanogui::Button * button = new nanogui::Button(window, "exit");
	button->setCallback([&]{
		exit(0);
	});
	nanogui:CheckBox * upd = new nanogui::CheckBox(window, "update");
	upd->setCallback([&](const bool v){
		database_upd = v;
		for(auto b = buttons.begin(); b != buttons.end(); b++)
			(*b)->setVisible(!v);
	});
	screen->performLayout();
}

void guiRemoteManipulator::doStep(){
	if(database_upd){
		RemoteManipulator::doStep();
	}else{
		guiManipulator::doStep();
	}
}
