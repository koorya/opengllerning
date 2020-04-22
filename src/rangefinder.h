
#ifndef RANGEFINDER_H
#define RANGEFINDER_H

#include "model.h"
#include <vector>
#include "cl_kernel_container.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

#include <nanogui/nanogui.h>

#include <mysql.h>
#include <thread>

class Rangefinder : public Ray{
	public:
	Rangefinder(glm::mat4 * parent_m4 = NULL);
	~Rangefinder();

	void calcProp();
	glm::mat4 get_intersect_mat();

	float distance;

	private:
	glm::mat4 * mat;


};

class RangefindersContainer{
	public:
	
	RangefindersContainer(int max_cnt = 10);
	
	void computeRays();
	void Draw(Shader shader);

	void addRangefinder(glm::mat4 * parent_m4);
	void addTarget(RayTarget * target);

	std::vector <Rangefinder> rangef_list;

	private:
	int max_rf_cnt;
	Shader * ray_shader;
	Model * sphere;
	Model * box;
	std::vector<float> ray_vertices;
	GLuint my_ray_vao;
	GLuint my_ray_vbo;

	protected:

	std::vector <RayTarget *> target_list;


};

class guiRangefindersContainer: virtual public RangefindersContainer{
	public:
	
	guiRangefindersContainer(nanogui::Screen * screen, int max_cnt = 10);
	void addRangefinder(glm::mat4 * parent_m4);
	void computeRays();
	private:
	nanogui::Window *ray_gui_window;
	nanogui::Screen * screen;
	std::vector<nanogui::TextBox *> text_boxes;

};

class sqlRangefindersContainer: virtual public RangefindersContainer{
	public:
	
	sqlRangefindersContainer();
	void updateBD();	

	private:
	MYSQL *conn;
	std::string SQL_query;
};

class guisqlRangefinderContainer: public guiRangefindersContainer, public sqlRangefindersContainer{
	public:
	guisqlRangefinderContainer(nanogui::Screen * screen, int max_cnt = 10);
	void computeRays();
};

#endif
