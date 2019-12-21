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

	float _phi = glm::radians(90.0f - this->config.tower.value - this->config.wrist.value); // поворот башни, поворот кисти
	float _theta = glm::radians(this->config.brot.value); // вращение связи
	

    float t = 1; ///< шаг в миллиметрах
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

		res_pos = manip.getTarget() + add_pos

		conf = manip.getConfigByTarget(res_pos)
		
		manip.setConfig(conf)
    }


	void getTarget(self){
		cur_vec = np.array([0, 0, 0, 1])
		for i in reversed(self.joints):
			cur_vec = np.dot(i.H, cur_vec)
		cur_vec += np.array([self.position[0], self.position[1], self.position[2], 0])
		return np.array([cur_vec[0], -cur_vec[1], cur_vec[2], self.joints[4].q, 90 - self.joints[0].q - self.joints[3].q])
    }


	void getConfigByTarget(self, pos_vect){	// x, y, z, Theta, Phi
		global d_1, d_3_c, d_2_1, d_2_2, d_5
		
		a = d_5	
		x, y, z = pos_vect[0] - self.position[0], pos_vect[1]-self.position[1], pos_vect[2] - self.position[2]

		Theta = np.radians(pos_vect[3])
		Phi = np.radians(pos_vect[4])
		Phi = Phi
		#y = -y
		m = [x - a*np.sin(Phi), y-(a*np.cos(Phi))]
		psy1 = np.arctan2(m[1], m[0])
		psy2 = np.pi/2 - psy1 - Phi

		
		r = np.sqrt(m[0]*m[0]+m[1]*m[1])
#		print "r: %.2f"%r
		
		td2v = (r - d_2_1 - d_2_2)
#		td3v = (z - d_3_c - d_1)
		td3v = (z - d_3_c - d_1)
		return (np.rad2deg(psy1), td2v, td3v, np.rad2deg(psy2), np.rad2deg(Theta))
    }



	void setConfig(self, config){
		j = 0
		update_flag = 0
		for i in self.joints:
			if(i.q != config[j]):
				update_flag = 1
			i.q = config[j]
			j += 1
		if update_flag == 1:
			self.calcMatrixes()
			print "set config res:",self.getConfig()
			print "set congig res pos", self.getTarget()
    }

private:


};




#endif