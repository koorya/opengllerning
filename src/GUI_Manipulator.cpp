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

    using namespace nanogui;

    Window *window = new Window(screen, "manip");
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



	nanogui::Button * button = new nanogui::Button(window, "pickUpBond");
	button->setCallback([&]{
		pickUpBond();
	});
	button = new nanogui::Button(window, "mountBond");
	button->setCallback([&]{
		mountBond();
	});
	screen->performLayout();
	
}

guiManipulator::~guiManipulator() {

}


void guiManipulator::doStep(){
	this->syncValues();
	int i=0;
	//config_mutex->lock();
	// config.tower = 360.0 * gui_config[i++];
	// config.bpant = 3000 * gui_config[i++];
	// config.bcar = 1550 * gui_config[i++];
	// config.wrist = 80.0 * (-1 + 2*gui_config[i++]);
	// config.brot = 360.0 * gui_config[i++];
	// config.cpant = 800 * gui_config[i++];
	// config.ccar = 450 * gui_config[i++];
	// config.rail = 1000 + 0.86 * 3000 * 6 * gui_config[i++];
	//config_mutex->unlock();
}

