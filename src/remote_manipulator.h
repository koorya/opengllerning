#ifndef REMOTE_MANIPULATOR
#define REMOTE_MANIPULATOR

#include "manipulator.h"
#include <mysql.h>

class RemoteManipulator : public Manipulator{
public:
	RemoteManipulator();
	void updateManipConfig();
	void resetConfiguration();
	void doStep();
private:
	MYSQL *conn;
};


#endif