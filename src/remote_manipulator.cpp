
#include "remote_manipulator.h"

RemoteManipulator::RemoteManipulator(unsigned int manip_id): manip_id(manip_id), rf_cont(NULL){
	std::cout<<"RemoteManipulator()"<<std::endl;
	SQL_query = "SELECT kareta, tower, link_pantograph, column_pantograph, link_carige, column_carrige, wrist, link_rotation, stick_in_hand \
	 FROM configuration_new WHERE id=";
	SQL_query += std::to_string(manip_id);

	// Получаем дескриптор соединения
	conn = mysql_init(NULL);
	if (conn == NULL)
	{
		// Если дескриптор не получен – выводим сообщение об ошибке
		fprintf(stderr, "Error: can'tcreate MySQL-descriptor\n");
		//exit(1); //Если используется оконное приложение
	}
	// Подключаемся к серверу MySQLdb.connect('172.16.0.77', 'user1', 'vbtqjpxe', DATABASE_NAME)
	//194.190.149.60 - новый
	//172.16.0.77 - старый
	if (!mysql_real_connect(conn, "194.190.149.60", "user1", "vbtqjpxe", "MM", NULL, NULL, 0))
	{
		// Если нет возможности установить соединение с сервером
		// базы данных выводим сообщение об ошибке
		fprintf(stderr, "Error: can'tconnecttodatabase %s\n", mysql_error(conn));
	}
	else
	{
		// Если соединение успешно установлено выводим фразу - "Success!"
		fprintf(stdout, "Success!\n");
	}

	recieve_data = new std::mutex();

	this->destroy_flag = false;
	std::thread thr(updateThread, this);
	thr.detach();

}

RemoteManipulator::~RemoteManipulator(){
	this->destroy_flag = true;
	while(this->destroy_flag);//ожидаем, пока завершится поток для общения с БД

	mysql_close(conn);
	delete recieve_data;
}

void RemoteManipulator::updateManipConfig(){

	MYSQL_RES * res;
	MYSQL_ROW row;
	if(rf_cont != NULL)
	{
		std::string SQL_query_upd = "UPDATE sensors SET sensor1 = "+std::to_string(rf_cont->rangef_list[0].distance)
			+", sensor2 =  "+std::to_string(rf_cont->rangef_list[1].distance)
			+", sensor3 =  "+std::to_string(rf_cont->rangef_list[2].distance)
			+", sensor4 =  "+std::to_string(rf_cont->rangef_list[3].distance)+
			" WHERE id=" + std::to_string(manip_id);
		mysql_query(conn, SQL_query_upd.c_str());
		res = mysql_store_result(conn);
	}
	mysql_query(conn, SQL_query.c_str()); 
	

	if (res = mysql_store_result(conn))
	{
		while (row = mysql_fetch_row(res))
		{
			recieve_data->lock();
			config_rec.rail.value = atof(row[0]);
			config_rec.tower.value = atof(row[1]);
			config_rec.bpant.value = atof(row[2]);
			config_rec.cpant.value = atof(row[3]);
			config_rec.bcar.value = atof(row[4]);
			config_rec.ccar.value = atof(row[5]);
			config_rec.wrist.value = atof(row[6]);
			config_rec.brot.value = atof(row[7]);
			config_rec.bond_capture_actuator = atoi(row[8]);
			recieve_data->unlock();
			// for (int i = 0; i < mysql_num_fields(res); i++)
			// {
			// 	std::cout << row[i] << "\n"; //Выводим все что есть в базе через цикл
			// }
		}
		mysql_free_result(res);
	}
	else{
		fprintf(stderr, "%s\n", mysql_error(conn));
	}

//	config.rail += 100;
//	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void RemoteManipulator::resetConfiguration(){
	config.rail.value = 0.0;
	config.tower.value = 0.0;
	config.bpant.value = 0.0;
	config.cpant.value = 0.0;
	config.bcar.value = 0.0;
	config.ccar.value = 0.0;
	config.wrist.value = 0.0;
	config.brot.value = 0.0;
	config.bond_capture_actuator = false;
}

void RemoteManipulator::doStep(){
//	std::cout<<"doStep"<<std::endl;
	recieve_data->lock();
//	std::cout<<"doStep LOCK"<<std::endl;
	config.rail = config_rec.rail;
	config.tower = config_rec.tower;
	config.bpant = config_rec.bpant;
	config.cpant = config_rec.cpant;
	config.bcar = config_rec.bcar;
	config.ccar = config_rec.ccar;
	config.wrist = config_rec.wrist;
	config.brot = config_rec.brot;
	calculateMatrices();
	if(!config.bond_capture_actuator && config_rec.bond_capture_actuator)
		pickUpBond();
	if(config.bond_capture_actuator && !config_rec.bond_capture_actuator)
		mountBond();
	
	config.bond_capture_actuator = config_rec.bond_capture_actuator;
	recieve_data->unlock();
//	this->updateManipConfig();
}
