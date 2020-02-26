/** \file */

#include "manipulator.h"
#include <iostream>

#define EPSILON 0.01


float section_length = 3500.0;

Manipulator::Manipulator(){
	config.rail.value = 0.0;
	config.tower.value = 0.0;
	config.bpant.value = 0.0;
	config.cpant.value = 0.0;
	config.bcar.value = 0.0;
	config.ccar.value = 0.0;
	config.wrist.value = 0.0;
	config.brot.value = 0.0;


	config.rail.limits = std::pair<float, float> (0.0, 1000 + 0.86 * 3000 * 6 );
	config.tower.limits = std::pair<float, float> (-110.0, 210.0);
	config.bpant.limits = std::pair<float, float> (0.0, 2920.0);
	config.bcar.limits = std::pair<float, float> (0.0, 1645.0);
	config.wrist.limits = std::pair<float, float> (-95.0, 95.0);
	config.brot.limits = std::pair<float, float> (-180.0, 20.0);
	config.cpant.limits = std::pair<float, float> (0.0, 680.0);
	config.ccar.limits = std::pair<float, float> (0.0, 765.0);



}

bool inLimits(const float val, const std::pair<float, float> &lim){
	return ( lim.first <= val ) && ( lim.second >= val);
}

bool Manipulator::checkConfig(){
	return 	inLimits(config.rail.value, config.rail.limits) &&
			inLimits(config.tower.value, config.tower.limits) &&
			inLimits(config.bpant.value, config.bpant.limits) &&
			inLimits(config.bcar.value, config.bcar.limits) &&
			inLimits(config.wrist.value, config.wrist.limits) &&
			inLimits(config.brot.value, config.brot.limits) &&
			inLimits(config.cpant.value, config.cpant.limits) &&
			inLimits(config.ccar.value, config.ccar.limits);
}


BotManipulator::BotManipulator(double (*time_funct)(void)) : Manipulator(){

	this->time_funct = time_funct;

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

	add_config.rail.value = 0.0;
	add_config.tower.value = 0.0;
	add_config.bpant.value = 0.0;
	add_config.cpant.value = 0.0;
	add_config.bcar.value = 0.0;
	add_config.ccar.value = 0.0;
	add_config.wrist.value = 0.0;
	add_config.brot.value = 0.0;


	struct AxisDriver * axes 			= 	axes_array;
	axes[AxisName::carrige].cur_pos 	= 	0.0;
	axes[AxisName::pantograph].cur_pos 	= 	0.0;
	axes[AxisName::rotate].cur_pos 		=	0.0;
	axes[AxisName::CAR].cur_pos 		= 	0.0;
	axes[AxisName::tower].cur_pos 		= 	0.0;
	axes[AxisName::turn].cur_pos 		= 	0.0;

	config.rail.value =  add_config.rail.value + axes[AxisName::CAR].cur_pos; 
	config.tower.value = add_config.tower.value + axes[AxisName::tower].cur_pos;
	config.bpant.value = add_config.bpant.value + axes[AxisName::pantograph].cur_pos;
	config.cpant.value = add_config.cpant.value;
	config.bcar.value =  add_config.bcar.value + axes[AxisName::carrige].cur_pos; 
	config.ccar.value =  add_config.ccar.value; 
	config.wrist.value = add_config.wrist.value + axes[AxisName::turn].cur_pos;
	config.brot.value =  add_config.brot.value + axes[AxisName::rotate].cur_pos; 
}

int BotManipulator::sequenceSM(float time){
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
		return 1;
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
	return 0;
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


/**
 * \brief задает программу из одного шага для перемещения в заданную секцию
 */
void BotManipulator::moveToSection(int section){
	setProgram(sect_move_prg);

	cur_section = section;
	sect_move_prg[0].axis = AxisName::CAR;
	sect_move_prg[0].Vel = 200.0;
	sect_move_prg[0].Acc = 20.0;
	sect_move_prg[0].Dec = 20.0;
	sect_move_prg[0].POS = cur_section*section_length;

}
/**
 * \brief установка соответсвующей программы установки связи по ее номеру
 * 
 * Этой функции передается номер связи в секции для текущего манипулятора (не этажа),
 * она выбирает соответствующую программу движений и задает ее как текущую выполняемую. 
 * \callgraph
 * \callergraph
 */
void BotManipulator::mountBond(int b_id){
	MovementStep * prg = seqToComand(b_id);
	setProgram(prg);
}


/**
 * \brief подготовка конфигурации к выполнению программы
 * 
 * Подготавливает позиции осей под выполняемую программу. А именно, виртуально смещает вдоль рельсов так, чтобы для программы
 * все выглядело так, будто он находится в нулевой секции. Так же, устанавливает текущую программу в соответствии со входным параметром.
 * Сбрасывает драйверы приводов.
 * \callergraph
 */
void BotManipulator::setProgram(MovementStep * prg){
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

	struct AxisDriver * axes 	= 	axes_array;

	axes[AxisName::CAR].cur_pos =  config.rail.value - cur_section*section_length;
	axes[AxisName::tower].cur_pos = config.tower.value;
	axes[AxisName::pantograph].cur_pos = config.bpant.value;

	axes[AxisName::carrige].cur_pos =  config.bcar.value;

	axes[AxisName::turn].cur_pos = config.wrist.value;
	axes[AxisName::rotate].cur_pos =  config.brot.value ;
}

void BotManipulator::resetConfiguration(){
	config.rail.value = 0.0;
	config.tower.value = 0.0;
	config.bpant.value = 0.0;
	config.cpant.value = 0.0;
	config.bcar.value = 0.0;
	config.ccar.value = 0.0;
	config.wrist.value = 0.0;
	config.brot.value = 0.0;

	add_config.rail.value = 0.0;
	add_config.tower.value = 0.0;
	add_config.bpant.value = 0.0;
	add_config.cpant.value = 0.0;
	add_config.bcar.value = 0.0;
	add_config.ccar.value = 0.0;
	add_config.wrist.value = 0.0;
	add_config.brot.value = 0.0;
}
/**
 * \brief сбрасывает параметры осей для записи в них программы
 * 
 */
void BotManipulator::resetDrivers(){
	cur_step = 0;
	for(int i = 0; i < AXIS_CNT; i++){
		axes_array[i].activate = false;
		axes_array[i].Acc = 0.0;
		axes_array[i].cur_pos = 0.0;
		axes_array[i].set_point = 0.0;
		axes_array[i].Vel = 0.0;
		axes_array[i].Acc = 0.0;
		axes_array[i].Dec = 0.0;
	}


}

/**
 * \brief выдает позицию исходя из параметров движения и времени.
 * 
 * Используется для получения позиции привода во время с начала выполнения шага этого привода.
 * \param x относительное расстояние
 * \param a1,a2  ускорение и замедление
 * \param v максимальная скорость
 * \param t количество секунд с начала движения
 * 
 * \returns 0 <= pos(t) <= x
 **/

float calcPos(float a1, float a2, float v, float x, float t){
	float res = 0;
	if(v < 0){
		a1 = -a1;
		a2 = -a2;
	}
	if(x > v*v*(1.0f/a1+ 1.0f/a2)){//параметры некорректные, за такое расстояние он не успеет разогнаться до скорости, считаем по другому алгоритму
		std::cout<<"calcPos: param is not correct"<<std::endl;
		float t1 = sqrt(2*x / ( a1 * (1 + a1/a2) ));
		float t2 = t1 * ( a1 / a2 + 1 );
		if (t < t1){
			res = a1 * t * t / 2.0;
		}else if(t < t2){
			res = x - a2 * (t2 - t) * (t2 - t)/2;
		}else{
			res = x;
		}
	}else{
		float t1 = v/a1;
		float t3 = 0.5*(v/a2 + t1) + x/v;
		float t2 = t3 - v/a2;
		if(t <= t1){
			res = a1 * t * t / 2.0;
		}else if(t <= t2){
			res = v * ( t - t1 / 2.0 );
		}else if(t <= t3){
			res = v * ( t + t2 - t1 ) / 2.0;
		}else{
			res = x;
		}
 	}
	return res;
}

char * axis_names[] = {"none", "pantograph", "tower", "carrige", "turn", "rotate", "__6__", "CAR", "__8__", "__9__", "syncW"};
/*!
 * \brief Функция, ответственная за выполнения программ движений.
 * 
 * 
 * Функция берет текущую установленную программу движений и в соответсвии с текущим временем устанавливает позиции всех приводов.
 * Она должна учитывать ускорения, замедления, и запуск следующих шагов по условию на предыдущие шаги.
 */
int BotManipulator::driverSM(float time){
	if(cur_program == 0){
		std::cout<<"ERROR::driverSM PROGRAM IS NOT SET"<<std::endl;
		return -1;
	}

	int last_step = 0;
	int all_axis_complete = 1;


	struct MovementStep cstep = cur_program[cur_step];

	if(axes_array[cstep.axis].activate == false){//перешли на следующий шаг

		axes_array[cstep.axis].activate = true;
		axes_array[cstep.axis].set_point = cstep.POS;
		axes_array[cstep.axis].Vel = cstep.Vel * (((cstep.POS - axes_array[cstep.axis].cur_pos)>0)?1:-1);
		std::cout<<"!!!!!!!!!!cstep.POS - axes_array[cstep.axis].cur_pos!!! "<<cstep.POS<<" - "<<axes_array[cstep.axis].cur_pos<<std::endl;
		axes_array[cstep.axis].Acc = cstep.Acc;
		axes_array[cstep.axis].Dec = cstep.Dec;
		axes_array[cstep.axis].start_time = time; //запоминаем время для расчета движений

		if(cstep.axis == AxisName::none){ //означает, что программа закончилась
			last_step = 1;
			axes_array[cstep.axis].activate = false;
		}
	}


	if(cstep.Next == 0.0){//это означает, чно порог включения следующего не установлен. Ожидаем завержения
		if(std::fabs(axes_array[cstep.axis].cur_pos - axes_array[cstep.axis].set_point) < EPSILON){//это означает, чно нужно дождаться окончания этого шага
			cur_step ++;
		}
	}else{
		if(axes_array[cstep.axis].Vel > 0){
			if(axes_array[cstep.axis].cur_pos > cstep.Next)
				cur_step ++;
		}else if(axes_array[cstep.axis].Vel < 0){
			if(axes_array[cstep.axis].cur_pos < cstep.Next)
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
		if(axes_array[i].activate == false)
			continue;
		AxisDriver * ca = &(axes_array[i]);
		float pos = calcPos(ca->Acc, ca->Dec, ca->Vel, ca->set_point, time - ca->start_time);
		std::cout<<"axis: "<<axis_names[i]<<", remaining_dist: "<<axes_array[i].set_point - axes_array[i].cur_pos
					<<", axes_array[i].cur_pos: "<<axes_array[i].cur_pos
					<<", dx: "<<axes_array[i].cur_pos - pos
					<<std::endl;

		if(axes_array[i].cur_pos != pos){
			axes_array[i].cur_pos = pos;
			all_axis_complete = 0;
		}
	}
	// if(last_step & all_axis_complete)
	// 	exit(0);
	std::cout<<"last_step "<<last_step<<", all_axis_complete "<<all_axis_complete<<std::endl;
	return last_step & all_axis_complete;
}

void BotManipulator::doStep(){
	this->sequenceSM((*(this->time_funct))()/2);
	this->updateManipConfig();
}

void BotManipulator::updateManipConfig(){
	struct AxisDriver * axes = axes_array;

	config.rail.value =  add_config.rail.value + axes[AxisName::CAR].cur_pos+ cur_section*section_length; 
	config.tower.value = add_config.tower.value + axes[AxisName::tower].cur_pos;
	config.bpant.value = add_config.bpant.value + axes[AxisName::pantograph].cur_pos;
	config.cpant.value = add_config.cpant.value;
	config.bcar.value =  add_config.bcar.value + axes[AxisName::carrige].cur_pos; 
	config.ccar.value =  add_config.ccar.value; 
	config.wrist.value = add_config.wrist.value + axes[AxisName::turn].cur_pos;
	config.brot.value =  add_config.brot.value + axes[AxisName::rotate].cur_pos; 


}



void Manipulator::calculateMatrices(){

	float max_bcar_level = 1443.0;
	float max_ccar_level = 0.0;


	this->C = glm::translate(this->B, glm::vec3(this->config.rail.value, 0.0, -400.0)); // 
	this->D = glm::rotate(this->C, glm::radians(this->config.tower.value), glm::vec3(0.0, 0.0, 1.0));
	this->E1 = glm::translate(glm::rotate(this->D, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
										glm::vec3(this->config.bpant.value, 0.0f, 0.0f));
	this->E2 = glm::translate(this->D, glm::vec3(this->config.cpant.value, 0.0f, 0.0f));
	this->F1 = glm::translate(this->E1, glm::vec3(627.0+13.0, 0.0, -max_bcar_level + this->config.bcar.value));
	this->F2 = glm::translate(this->E2, glm::vec3(0.0, 0.0, -max_ccar_level + this->config.ccar.value));
	this->G1 = glm::rotate(this->F1, glm::radians(this->config.wrist.value), glm::vec3(0.0, 0.0, 1.0));
	this->G2 = glm::rotate(glm::translate(this->F2, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
	this->H = glm::rotate(this->G1, glm::radians(180.0f-this->config.brot.value), glm::vec3(1.0, 0.0, 0.0));
	this->I = glm::rotate(glm::translate(this->H, glm::vec3(653.0, 0.0, -1210.0)), glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));


	float bpant_length = this->config.bpant.value+334.2;
	float bangle = -asinf((bpant_length)/4450);
	glm::mat4 bpant_origin = glm::rotate(this->D, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));

	bpant_origin = glm::translate(bpant_origin, glm::vec3(-49.0, 0.0, -224.0));
	this->pb1 = glm::rotate(bpant_origin, bangle, glm::vec3(0.0, 1.0, 0.0));
	this->pb2 = glm::rotate(glm::translate(bpant_origin, glm::vec3(bpant_length/2.0, 0.0f, 0.0f)),
										bangle, glm::vec3(0.0, 1.0, 0.0));

	this->pb3 = glm::rotate(glm::translate(bpant_origin, glm::vec3(bpant_length/2.0, 0.0f, 0.0f)),
										-bangle, glm::vec3(0.0, 1.0, 0.0));
	this->pb4 = glm::rotate(glm::translate(bpant_origin, glm::vec3(bpant_length, 0.0f, 0.0f)),
										-bangle, glm::vec3(0.0, 1.0, 0.0));



	float cpant_length = this->config.cpant.value + 148.0;
	float cangle = -asinf((cpant_length)/1420);
	glm::mat4 cpant_origin = this->D;

	cpant_origin = glm::translate(cpant_origin, glm::vec3(394.0, 0.0, -511.0));
	this->pc1 = glm::rotate(cpant_origin, cangle, glm::vec3(0.0, 1.0, 0.0));
	this->pc2 = glm::rotate(glm::translate(cpant_origin, glm::vec3(cpant_length, 0.0f, 0.0f)),
										-cangle, glm::vec3(0.0, 1.0, 0.0));


}
