#ifndef GUI_MANIPULATOR_H
#define GUI_MANIPULATOR_H

#include "manipulator.h"
#include <mutex>
#include <thread>


#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>

#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>

#include <nanogui/textbox.h>
#include <nanogui/slider.h>


#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/colorpicker.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <iostream>
#include <string>



// class guiManipulator : public Manipulator{
// public:

// 	};
// };


class guiManipulator : public nanogui::Screen, public Manipulator {
public:
    guiManipulator() : nanogui::Screen(Eigen::Vector2i(300, 550), "Управление манипулятором") {
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

        /* All NanoGUI widgets are initialized at this point. Now
           create an OpenGL shader to draw the main window contents.

           NanoGUI comes with a simple Eigen-based wrapper around OpenGL 3,
           which eliminates most of the tedious and error-prone shader and
           buffer object management.
        */

		//config_mutex = new std::mutex();

		std::thread thr(updateThread, this);
		thr.detach();

//		nanogui::mainloop();
    }



    ~guiManipulator() {

    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Animate the scrollbar */
//        mProgress->setValue(std::fmod((float) glfwGetTime() / 10, 1.0f));

        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() {
        using namespace nanogui;

        /* Draw the window contents using OpenGL */

    }
	void doStep(){
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

private:
//    nanogui::ProgressBar *mProgress;

	static void updateThread(void * arg){
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
	};

	float gui_config[8] = {0};
	std::mutex * config_mutex;/// < блокирует обновление данных, пока происходит пересчет матриц


};




#endif