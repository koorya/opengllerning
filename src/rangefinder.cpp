
#include "rangefinder.h"

Rangefinder::Rangefinder(glm::mat4 * parent_m4): mat(parent_m4){
	distance = -1.0f;
}

Rangefinder::~Rangefinder(){

}

void Rangefinder::calcProp(){
	if(mat == NULL)
		return;
	glm::vec3 my_glm_origin;
	my_glm_origin = (*mat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	origin.v4[0] = my_glm_origin.x;
	origin.v4[1] = my_glm_origin.y;
	origin.v4[2] = my_glm_origin.z;

	glm::vec3 my_glm_dir;
	my_glm_dir = (*mat) * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	dir.v4[0] = my_glm_dir.x;
	dir.v4[1] = my_glm_dir.y;
	dir.v4[2] = my_glm_dir.z;

}

RangefindersContainer::RangefindersContainer(int max_cnt){


}

void RangefindersContainer::computeRays(){
	for(auto ray = rangef_list.begin(); ray != rangef_list.end(); ray++){
		(*ray).calcProp();
		float ret = -1.0f;
		for(auto target = target_list.begin(); target != target_list.end(); target++){
			float _ret;
			_ret = (*target)->computeRay(&(*ray));

			if(	_ret > 0 && ( ret < 0 || ret > _ret ))
				ret = _ret;
		}
		(*ray).distance = ret;
	}
}