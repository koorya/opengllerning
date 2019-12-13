#include "gui.h"

SliderWithText::SliderWithText(nanogui::Window *window, const std::string &name, const std::pair<float, float> &range, const std::string &units, float &value_var){
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

void SliderWithText::updateState(){
    slider->setValue((*value_var_ptr));
    textBox->setValue(std::to_string((int) (*value_var_ptr)));
}


GUIWindow::GUIWindow() : nanogui::Screen(Eigen::Vector2i(300, 550), "Управление манипулятором") {
	using namespace nanogui;

	// Window *window = new Window(this, "Button demo");
	Window *window = (Window*)this;
	window->setPosition(Vector2i(15, 15));
	window->setLayout(new GroupLayout());

	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Перемещение вдоль рамы", std::pair<float, float> (0.0, 1000 + 0.86 * 3000 * 6 ), "mm", std::ref(this->config.rail)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Поворот башни", std::pair<float, float> (-110.0, 210.0), "°", std::ref(this->config.tower)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Пантограф связи", std::pair<float, float> (0.0, 2920.0), "mm", std::ref(this->config.bpant)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Каретка связи", std::pair<float, float> (0.0, 1645.0), "mm", std::ref(this->config.bcar)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Вращение связи 1", std::pair<float, float> (-95.0, 95.0), "°", std::ref(this->config.wrist)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Вращение связи 2", std::pair<float, float> (-180.0, 20.0), "°", std::ref(this->config.brot)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Пантограф колонны", std::pair<float, float> (0.0, 680.0), "mm", std::ref(this->config.cpant)));
	// gui_inputs.push_back((GUIField*) new SliderWithText(window, "Каретка колонны", std::pair<float, float> (0.0, 765.0), "mm", std::ref(this->config.ccar)));

	performLayout();

	std::thread thr(updateThread, this);
	thr.detach();
}


GUIWindow::~GUIWindow() {

}

bool GUIWindow::keyboardEvent(int key, int scancode, int action, int modifiers) {
	if (Screen::keyboardEvent(key, scancode, action, modifiers))
		return true;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		setVisible(false);
		return true;
	}
	return false;
}

void GUIWindow::draw(NVGcontext *ctx) {
	/* Animate the scrollbar */
//        mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

	/* Draw the user interface */
	Screen::draw(ctx);
}

void GUIWindow::drawContents() {
	using namespace nanogui;

	/* Draw the window contents using OpenGL */

}

void GUIWindow::syncValues(){
	for(int i = 0; i < gui_inputs.size(); i++)
		gui_inputs[i]->updateState();
	int i=0;
}

void GUIWindow::updateThread(void * arg){
	GUIWindow * arg_ = (GUIWindow*)arg;
	arg_->setVisible(true);
	glfwMakeContextCurrent(arg_->glfwWindow());
	arg_->drawAll();
	std::chrono::milliseconds time(5);
	while(!glfwWindowShouldClose(arg_->glfwWindow())){
//		std::this_thread::sleep_for(time);
		arg_->drawAll();
	}
	arg_->setVisible(false);

	return;
}
