#ifndef MOVEMENT_PROGRAM_H
#define MOVEMENT_PROGRAM_H

#define AXIS_CNT 15

enum AxisName{
	none = 0,
	pantograph = 1,
	tower = 2,
	carrige = 3,
	turn = 4,
	rotate = 5,
	CAR = 7,
	syncW = 10
};



// enum AxisAltName{
//     bpant = 1,
//     tower = 2,
//     bcar = 3,
//     wrist = 4,
//     brot = 5,
//     rail = 7,
//     cpant = 8,//пантограф колонны
//     ccar = 9//каретка колонны
// };

struct AxisDriver{
	bool activate;
	float cur_pos;
	float set_point;
	float Vel;
	float Acc;
	float Dec;
};

struct MovementStep{
	enum AxisName axis;
	float POS;
	float Next; // not negative value of relative position
	float Vel;
	float Acc;
	float Dec;
};


void fillUpSPProgramsArray();
extern struct MovementStep sp_programs[][8];


#endif

