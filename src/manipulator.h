#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "movement_program.h"


struct ManipulatorConfig{
    float rail;
    float tower;
    float bpant;
    float cpant;
    float bcar;
    float ccar;
    float wrist;
    float brot;
    struct AxisDriver axes_array[AXIS_CNT];
};

struct FrameMatrices{
    glm::mat4 World, A;
};

class Manipulator{
public:
    glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2;
    struct ManipulatorConfig config;

    Manipulator();
    void resetDrivers();
    void setProgram(int prg_id);
    int driverSM(float time);
    void updateManipConfig();
private:
    int cur_program;
    int cur_step;
    float last_time;


};

// struct ManipulatorGraphMatrices{
//     glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2;
// };

extern Manipulator m_mat[3];
extern struct FrameMatrices f_mat;


void calculateManipulatorGraphicMatrices();



#endif

