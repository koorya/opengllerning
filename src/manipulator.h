#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "movement_program.h"
#include "ConstructionContainer.h"
#include <vector>
#include "cassete.h"

struct ManipulatorDrive{
	float value;
	std::pair<float, float> limits;
};

struct ManipulatorConfig{
	struct ManipulatorDrive rail;
	struct ManipulatorDrive tower;
	struct ManipulatorDrive bpant;
	struct ManipulatorDrive cpant;
	struct ManipulatorDrive bcar;
	struct ManipulatorDrive ccar;
	struct ManipulatorDrive wrist;
	struct ManipulatorDrive brot;
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
	glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2; ///< матрицы положений элементов манипулятора
	struct ManipulatorConfig config; ///< актуальная конфигурация манипулятора, в абсолютных единицах координатной системы рамы.
	ConstructionContainer * container;///< ссылка на контейнер связей. Нужна для отображения захваченной связи и ее отображения после установки
	Cassete * cassete;

	Manipulator();
//	~Manipulator();
	void calculateMatrices();
	virtual void doStep() = 0;
	void pickUpBond();
	void mountBond();

	bool checkConfig();
};

class BotManipulator : public Manipulator{
public:

	struct AxisDriver axes_array[AXIS_CNT];
	struct ManipulatorConfig add_config; ///< смещение на секции, например
	std::vector <struct BondLocation> bond_list;///< список позиций связей, которые манипулятор должен установить
	
	BotManipulator(double (*time_funct)(void));
	void resetDrivers();
	void setProgram(MovementStep * prg);
	void moveToSection(int section);
	void mountBond(int b_id);
	int driverSM(float time);
	int sequenceSM(float time);
	void updateManipConfig();
	void resetConfiguration();
	void doStep();
private:
	double (*time_funct)(void);
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





#endif

