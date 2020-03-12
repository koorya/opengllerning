
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
	my_glm_dir = (*mat) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	dir.v4[0] = my_glm_dir.x;
	dir.v4[1] = my_glm_dir.y;
	dir.v4[2] = my_glm_dir.z;

}

glm::mat4 Rangefinder::get_intersect_mat(){
	return glm::translate(*mat, glm::vec3(distance, 0.0f, 0.0f));
}

RangefindersContainer::RangefindersContainer(int max_cnt):max_rf_cnt(max_cnt){

	ray_shader = new Shader("../shaders/ray.vert", "../shaders/ray.frag");
	sphere = new Model("../3D_models/sphere.obj", max_cnt);

	
	ray_vertices.resize(max_cnt * 6);
	for(int i = 0; i < ray_vertices.size(); i++)
		ray_vertices[i] = 0.0f;


	glGenVertexArrays(1, &my_ray_vao);
	glGenBuffers(1, &my_ray_vbo);
	glBindVertexArray(my_ray_vao);
	glBindBuffer(GL_ARRAY_BUFFER, my_ray_vbo);
	glBufferData(GL_ARRAY_BUFFER, ray_vertices.size()*sizeof(float), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, ray_vertices.size()*sizeof(float), ray_vertices.data());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	glBindVertexArray(0);
}

void RangefindersContainer::addRangefinder(glm::mat4 * parent_m4){
	if(rangef_list.size() < max_rf_cnt)
		rangef_list.push_back(Rangefinder(parent_m4));
}

void RangefindersContainer::addTarget(RayTarget * target){
	target_list.push_back(target);
}

void RangefindersContainer::Draw(Shader shader){

	ray_shader->use();

	glBindVertexArray(my_ray_vao);

	glBindBuffer(GL_ARRAY_BUFFER, my_ray_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ray_vertices.size()*sizeof(float), ray_vertices.data());

	glDrawArrays(GL_LINES, 0, rangef_list.size()*2);

	glBindVertexArray(0);

	shader.use();

	sphere->Draw(shader, rangef_list.size());

}

void RangefindersContainer::computeRays(){
	int i = 0;
	for(auto ray = rangef_list.begin(); ray != rangef_list.end(); ray++, i++){
		(*ray).calcProp();
		float ret = -1.0f;
		for(auto target = target_list.begin(); target != target_list.end(); target++){
			float _ret;
			_ret = (*target)->computeRay(&(*ray));

			if(	_ret > 0 && ( ret < 0 || ret > _ret ))
				ret = _ret;
		}
		(*ray).distance = ret;
		ray_vertices[i*6] 	= (*ray).origin.v4[0];
		ray_vertices[i*6+1] = (*ray).origin.v4[1];
		ray_vertices[i*6+2] = (*ray).origin.v4[2];

		ray_vertices[i*6+3] = (*ray).origin.v4[0] + (*ray).dir.v4[0]*ret;
		ray_vertices[i*6+4] = (*ray).origin.v4[1] + (*ray).dir.v4[1]*ret;
		ray_vertices[i*6+5] = (*ray).origin.v4[2] + (*ray).dir.v4[2]*ret;

		sphere->setMatrixByID(i, (*ray).get_intersect_mat());
	}
}