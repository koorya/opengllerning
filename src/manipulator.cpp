#include "manipulator.h"
#include <iostream>

#define EPSILON 0.01

Manipulator m_mat[3] = {Manipulator(), Manipulator(), Manipulator()};
struct FrameMatrices f_mat;
float section_length = 3500.0;

Manipulator::Manipulator(){

	state = vacant;

	for(int i = 0; i < 8; i++){
		sect_move_prg[i].axis = AxisName::none;
		sect_move_prg[i].POS = 0.0;
		sect_move_prg[i].Vel = 0.0;
		sect_move_prg[i].Acc = 0.0;
		sect_move_prg[i].Dec = 0.0;
		sect_move_prg[i].Next = 0.0;
	}

	// add_config.rail = 10000.0;
	// add_config.tower = 30.0;
	// add_config.bpant = 3000.0;
	// add_config.cpant = 500.0;
	// add_config.bcar = 1000.0;
	// add_config.ccar = 500.0;
	// add_config.wrist = 20.0;
	// add_config.brot = 60.0;

	add_config.rail = 0.0;
	add_config.tower = 0.0;
	add_config.bpant = 0.0;
	add_config.cpant = 0.0;
	add_config.bcar = 0.0;
	add_config.ccar = 0.0;
	add_config.wrist = 0.0;
	add_config.brot = 0.0;


	struct AxisDriver * axes 			= 	config.axes_array;
	axes[AxisName::carrige].cur_pos 	= 	0.0;
	axes[AxisName::pantograph].cur_pos 	= 	0.0;
	axes[AxisName::rotate].cur_pos 		=	0.0;
	axes[AxisName::CAR].cur_pos 		= 	0.0;
	axes[AxisName::tower].cur_pos 		= 	0.0;
	axes[AxisName::turn].cur_pos 		= 	0.0;

	config.rail =  add_config.rail + axes[AxisName::CAR].cur_pos; 
	config.tower = add_config.tower + axes[AxisName::tower].cur_pos;
	config.bpant = add_config.bpant + axes[AxisName::pantograph].cur_pos;
	config.cpant = add_config.cpant;
	config.bcar =  add_config.bcar + axes[AxisName::carrige].cur_pos; 
	config.ccar =  add_config.ccar; 
	config.wrist = add_config.wrist + axes[AxisName::turn].cur_pos;
	config.brot =  add_config.brot + axes[AxisName::rotate].cur_pos; 
}

int Manipulator::sequenceSM(float time){
	if(state == ManStates::vacant){
		if(bond_list.empty())
			return 1;
		state = pickup;
	}else if(state == ManStates::pickup){
		container->attachBond(&(this->I), bond_list.back());
		state = move_to_section;
		this->moveToSection(bond_list.back().section);
	}else if(state == ManStates::move_to_section){
		if(driverSM(time)){
			state = move_to_place;
			this->mountBond(bond_list.back().name);
		}
	}else if(state == ManStates::move_to_place){
		if(driverSM(time)){
			state = mount;
		}
	}else if(state == ManStates::mount){
		container->detach(&(this->I));
		state = move_from_place;
		this->mountBond(bond_list.back().name + 100);//обратно
		bond_list.pop_back(); // удалили из списка 
	}else if(state == ManStates::move_from_place){
		if(driverSM(time)){
			state = move_to_origin;
			this->moveToSection(0);
		}
	}else if(state == ManStates::move_to_origin){
		if(driverSM(time)){
			if(bond_list.empty()){
				state = vacant;
			}else{
				state = pickup;
			}
		}	
	}
}

MovementStep * seqToComand(int sq){
	int ret;
	bool fw = true;
	if(sq>100){ //rew
		fw = false;
		sq %= 100; //с префиксом 100 обозначаются обратные движения
	}
	if(sq == 1)
		ret = 1;
	else if (sq == 2)
		ret = 9;
	else if (sq == 3)
		ret = 8;
	else if (sq == 4)
		ret = 7;
	else if (sq == 5)
		ret = 6;
	else if (sq == 6)
		ret = 15;
	else if (sq == 7)
		ret = 17;
	else if (sq == 8)
		ret = 14;
	else if (sq == 9)
		ret = 16;
	else if (sq == 10)
		ret = 13;
	else if (sq == 11)
		ret = 12;
	else if (sq == 12)
		ret = 3;
	else if (sq == 13)
		ret = 2;
	else
		return 0;

	ret--; //нумерация в массиве команд начинается с нуля

	if(!fw){
		ret += 17; //во второй половине массива у нас находятся обратные движения
	}
	
	return sp_programs[ret];
}

void Manipulator::moveToSection(int section){
	cur_section = section;
	sect_move_prg[0].axis = AxisName::CAR;
	sect_move_prg[0].Vel = 2000.0;
	setProgram(sect_move_prg);
}

void Manipulator::mountBond(int b_id){
	MovementStep * prg = seqToComand(b_id);
	setProgram(prg);
}

void Manipulator::setProgram(MovementStep * prg){
	cur_program = prg;
	if(cur_program == 0){
		std::cout<<"ERROR::INVALID BOND POSITION ID"<<std::endl;
		return;
	}

	this->resetDrivers();

	// add_config.rail =  config.rail;
	// add_config.tower = config.tower;
	// add_config.bpant = config.bpant;
	// add_config.cpant = config.cpant;
	// add_config.bcar =  config.bcar;
	// add_config.ccar =  config.ccar;
	// add_config.wrist = config.wrist;
	// add_config.brot =  config.brot;

	struct AxisDriver * axes 	= 	config.axes_array;

	axes[AxisName::CAR].cur_pos =  config.rail - cur_section*section_length;
	axes[AxisName::tower].cur_pos = config.tower;
	axes[AxisName::pantograph].cur_pos = config.bpant;

	axes[AxisName::carrige].cur_pos =  config.bcar;

	axes[AxisName::turn].cur_pos = config.wrist;
	axes[AxisName::rotate].cur_pos =  config.brot ;
}

void Manipulator::resetConfiguration(){
	config.rail = 0.0;
	config.tower = 0.0;
	config.bpant = 0.0;
	config.cpant = 0.0;
	config.bcar = 0.0;
	config.ccar = 0.0;
	config.wrist = 0.0;
	config.brot = 0.0;

	add_config.rail = 0.0;
	add_config.tower = 0.0;
	add_config.bpant = 0.0;
	add_config.cpant = 0.0;
	add_config.bcar = 0.0;
	add_config.ccar = 0.0;
	add_config.wrist = 0.0;
	add_config.brot = 0.0;
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

char * axis_names[] = {"none", "pantograph", "tower", "carrige", "turn", "rotate", "__6__", "CAR", "__8__", "__9__", "syncW"};

int Manipulator::driverSM(float time){
	if(cur_program == 0){
		std::cout<<"ERROR::driverSM PROGRAM IS NOT SET"<<std::endl;
		return -1;
	}
	// if(cur_step == 1)
	// 	return 0;
	float dt = time - last_time;

	last_time = time;
	int last_step = 0;
	int all_axis_complete = 1;


	struct MovementStep cstep = cur_program[cur_step];
	std::cout<<std::endl<<"dt "<<dt<<", step: "<<cur_step<<", axis: "<<axis_names[cstep.axis]<<" activate:"<<config.axes_array[cstep.axis].activate<<std::endl;

	if(config.axes_array[cstep.axis].activate == false){//перешли на следующий шаг

		config.axes_array[cstep.axis].activate = true;
		config.axes_array[cstep.axis].set_point = cstep.POS;
		config.axes_array[cstep.axis].Vel = cstep.Vel * (((cstep.POS - config.axes_array[cstep.axis].cur_pos)>0)?1:-1);
		std::cout<<"!!!!!!!!!!cstep.POS - config.axes_array[cstep.axis].cur_pos!!! "<<cstep.POS<<" - "<<config.axes_array[cstep.axis].cur_pos<<std::endl;
		config.axes_array[cstep.axis].Acc = cstep.Acc;
		config.axes_array[cstep.axis].Dec = cstep.Dec;
		if(cur_step == 0){//означает, что это первый цикл
			dt = 0.0;
		}
		if(cstep.axis == AxisName::none){
			last_step = 1;
			config.axes_array[cstep.axis].activate = false;
		}
	}


	if(cstep.Next == 0.0){//это означает, чно порог включения следующего не установлен. Ожидаем завержения
		if(std::fabs(config.axes_array[cstep.axis].cur_pos - config.axes_array[cstep.axis].set_point) < EPSILON){//это означает, чно нужно дождаться окончания этого шага
			cur_step ++;
		}
	}else{
		if(config.axes_array[cstep.axis].Vel > 0){
			if(config.axes_array[cstep.axis].cur_pos > cstep.Next)
				cur_step ++;
		}else if(config.axes_array[cstep.axis].Vel < 0){
			if(config.axes_array[cstep.axis].cur_pos < cstep.Next)
				cur_step ++;
		}else{
			cur_step ++;
		}
	}
	
	if(cur_step >= 8){
		cur_step = 7;
		last_step = 1;
	}
	

	for(int i = 0; i < AXIS_CNT; i++){
		if(config.axes_array[i].activate == false)
			continue;
		float remaining_dist = config.axes_array[i].cur_pos - config.axes_array[i].set_point;
		float dx = config.axes_array[i].Vel*dt;

		// std::cout<<"i: "<<i<<", remaining_dist: "<<remaining_dist<<", config.axes_array[i].cur_pos: "<<config.axes_array[i].cur_pos<<", dx: "<<dx<<std::endl;
		if( (std::fabs(remaining_dist + dx) <= std::fabs(remaining_dist) ) && (config.axes_array[i].Vel != 0.0f)){ // если следующий шаг нас приближает к конечной точке, то добавляем шаг
			config.axes_array[i].cur_pos += dx;
			all_axis_complete = 0;
			std::cout<<"i: "<<i<<", remaining_dist: "<<remaining_dist<<", config.axes_array[i].cur_pos: "<<config.axes_array[i].cur_pos<<", dx: "<<dx<<std::endl;
		}else{	
				// если следующий шаг нас удаляет от конечной точки (т.е. пропустили изза большого шага дискретизации), 
				// то выключаем скорость и устанавливаем позицию в конечную точку
			config.axes_array[i].Vel = 0.0;
			config.axes_array[i].cur_pos = config.axes_array[i].set_point;
		}
	}
	// if(last_step & all_axis_complete)
	// 	exit(0);
	std::cout<<"last_step "<<last_step<<", all_axis_complete "<<all_axis_complete<<std::endl;
	return last_step & all_axis_complete;
}


void Manipulator::updateManipConfig(){
	struct AxisDriver * axes = config.axes_array;

	config.rail =  add_config.rail + axes[AxisName::CAR].cur_pos + cur_section*section_length; 
	config.tower = add_config.tower + axes[AxisName::tower].cur_pos;
	config.bpant = add_config.bpant + axes[AxisName::pantograph].cur_pos;
	config.cpant = add_config.cpant;
	config.bcar =  add_config.bcar + axes[AxisName::carrige].cur_pos; 
	config.ccar =  add_config.ccar; 
	config.wrist = add_config.wrist + axes[AxisName::turn].cur_pos;
	config.brot =  add_config.brot + axes[AxisName::rotate].cur_pos; 


}

void calculateManipulatorGraphicMatrices(){



	float frame_level = 0;//1*3000.0 ;//+ 500;
	float max_bcar_level = 1443.0;
	float max_ccar_level = 0.0;

	f_mat.World = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
	f_mat.World = glm::rotate(f_mat.World, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	f_mat.A  = glm::translate(f_mat.World, glm::vec3(0.0, 0.0, frame_level));

	//rail position on frame
	m_mat[0].B = glm::translate(f_mat.A, glm::vec3(350.0, 6760.0, -400.0));
	m_mat[1].B = glm::translate(f_mat.A, glm::vec3(350.0, -245.0, -400.0));
	m_mat[2].B = glm::translate(f_mat.A, glm::vec3(350.0, -7250.0, -400.0));

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
		m_mat[i].H = glm::rotate(m_mat[i].G1, glm::radians(180.0f-m_mat[i].config.brot), glm::vec3(1.0, 0.0, 0.0));
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
