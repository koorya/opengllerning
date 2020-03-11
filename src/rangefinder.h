
#ifndef RANGEFINDER_H
#define RANGEFINDER_H

//#include "model.h"
#include <vector>
#include "cl_kernel_container.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

class Rangefinder : public Ray{
	public:
	Rangefinder(glm::mat4 * parent_m4 = NULL);
	~Rangefinder();

	void calcProp();

	float distance;

	private:
	glm::mat4 * mat;


};

class RangefindersContainer{
	public:
	
	RangefindersContainer(int max_cnt = 10);
	
	void computeRays();

	private:
//	Model * sphere;
//	Model * box;

	std::vector <Rangefinder> rangef_list;

	std::vector <RayTarget *> target_list;

};

#endif
