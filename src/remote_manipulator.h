#ifndef REMOTE_MANIPULATOR
#define REMOTE_MANIPULATOR

#include "manipulator.h"
#include <mysql.h>
#include <mutex>
#include <thread>

class RemoteManipulator : virtual public Manipulator{
public:
	RemoteManipulator(unsigned int manip_id);
	~RemoteManipulator();
	void resetConfiguration();
	void doStep();
	void updateManipConfig();

protected:
	bool destroy_flag;

private:
	static void updateThread(void * arg){
		while( ((RemoteManipulator*)arg)->destroy_flag == false)
			((RemoteManipulator*)arg)->updateManipConfig();

		((RemoteManipulator*)arg)->destroy_flag = false;
		return;
	};
	MYSQL *conn;
	std::string SQL_query;
	struct ManipulatorConfig config_rec;
	std::mutex * recieve_data;/// < блокирует обновление данных, пока происходит пересчет матриц
};


#endif