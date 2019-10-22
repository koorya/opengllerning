#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "movement_program.h"
#include "ConstructionContainer.h"
#include <vector>

struct ManipulatorConfig{
	float rail;
	float tower;
	float bpant;
	float cpant;
	float bcar;
	float ccar;
	float wrist;
	float brot;
	struct AxisDriver axes_array[AXIS_CNT];
};

struct FrameMatrices{
	glm::mat4 World, A;
};

enum ManStates{
	vacant = 0,
	pickup,
	move_to_section,
	move_to_place,
	mount,
	move_from_place,
	move_to_origin
};



class Manipulator{
public:
	glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2;
	struct ManipulatorConfig config;
	struct ManipulatorConfig add_config;
	std::vector <struct BondLocation> bond_list;
	ConstructionContainer * container;

	Manipulator();
	void resetDrivers();
	void setProgram(MovementStep * prg);
	void moveToSection(int section);
	void mountBond(int b_id);
	int driverSM(float time);
	int sequenceSM(float time);
	void updateManipConfig();
	void resetConfiguration();
private:
	struct MovementStep * cur_program;
	struct MovementStep sect_move_prg[8];
	int cur_section;
	int cur_step;
	float last_time;
	enum ManStates state;
};

// struct ManipulatorGraphMatrices{
//     glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2;
// };

extern Manipulator m_mat[3];
extern struct FrameMatrices f_mat;


void calculateManipulatorGraphicMatrices();



#endif

