#include "GUI_Manipulator.h"


guiManipulator::guiManipulator() : nanogui::Screen(Eigen::Vector2i(300, 550), "Управление манипулятором") {
    using namespace nanogui;

    // Window *window = new Window(this, "Button demo");
    Window *window = (Window*)this;
    window->setPosition(Vector2i(15, 15));
    window->setLayout(new GroupLayout());

    Widget *panel;
    Slider *slider;
    TextBox *textBox;

    std::string handle_names[] = {
                        "Поворот башни",
                        "Пантограф связи",
                        "Каретка связи", 
                        "Вращение связи 1",
                        "Вращение связи 2", 
                        "Пантограф колонны",
                        "Каретка колонны", 
                        "Перемещение вдоль рамы" };  
    for(int i=0; i<8; i++){
        new Label(window, handle_names[i], "sans-bold");
        panel = new Widget(window);
        panel->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 0, 5));
        slider = new Slider(panel);
        slider->setValue(0.0f);

        slider->setFixedWidth(150);

        textBox = new TextBox(panel);

        textBox->setEditable(true);
        textBox->setDefaultValue("0.0");

        textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");

        textBox->setFixedSize(Vector2i(60, 25));
        textBox->setValue("0");
        textBox->setUnits("%");
        textBox->setCallback([&, slider, i](const std::string& value_str){
            float val = std::stof(value_str)/ 100.0;
            //config_mutex->lock();
            this->gui_config[i] = val;
            //config_mutex->unlock();
            slider->setValue((val)); 
            return true;           
        });
        slider->setCallback([&, textBox, i](float value) {
            //config_mutex->lock();
            this->gui_config[i] = value;
            //config_mutex->unlock();
            textBox->setValue(std::to_string((int) (value * 100)));
        });
        // slider->setFinalCallback([&](float value) {
        //     cout << "Final slider value: " << (int) (value * 100) << endl;
        // });
        textBox->setFontSize(20);
        textBox->setAlignment(TextBox::Alignment::Right);

        if(i == 3){
            slider->setValue(0.5f);
            this->gui_config[i] = 0.5;
            textBox->setValue(std::to_string((int) (0.5 * 100)));
        }

    }
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
    int i=0;
    //config_mutex->lock();
    config.tower = 360.0 * gui_config[i++];
    config.bpant = 3000 * gui_config[i++];
    config.bcar = 1550 * gui_config[i++];
    config.wrist = 80.0 * (-1 + 2*gui_config[i++]);
    config.brot = 360.0 * gui_config[i++];
    config.cpant = 800 * gui_config[i++];
    config.ccar = 450 * gui_config[i++];
    config.rail = 1000 + 0.86 * 3000 * 6 * gui_config[i++];
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
