#include "gui.h"

SliderWithText::SliderWithText(nanogui::Widget *parent, const std::string &name, const std::pair<float, float> &range, const std::string &units, float &value_var){
    using namespace nanogui;
    value_var_ptr = &value_var;

    new Label(parent, name, "sans-bold");
    
    panel = new Widget(parent);
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


GUIWindow::GUIWindow()  {
}

float val_var;
std::pair<float, float> limits = std::pair<float, float>(10.0f, 100.0f);
GUIWindow::GUIWindow(nanogui::Screen * screen)  {




    // using namespace nanogui;

	// gui = new FormHelper(screen);
	// nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper gui");
	// gui->addGroup("Other widgets");
	// gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
    // SliderWithText * slider = new SliderWithText((nanogui::Widget*)nanoguiWindow, "serega", std::ref(limits),"cm", std::ref(val_var));


 //   gui->addWidget("", slider->panel);
	screen->setVisible(true);
	screen->performLayout();
//	nanoguiWindow->center();

}

GUIWindow::~GUIWindow() {

}


void GUIWindow::syncValues(){
	for(int i = 0; i < gui_inputs.size(); i++)
		gui_inputs[i]->updateState();
	int i=0;

	// GLFWwindow * prew_window = glfwGetCurrentContext();

	// glfwMakeContextCurrent(glfwWindow());
	// glfwPollEvents();

	// glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
	// glClear(GL_COLOR_BUFFER_BIT);

	// // Draw nanogui
	// this->drawContents();
	// this->drawWidgets();

	// glfwSwapBuffers(glfwWindow());
	// glfwMakeContextCurrent(prew_window);
}


