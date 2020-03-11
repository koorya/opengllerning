#ifndef CONSTRUCTIONCONTAINER_H
#define CONSTRUCTIONCONTAINER_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include "model.h"
#include "cl_kernel_container.h"
#include "rangefinder.h"

struct BondLocation{
	int section;
	int name;
};

struct Attachment{
	const glm::mat4 * matr;
	int id;
};

class ConstructionContainer:public RayTarget{
public:
	ConstructionContainer(clKernelsContainer * cl_kernel_cont=NULL);
	~ConstructionContainer();
	void attachBond(const glm::mat4 * matr, struct BondLocation bond);
	void detach(const glm::mat4 * matr);
	void reattach(const glm::mat4 * matr_prev, const glm::mat4 * matr_new);
	void updateMatrices();
	void drawElements(Shader shader);

	float computeRay(const Ray * ray, int inst_cntinst_cnt = 1);

	int get_t_bond_max_cnt();
	int get_h_bond_max_cnt();
	int get_column_max_cnt();


private:
	int tilted_bond_cnt;
	int horizontal_bond_cnt;
	int column_cnt;

	int t_bond_max_cnt;
	int h_bond_max_cnt;
	int column_max_cnt;

	Model * tilted_bond;
	Model * horizontal_bond;
	Model * column;

	std::list <struct Attachment> tilted_bond_list;
	std::list <struct Attachment> horizontal_bond_list;
	std::list <struct Attachment> column_list;


	clKernelsContainer * cl_kernel_cont;

};

#endif

