#include "GUI_Manipulator.h"

class SliderWithText : public GUIField{
public:
	SliderWithText(nanogui::Window *window, const std::string &name, const std::pair<float, float> &range, const std::string &units, float &value_var){
		using namespace nanogui;
		value_var_ptr = &value_var;

		new Label(window, name, "sans-bold");
		
		panel = new Widget(window);
		panel->setLayout(new BoxLayout(Orientation::Horizontal,
									Alignment::Middle, 0, 1));
		slider = new Slider(panel);
		slider->setValue(value_var);

		slider->setFixedWidth(150);

		textBox = new TextBox(panel);

		textBox->setEditable(true);
		textBox->setDefaultValue("0.0");

		textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");

		textBox->setFixedSize(Vector2i(90, 25));
		textBox->setValue("0");
		textBox->setUnits(units);
		textBox->setCallback([&](const std::string& value_str){
			float val = std::stof(value_str);
			//config_mutex->lock();
			value_var = val;
			//config_mutex->unlock();
			slider->setValue((val)); 
			return true;           
		});
		slider->setRange(range);
		slider->setCallback([&](float value) {
			//config_mutex->lock();
			value_var = value;
			//config_mutex->unlock();
			textBox->setValue(std::to_string((int) (value)));
		});


		// slider->setFinalCallback([&](float value) {
		//     cout << "Final slider value: " << (int) (value * 100) << endl;
		// });
		textBox->setFontSize(20);
		textBox->setAlignment(TextBox::Alignment::Right);		
	}

	virtual void updateState(){
		slider->setValue((*value_var_ptr));
		textBox->setValue(std::to_string((int) (*value_var_ptr)));
	}
private:
	float * value_var_ptr;
	nanogui::Widget *panel;
	nanogui::Slider *slider;
	nanogui::TextBox *textBox;
};

guiManipulator::guiManipulator() : nanogui::Screen(Eigen::Vector2i(300, 550), "Управление манипулятором") {
	using namespace nanogui;

	// Window *window = new Window(this, "Button demo");
	Window *window = (Window*)this;
	window->setPosition(Vector2i(15, 15));
	window->setLayout(new GroupLayout());

	std::string handle_names[] = {
						"Поворот башни",
						"Пантограф связи",
						"Каретка связи", 
						"Вращение связи 1",
						"Вращение связи 2", 
						"Пантограф колонны",
						"Каретка колонны", 
						"Перемещение вдоль рамы" };  


	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Перемещение вдоль рамы", std::pair<float, float> (0.0, 1000 + 0.86 * 3000 * 6 ), "mm", std::ref(this->config.rail)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Поворот башни", std::pair<float, float> (-110.0, 210.0), "°", std::ref(this->config.tower)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Пантограф связи", std::pair<float, float> (0.0, 2920.0), "mm", std::ref(this->config.bpant)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Каретка связи", std::pair<float, float> (0.0, 1645.0), "mm", std::ref(this->config.bcar)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Вращение связи 1", std::pair<float, float> (-95.0, 95.0), "°", std::ref(this->config.wrist)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Вращение связи 2", std::pair<float, float> (-180.0, 20.0), "°", std::ref(this->config.brot)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Пантограф колонны", std::pair<float, float> (0.0, 680.0), "mm", std::ref(this->config.cpant)));
	gui_inputs.push_back((GUIField*) new SliderWithText(window, "Каретка колонны", std::pair<float, float> (0.0, 765.0), "mm", std::ref(this->config.ccar)));

	performLayout();

	std::thread thr(updateThread, this);
	thr.detach();
}


guiManipulator::~guiManipulator() {

}

bool guiManipulator::keyboardEvent(int key, int scancode, int action, int modifiers) {
	if (Screen::keyboardEvent(key, scancode, action, modifiers))
		return true;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		setVisible(false);
		return true;
	}
	return false;
}

void guiManipulator::draw(NVGcontext *ctx) {
	/* Animate the scrollbar */
//        mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

	/* Draw the user interface */
	Screen::draw(ctx);
}

void guiManipulator::drawContents() {
	using namespace nanogui;

	/* Draw the window contents using OpenGL */

}

void guiManipulator::doStep(){
	for(int i = 0; i < gui_inputs.size(); i++)
		gui_inputs[i]->updateState();
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

void guiManipulator::updateThread(void * arg){
	guiManipulator * arg_ = (guiManipulator*)arg;
	arg_->setVisible(true);
	glfwMakeContextCurrent(arg_->glfwWindow());
	arg_->drawAll();
	std::chrono::milliseconds time(50);
	while(!glfwWindowShouldClose(arg_->glfwWindow())){
		std::this_thread::sleep_for(time);
		arg_->drawAll();
	}
	arg_->setVisible(false);

	return;
}
