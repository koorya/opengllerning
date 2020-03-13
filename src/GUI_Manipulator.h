#ifndef GUI_MANIPULATOR_H
#define GUI_MANIPULATOR_H

#include "manipulator.h"

#include "gui.h"
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

#define _USE_MATH_DEFINES
#include <math.h>

#include "remote_manipulator.h"


class guiManipulator : public GUIWindow, virtual public Manipulator {
public:
	guiManipulator();
	guiManipulator(nanogui::Screen * screen);
	~guiManipulator();

	void doStep();

private:
protected:
	nanogui::Window *window;
	std::vector<nanogui::Button *> buttons;
};

class guiRemoteManipulator : public guiManipulator, public RemoteManipulator {
public:
	guiRemoteManipulator(nanogui::Screen * screen, unsigned int manip_id);
	void doStep();
private:
	bool database_upd;
};


#endif

