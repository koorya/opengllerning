#include "manipulator.h"
#include <iostream>

#define EPSILON 0.01

Manipulator m_mat[3] = {Manipulator(), Manipulator(), Manipulator()};
struct FrameMatrices f_mat;


Manipulator::Manipulator(){
	config.rail = 10000.0;
	config.tower = 30.0;
	config.bpant = 3000.0;
	config.cpant = 500.0;
	config.bcar = 1000.0;
	config.ccar = 500.0;
	config.wrist = 20.0;
	config.brot = 60.0;

	struct AxisDriver * axes 			= 	config.axes_array;
	axes[AxisName::carrige].cur_pos 	= 	config.bcar;
	axes[AxisName::pantograph].cur_pos 	= 	config.bpant;
	axes[AxisName::rotate].cur_pos 		=	config.brot;
	axes[AxisName::CAR].cur_pos 		= 	config.rail;
	axes[AxisName::tower].cur_pos 		= 	config.tower;
	axes[AxisName::turn].cur_pos 		= 	config.wrist;

}




void Manipulator::setProgram(int prg_id){
	cur_program = prg_id;
	this->resetDrivers();
}

void Manipulator::resetDrivers(){
	cur_step = 0;
	for(int i = 0; i < AXIS_CNT; i++){
		config.axes_array[i].activate = false;
		config.axes_array[i].Acc = 0.0;
		config.axes_array[i].cur_pos = 0.0;
		config.axes_array[i].set_point = 0.0;
		config.axes_array[i].Vel = 0.0;
		config.axes_array[i].Acc = 0.0;
		config.axes_array[i].Dec = 0.0;
	}
}


int Manipulator::driverSM(float time){
	// if(cur_step == 1)
	// 	return 0;
	float dt = time - last_time;

	last_time = time;
	int last_step = 0;
	int all_axis_complete = 1;

//	std::cout<<"dt "<<dt<<", step: "<<cur_step<<std::endl;

	struct MovementStep cstep = sp_programs[cur_program][cur_step];

	if(config.axes_array[cstep.axis].activate == false){//перешли на следующий шаг
		if(cstep.axis == AxisName::none)
			last_step = 1;
		config.axes_array[cstep.axis].activate = true;
		config.axes_array[cstep.axis].set_point = cstep.POS;
		config.axes_array[cstep.axis].Vel = cstep.Vel * ((cstep.POS>0)?1:-1);
		config.axes_array[cstep.axis].Acc = cstep.Acc;
		config.axes_array[cstep.axis].Dec = cstep.Dec;
		if(cur_step == 0){//означает, что это первый цикл
			dt = 0.0;
		}
	}
	if(std::fabs(config.axes_array[cstep.axis].cur_pos) >= cstep.Next){
		if(cstep.Next > 0.0){//это означает, чно порог включения следующего шага достигнут
			cur_step ++;
		}else if(std::fabs(config.axes_array[cstep.axis].cur_pos - config.axes_array[cstep.axis].set_point) < EPSILON){//это означает, чно нужно дождаться окончания этого шага
			cur_step ++;
		}
		if(cur_step >= 8){
			cur_step = 7;
			last_step = 1;
		}
	}

	for(int i = 0; i < AXIS_CNT; i++){
		if(config.axes_array[i].activate == false)
			continue;
		float remaining_dist = config.axes_array[i].cur_pos - config.axes_array[i].set_point;
		float dx = config.axes_array[i].Vel*dt;

//		std::cout<<"i: "<<i<<", remaining_dist: "<<remaining_dist<<", config.axes_array[i].cur_pos: "<<config.axes_array[i].cur_pos<<", dx: "<<dx<<std::endl;
		if( std::fabs(remaining_dist + dx) <= std::fabs(remaining_dist) ){ // если следующий шаг нас приближает к конечной точке, то добавляем шаг
			config.axes_array[i].cur_pos += dx;
			all_axis_complete = 0;
		}else{	
				// если следующий шаг нас удаляет от конечной точки (т.е. пропустили изза большого шага дискретизации), 
				// то выключаем скорость и устанавливаем позицию в конечную точку
			config.axes_array[i].Vel = 0.0;
			config.axes_array[i].cur_pos = config.axes_array[i].set_point;
		}
	}
	// if(last_step & all_axis_complete)
	// 	exit(0);
	return last_step & all_axis_complete;
}


void Manipulator::updateManipConfig(){
	struct AxisDriver * axes = config.axes_array;

	config.bcar = axes[AxisName::carrige].cur_pos;
	config.bpant = axes[AxisName::pantograph].cur_pos;
	config.brot = axes[AxisName::rotate].cur_pos;
	config.rail = axes[AxisName::CAR].cur_pos;
	config.tower = axes[AxisName::tower].cur_pos;
	config.wrist = axes[AxisName::turn].cur_pos;

}

void calculateManipulatorGraphicMatrices(){



	float frame_level = 1*3000.0 ;//+ 500;
	float max_bcar_level = 1500.0;
	float max_ccar_level = 350.0;

	f_mat.World = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
	f_mat.World = glm::rotate(f_mat.World, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	f_mat.A  = glm::translate(f_mat.World, glm::vec3(0.0, 0.0, frame_level));

	//rail position on frame
	m_mat[0].B = glm::translate(f_mat.A, glm::vec3(2100.0, 6760.0, -400.0));
	m_mat[1].B = glm::translate(f_mat.A, glm::vec3(200.0, -245.0, -400.0));
	m_mat[2].B = glm::translate(f_mat.A, glm::vec3(2100.0, -7250.0, -400.0));

	for(int i = 0; i < 3; i++){
		m_mat[i].C = glm::translate(m_mat[i].B, glm::vec3(m_mat[i].config.rail, 0.0, -400.0));
		m_mat[i].D = glm::rotate(m_mat[i].C, glm::radians(m_mat[i].config.tower), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].E1 = glm::translate(glm::rotate(m_mat[i].D, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
											glm::vec3(m_mat[i].config.bpant, 0.0f, 0.0f));
		m_mat[i].E2 = glm::translate(m_mat[i].D, glm::vec3(m_mat[i].config.cpant, 0.0f, 0.0f));
		m_mat[i].F1 = glm::translate(m_mat[i].E1, glm::vec3(627.0, 0.0, -max_bcar_level + m_mat[i].config.bcar));
		m_mat[i].F2 = glm::translate(m_mat[i].E2, glm::vec3(0.0, 0.0, -max_ccar_level + m_mat[i].config.ccar));
		m_mat[i].G1 = glm::rotate(m_mat[i].F1, glm::radians(m_mat[i].config.wrist), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].G2 = glm::rotate(glm::translate(m_mat[i].F2, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].H = glm::rotate(m_mat[i].G1, glm::radians(m_mat[i].config.brot), glm::vec3(1.0, 0.0, 0.0));
		m_mat[i].I = glm::rotate(glm::translate(m_mat[i].H, glm::vec3(653.0, 0.0, -1210.0)), glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	}


	for(int i=0; i<3; i++){
		float pant_length = m_mat[i].config.bpant+334.2;
		float angle = -asinf((pant_length)/4450);
		glm::mat4 pant_origin = glm::rotate(m_mat[i].D, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));

		pant_origin = glm::translate(pant_origin, glm::vec3(-49.0, 0.0, -224.0));
		m_mat[i].pb1 = glm::rotate(pant_origin, angle, glm::vec3(0.0, 1.0, 0.0));
		m_mat[i].pb2 = glm::rotate(glm::translate(pant_origin, glm::vec3(pant_length/2.0, 0.0f, 0.0f)),
											angle, glm::vec3(0.0, 1.0, 0.0));

		m_mat[i].pb3 = glm::rotate(glm::translate(pant_origin, glm::vec3(pant_length/2.0, 0.0f, 0.0f)),
											-angle, glm::vec3(0.0, 1.0, 0.0));
		m_mat[i].pb4 = glm::rotate(glm::translate(pant_origin, glm::vec3(pant_length, 0.0f, 0.0f)),
											-angle, glm::vec3(0.0, 1.0, 0.0));
	}


	for(int i=0; i<3; i++){
		float pant_length = m_mat[i].config.cpant + 148.0;
		float angle = -asinf((pant_length)/1420);
		glm::mat4 pant_origin = m_mat[i].D;

		pant_origin = glm::translate(pant_origin, glm::vec3(394.0, 0.0, -511.0));
		m_mat[i].pc1 = glm::rotate(pant_origin, angle, glm::vec3(0.0, 1.0, 0.0));
		m_mat[i].pc2 = glm::rotate(glm::translate(pant_origin, glm::vec3(pant_length, 0.0f, 0.0f)),
											-angle, glm::vec3(0.0, 1.0, 0.0));
	}

}
