
#include "movement_program.h"

#include <cmath>
#include <iostream>




struct MovementStep sp_programs[100][8];//17 программ по 8 шагов каждая, неиспользуемые шаги в конце с осью none

void fillUpSPProgramsArray(){
	float raw_array[] = {
							1,	2,	-180,	200,	200,	200,	165,	1,	5,	-90,	500,	200,	200,	50,	1,	3,	1509,	2000,	2000,	2000,	1400,	1,	1,	170,	2000,	2000,	2000,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	93,	200,	200,	200,	65,	1,	1,	2866.26,	2000,	2000,	2000,	2700,	1,	4,	87,	500,	200,	200,	80,	1,	5,	-90,	500,	200,	200,	70,	1,	3,	1509,	2000,	2000,	2000,	1000,	1,	7,	617,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-93,	200,	200,	200,	65,	1,	1,	2866.26,	2000,	2000,	2000,	2700,	1,	4,	-87,	500,	200,	200,	80,	1,	5,	-90,	500,	200,	200,	70,	1,	3,	1509,	2000,	2000,	2000,	1000,	1,	7,	617,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	87,	200,	200,	200,	65,	1,	1,	2866.26,	2000,	2000,	2000,	2700,	1,	4,	-87,	500,	200,	200,	80,	1,	5,	-90,	500,	200,	200,	70,	1,	3,	1509,	2000,	2000,	2000,	1000,	1,	7,	-617,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-87,	200,	200,	200,	65,	1,	1,	2866.26,	2000,	2000,	2000,	2700,	1,	4,	87,	500,	200,	200,	80,	1,	5,	-90,	500,	200,	200,	70,	1,	3,	1509,	2000,	2000,	2000,	1000,	1,	7,	-617,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	101.59,	200,	200,	200,	65,	1,	1,	2289.34,	2000,	2000,	2000,	2000,	1,	5,	-153,	500,	200,	200,	100,	1,	4,	78.41,	500,	200,	200,	50,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	7,	220,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-105.77,	200,	200,	200,	65,	1,	1,	2341.55,	2000,	2000,	2000,	2000,	1,	5,	-27,	500,	200,	200,	20,	1,	4,	-74.23,	500,	200,	200,	50,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-196.67,	200,	200,	200,	180,	1,	5,	-27,	500,	200,	200,	25,	1,	4,	16.67,	500,	200,	200,	15,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	7,	1300,	500,	200,	200,	700,	1,	1,	-1562.52,	2000,	2000,	2000,	10,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-142,	200,	200,	200,	100,	1,	5,	-153,	500,	200,	200,	100,	1,	4,	-38,	500,	200,	200,	30,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	1,	387.25,	2000,	2000,	2000,	10,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	74.23,	200,	200,	200,	65,	1,	1,	2341.55,	2000,	2000,	2000,	2000,	1,	5,	-27,	500,	200,	200,	20,	1,	4,	-74.23,	500,	200,	200,	50,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-74.23,	200,	200,	200,	65,	1,	1,	2341.55,	2000,	2000,	2000,	2000,	1,	5,	-153,	500,	200,	200,	100,	1,	4,	74.23,	500,	200,	200,	50,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	90,	200,	200,	200,	65,	1,	5,	-90,	500,	200,	200,	50,	1,	3,	1509,	2000,	2000,	2000,	1400,	1,	1,	170,	2000,	2000,	2000,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-90,	200,	200,	200,	65,	1,	5,	-90,	500,	200,	200,	50,	1,	3,	1509,	2000,	2000,	2000,	1400,	1,	1,	170,	2000,	2000,	2000,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	99.22,	200,	200,	200,	70,	1,	5,	-153,	500,	200,	200,	100,	1,	4,	-9.22,	500,	200,	200,	14,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	1,	180.6,	2000,	2000,	2000,	10,	1,	7,	500,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-128,	200,	200,	200,	100,	1,	5,	-27,	500,	200,	200,	20,	1,	4,	38,	500,	200,	200,	30,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	1,	387.25,	2000,	2000,	2000,	10,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	52,	200,	200,	200,	40,	1,	5,	-27,	500,	200,	200,	20,	1,	4,	38,	500,	200,	200,	30,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	1,	387.25,	2000,	2000,	2000,	10,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	2,	-52,	200,	200,	200,	50,	1,	5,	-153,	500,	200,	200,	100,	1,	4,	-38,	500,	200,	200,	30,	1,	3,	58.17,	2000,	2000,	2000,	100,	1,	1,	387.25,	2000,	2000,	2000,	10,	1,	7,	0,	500,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,

							0,	10,	-100,	200,	500,	500,	0,	1,	1,	0,	2000,	2000,	2000,	50,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	20,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	10,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	30,	1,	4,	0,	500,	200,	200,	10,	1,	1,	0,	2000,	2000,	2000,	200,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	10,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	30,	1,	4,	0,	500,	200,	200,	10,	1,	1,	0,	2000,	2000,	2000,	200,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	10,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	30,	1,	4,	0,	500,	200,	200,	10,	1,	1,	0,	2000,	2000,	2000,	200,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	10,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	30,	1,	4,	0,	500,	200,	200,	10,	1,	1,	0,	2000,	2000,	2000,	200,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	50,	1,	3,	299,	2000,	2000,	2000,	100,	1,	4,	0,	500,	200,	200,	50,	1,	5,	0,	500,	200,	200,	100,	1,	1,	0,	2000,	2000,	2000,	100,	1,	2,	0,	200,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	50,	1,	3,	299,	2000,	2000,	2000,	100,	1,	4,	0,	500,	200,	200,	50,	1,	5,	0,	500,	200,	200,	100,	1,	1,	0,	2000,	2000,	2000,	100,	1,	2,	0,	200,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,
							0,	10,	-1,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	7,	0,	500,	200,	200,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	7,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	10,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	1,	7,	0,	500,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	50,	1,	3,	299,	2000,	2000,	2000,	100,	1,	4,	0,	500,	200,	200,	50,	1,	5,	0,	500,	200,	200,	100,	1,	1,	0,	2000,	2000,	2000,	100,	1,	2,	0,	200,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	500,	500,	0,	1,	7,	0,	500,	200,	200,	50,	1,	3,	299,	2000,	2000,	2000,	100,	1,	4,	0,	500,	200,	200,	50,	1,	5,	0,	500,	200,	200,	100,	1,	1,	0,	2000,	2000,	2000,	100,	1,	2,	0,	200,	200,	200,	0.0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-100,	200,	500,	500,	0,	1,	1,	0,	2000,	2000,	2000,	50,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	20,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-100,	200,	500,	500,	0,	1,	1,	0,	2000,	2000,	2000,	50,	1,	3,	299,	2000,	2000,	2000,	500,	1,	5,	0,	500,	200,	200,	20,	1,	2,	0,	200,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	10,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	1,	7,	0,	500,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	10,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	1,	7,	0,	500,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	10,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	1,	7,	0,	500,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0,
							1,	10,	-300,	200,	1000,	1000,	0,	1,	1,	0,	2000,	2000,	2000,	10,	1,	3,	299,	2000,	2000,	2000,	10,	1,	4,	0,	500,	200,	200,	10,	1,	5,	0,	500,	200,	200,	10,	1,	2,	0,	200,	200,	200,	50,	1,	7,	0,	500,	200,	200,	0,	0,	0,	0,	0,	0,	0,	0

						};
	int prg_cnt = sizeof(raw_array)/7/8/sizeof(float);
	for (int i = 0; i < prg_cnt; i++){
		for(int j = 0; j < 8; j++){
			sp_programs[i][j].axis = static_cast <enum AxisName> (std::round(raw_array[i*8*7 + j*7 + 1]));
			sp_programs[i][j].POS = raw_array[i*8*7 + j*7 + 2];
			sp_programs[i][j].Vel = raw_array[i*8*7 + j*7 + 3];
			sp_programs[i][j].Acc = raw_array[i*8*7 + j*7 + 4];
			sp_programs[i][j].Dec = raw_array[i*8*7 + j*7 + 5];
			sp_programs[i][j].Next = raw_array[i*8*7 + j*7 + 6];
		}
	}
}





