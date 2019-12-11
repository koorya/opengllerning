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

class GUIField{
public:
	virtual void updateState()=0;
};

class guiManipulator : public nanogui::Screen, public Manipulator {
public:
    guiManipulator();

    ~guiManipulator();

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);
    virtual void draw(NVGcontext *ctx);

    virtual void drawContents();
	void doStep();
private:

	static void updateThread(void * arg);

	float gui_config[8] = {0};
	std::mutex * config_mutex;/// < блокирует обновление данных, пока происходит пересчет матриц

	std::vector<GUIField*> gui_inputs;

};






#endif