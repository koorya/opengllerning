#include "GUI_MainFrame.h"

guiMainFrame::guiMainFrame() : GUIWindow(){

	gui_inputs.push_back((GUIField*) new SliderWithText((nanogui::Window*)this, "Уровень силовой рамы", std::pair<float, float> (0.0, 3000 * 3 ), "mm", std::ref(this->frame_level)));

	performLayout();


}

guiMainFrame::~guiMainFrame(){

}

void guiMainFrame::doStep(){
	this->syncValues();
	this->calcMatrices();


}