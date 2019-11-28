
#include "remote_manipulator.h"

RemoteManipulator::RemoteManipulator() : Manipulator(){

	MYSQL_RES *res;
	MYSQL_ROW row;
	// Получаем дескриптор соединения
	conn = mysql_init(NULL);
	if (conn == NULL)
	{
		// Если дескриптор не получен – выводим сообщение об ошибке
		fprintf(stderr, "Error: can'tcreate MySQL-descriptor\n");
		//exit(1); //Если используется оконное приложение
	}
	// Подключаемся к серверу MySQLdb.connect('172.16.0.77', 'user1', 'vbtqjpxe', DATABASE_NAME)
	if (!mysql_real_connect(conn, "172.16.0.77", "user1", "vbtqjpxe", "MM_dev", NULL, NULL, 0))
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

	mysql_query(conn, "SELECT id, link_pantograph FROM configuration_new"); //Делаем запрос к таблице по имени МНУ =)

	if (res = mysql_store_result(conn))
	{
		while (row = mysql_fetch_row(res))
		{
			for (int i = 0; i < mysql_num_fields(res); i++)
			{
				std::cout << row[i] << "\n"; //Выводим все что есть в базе через цикл
			}
		}
	}
	else
		fprintf(stderr, "%s\n", mysql_error(conn));

	// Закрываем соединение с сервером базы данных
	mysql_close(conn);

}

void RemoteManipulator::updateManipConfig(){
	config.rail += 1.0;
}

void RemoteManipulator::resetConfiguration(){
	config.rail = 0.0;
	config.tower = 0.0;
	config.bpant = 0.0;
	config.cpant = 0.0;
	config.bcar = 0.0;
	config.ccar = 0.0;
	config.wrist = 0.0;
	config.brot = 0.0;

}

void RemoteManipulator::doStep(){
	this->updateManipConfig();
}
