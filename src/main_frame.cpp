
#include "main_frame.h"



MainFrame::MainFrame(){
    frame_level = 0;
    World = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
    World = glm::rotate(World, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    calcMatrices();    
}

void MainFrame::calcMatrices(){
    this->A = glm::translate(this->World, glm::vec3(0.0, 0.0, this->frame_level));
	this->rail1 = glm::translate(this->A, glm::vec3(350.0, 6760.0, -400.0));
	this->rail2 = glm::translate(this->A, glm::vec3(350.0, -245.0, -400.0));
	this->rail3 = glm::translate(this->A, glm::vec3(350.0, -7250.0, -400.0));
}

