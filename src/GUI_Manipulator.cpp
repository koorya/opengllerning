#include "GUI_Manipulator.h"

guiManipulator::guiManipulator() : GUIWindow() {

	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Перемещение вдоль рамы", std::pair<float, float> (0.0, 1000 + 0.86 * 3000 * 6 ), "mm", std::ref(this->config.rail)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Поворот башни", std::pair<float, float> (-110.0, 210.0), "°", std::ref(this->config.tower)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Пантограф связи", std::pair<float, float> (0.0, 2920.0), "mm", std::ref(this->config.bpant)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Каретка связи", std::pair<float, float> (0.0, 1645.0), "mm", std::ref(this->config.bcar)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Вращение связи 1", std::pair<float, float> (-95.0, 95.0), "°", std::ref(this->config.wrist)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Вращение связи 2", std::pair<float, float> (-180.0, 20.0), "°", std::ref(this->config.brot)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Пантограф колонны", std::pair<float, float> (0.0, 680.0), "mm", std::ref(this->config.cpant)));
	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Каретка колонны", std::pair<float, float> (0.0, 765.0), "mm", std::ref(this->config.ccar)));

	performLayout();

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

