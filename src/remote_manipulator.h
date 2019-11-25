#ifndef REMOTE_MANIPULATOR
#define REMOTE_MANIPULATOR

#include "manipulator.h"

class RemoteManipulator : public Manipulator{
	void updateManipConfig();
	void resetConfiguration();
};


#endif