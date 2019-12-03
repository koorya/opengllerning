/*
    src/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes. For a Python implementation, see
    '../python/example1.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

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

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#  pragma warning(push)
#  pragma warning(disable: 4457 4456 4005 4312)
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication(float & float_var) : nanogui::Screen(Eigen::Vector2i(250, 500), "NanoGUI Test") {
        using namespace nanogui;

        // Window *window = new Window(this, "Button demo");
        Window *window = (Window*)this;
//        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());


        new Label(window, "Slider and text box", "sans-bold");

        Widget *panel;
        Slider *slider;
        TextBox *textBox;


        for(int i=0; i<7; i++){
            panel = new Widget(window);
            panel->setLayout(new BoxLayout(Orientation::Horizontal,
                                        Alignment::Middle, 0, 20));
            slider = new Slider(panel);
            slider->setValue(0.5f);
            slider->setFixedWidth(80);

            textBox = new TextBox(panel);
            textBox->setFixedSize(Vector2i(60, 25));
            textBox->setValue("50");
            textBox->setUnits("%");
            slider->setCallback([textBox, &float_var](float value) {
                float_var = value;
                textBox->setValue(std::to_string((int) (value * 100)));
            });
            // slider->setFinalCallback([&](float value) {
            //     cout << "Final slider value: " << (int) (value * 100) << endl;
            // });
            textBox->setFixedSize(Vector2i(60,25));
            textBox->setFontSize(20);
            textBox->setAlignment(TextBox::Alignment::Right);
        }
 

        performLayout();

        /* All NanoGUI widgets are initialized at this point. Now
           create an OpenGL shader to draw the main window contents.

           NanoGUI comes with a simple Eigen-based wrapper around OpenGL 3,
           which eliminates most of the tedious and error-prone shader and
           buffer object management.
        */

    }

    ~ExampleApplication() {

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
private:
//    nanogui::ProgressBar *mProgress;
};

