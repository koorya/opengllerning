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


GUIWindow::GUIWindow() : nanogui::Screen() {
	GLFWwindow * prew_window = glfwGetCurrentContext();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(300, 550, "example3", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    this->initialize(window, false);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);


    this->setVisible(true);
    this->performLayout();
	glfwSetWindowUserPointer(window, (void*)this);
	
	using namespace nanogui;
    glfwSetCursorPosCallback(window,
            [](GLFWwindow * window, double x, double y) {
				Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
            	screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow * window, int button, int action, int modifiers) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(window,
        [] (GLFWwindow * window, int key, int scancode, int action, int mods) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->keyCallbackEvent(key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow * window, unsigned int codepoint) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow * window, int count, const char **filenames) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow * window, double x, double y) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->scrollCallbackEvent(x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow * window, int width, int height) {
            Screen * screen = (Screen *)glfwGetWindowUserPointer(window);
			screen->resizeCallbackEvent(width, height);
        }
    );



	glfwMakeContextCurrent(prew_window);
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

	GLFWwindow * prew_window = glfwGetCurrentContext();

	glfwMakeContextCurrent(glfwWindow());
	glfwPollEvents();

	glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw nanogui
	this->drawContents();
	this->drawWidgets();

	glfwSwapBuffers(glfwWindow());
	glfwMakeContextCurrent(prew_window);
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
