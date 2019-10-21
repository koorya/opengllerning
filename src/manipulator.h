#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ManipulatorConfig{
    float rail;
    float tower;
    float bpant;
    float cpant;
    float bcar;
    float ccar;
    float wrist;
    float brot;
};

struct FrameMatrices{
    glm::mat4 World, A;
};

struct ManipulatorGraphMatrices{
    glm::mat4 B, C, D, E1, E2, F1, F2, G1, G2, H, I, pb1, pb2, pb3, pb4, pc1, pc2;
};

extern struct ManipulatorGraphMatrices m_mat[3];
extern struct FrameMatrices f_mat;

extern struct ManipulatorConfig m[3];

void initManipulatores();
void calculateManipulatorGraphicMatrices();


#endif

