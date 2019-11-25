
#include "remote_manipulator.h"

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

