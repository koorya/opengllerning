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



class guiManipulator : public GUIWindow, public Manipulator {
public:
	guiManipulator();

	~guiManipulator();

	void doStep();

	glm::mat4 getPosition();/// < выдает матрицу перехода из координат тележки в позицию конечной точки


	void syncmove(){

		float _phi = glm::radians(this->config.tower.value + this->config.wrist.value); // поворот башни, поворот кисти
		float _theta = glm::radians(this->config.brot.value); // вращение связи
	

		float t = 10; ///< шаг в миллиметрах
		glm::vec3 add_pos;
		//вдоль балки (проверил, точно работает)
		add_pos = glm::vec3(    t * cos(_phi) * sin(_theta), 
								-t * sin(_phi) * sin(_theta), 
								-t * cos(_theta));
	// //ортогонально балке вдоль кисти (проверил, работает)
	// 	add_pos = glm::vec3(-t * cos(_phi - M_PI_2), 
	// 						t * sin(_phi - M_PI_2), 
	// 						0.0f);
	// //ортогонально балке ортогонально кисти
	// 	add_pos = glm::vec3(-t * cos(_phi) * sin(_theta  +M_PI_2), 
	// 						t * sin(_phi) * sin(_theta + M_PI_2), 
	// 						t * cos(_theta + M_PI_2));

		glm::vec3 res_pos = this->getTarget();// + add_pos;

		struct ManipulatorConfig conf = this->getConfigByTarget(res_pos, _theta, _phi); 
		
		this->setConfig(conf);
	}


	glm::vec3 getTarget(){

		float l = 639.0f + this->config.bpant.value;
		float d = 300;
		float phi = this->config.tower.value;
		float psy = this->config.wrist.value;
		float x = -(l * cos(phi) + d * cos(phi+psy));
		float y = -(l * sin(phi) + d * sin(phi+psy));

	//	glm::vec4 cur_vec = (this->H - this->C) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return glm::vec3(x, y, 0.0f) ;
	}


	struct ManipulatorConfig getConfigByTarget(glm::vec3 pos_vect, float Theta, float Phi){	// x, y, z, Theta, Phi

	//	float l = 639.0f + this->config.bpant.value;
		float d = 300;

		float x = pos_vect.x;
		float y = pos_vect.y;
		float z = pos_vect.z;

		float m[] = {x + d * cos(Phi), y + d * sin(Phi)};
		float psy1 = M_PI - atan2(m[1], m[0]);
		float psy2 = Phi - psy1;

		
//		float r = sqrt(m[0]*m[0]+m[1]*m[1]);
		float l = sqrt(m[0]*m[0]+m[1]*m[1]) - 639.0f;


		struct ManipulatorConfig ret;
		ret.tower.value = glm::degrees(psy1);
		ret.bpant.value = l;
//		ret.bcar.value = td3v;
		ret.wrist.value = glm::degrees(psy2);
//		ret.brot.value = glm::degrees(Theta);

		return ret;
	}



	void setConfig(struct ManipulatorConfig &_config){
		this->config.tower.value = _config.tower.value;
		this->config.bpant.value = _config.bpant.value;
//		this->config.bcar.value = _config.bcar.value; 
		this->config.wrist.value = _config.wrist.value;
//		this->config.brot.value = _config.brot.value; 
	}

private:

};




#endif