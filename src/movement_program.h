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
void resetDrivers(int prg_id);
int driverSM(float time);

float getAxisValue(AxisName id);

