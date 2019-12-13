#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "movement_program.h"
#include "ConstructionContainer.h"
#include <vector>

class MainFrame{
public:
	MainFrame();

	void calcMatrices();

	glm::mat4 World, A;

	glm::mat4 rail1;
	glm::mat4 rail2;
	glm::mat4 rail3;



	float frame_level; 

};


#endif