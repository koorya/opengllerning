#ifndef CONSTRUCTIONCONTAINER_H
#define CONSTRUCTIONCONTAINER_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include "model.h"

struct BondLocation{
	int section;
	int name;
};

struct Attachment{
	const glm::mat4 * matr;
	int id;
};

class ConstructionContainer{
public:
	ConstructionContainer();
	~ConstructionContainer();
	void attachBond(const glm::mat4 * matr, struct BondLocation bond);
	void detach(const glm::mat4 * matr);
	void updateMatrices();
	void drawElements(Shader shader);

private:
	int tilted_bond_cnt;
	int horizontal_bond_cnt;
	int column_cnt;

	Model * tilted_bond;
	Model * horizontal_bond;
	Model * column;

	std::list <struct Attachment> tilted_bond_list;
	std::list <struct Attachment> horizontal_bond_list;
	std::list <struct Attachment> column_list;

};

#endif

