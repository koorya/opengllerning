#include "manipulator.h"


struct ManipulatorGraphMatrices m_mat[3];
struct FrameMatrices f_mat;
struct ManipulatorConfig m[3];

void initManipulatores(){
	m[0].rail = 10000.0;
	m[0].tower = 30.0;
	m[0].bpant = 3000.0;
	m[0].cpant = 500.0;
	m[0].bcar = 1000.0;
	m[0].ccar = 500.0;
	m[0].wrist = 20.0;
	m[0].brot = 60.0;

	m[1].rail = 5000.0;
	m[1].tower = 30.0;
	m[1].bpant = 0.0;
	m[1].cpant = 0.0;
	m[1].bcar = 0.0;
	m[1].ccar = 0.0;
	m[1].wrist = 0.0;
	m[1].brot = 0.0;

	m[2].rail = 12000.0;
	m[2].tower = 30.0;
	m[2].bpant = 1500.0;
	m[2].cpant = 700.0;
	m[2].bcar = 1000.0;
	m[2].ccar = 500.0;
	m[2].wrist = 20.0;
	m[2].brot = 60.0;

}

void calculateManipulatorGraphicMatrices(){

	float frame_level = 1*3000.0 ;//+ 500;
	float max_bcar_level = 1500.0;
	float max_ccar_level = 350.0;

	f_mat.World = glm::scale(glm::mat4(1.0f), glm::vec3(0.002));
	f_mat.World = glm::rotate(f_mat.World, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	f_mat.A  = glm::translate(f_mat.World, glm::vec3(0.0, 0.0, frame_level));

	//rail position on frame
	m_mat[0].B = glm::translate(f_mat.A, glm::vec3(2100.0, 6760.0, -400.0));
	m_mat[1].B = glm::translate(f_mat.A, glm::vec3(200.0, -245.0, -400.0));
	m_mat[2].B = glm::translate(f_mat.A, glm::vec3(2100.0, -7250.0, -400.0));

	for(int i = 0; i < 3; i++){
		m_mat[i].C = glm::translate(m_mat[i].B, glm::vec3(m[i].rail, 0.0, -400.0));
		m_mat[i].D = glm::rotate(m_mat[i].C, glm::radians(m[i].tower), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].E1 = glm::translate(glm::rotate(m_mat[i].D, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0)),
											glm::vec3(m[i].bpant, 0.0f, 0.0f));
		m_mat[i].E2 = glm::translate(m_mat[i].D, glm::vec3(m[i].cpant, 0.0f, 0.0f));
		m_mat[i].F1 = glm::translate(m_mat[i].E1, glm::vec3(627.0, 0.0, -max_bcar_level + m[i].bcar));
		m_mat[i].F2 = glm::translate(m_mat[i].E2, glm::vec3(0.0, 0.0, -max_ccar_level + m[i].ccar));
		m_mat[i].G1 = glm::rotate(m_mat[i].F1, glm::radians(m[i].wrist), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].G2 = glm::rotate(glm::translate(m_mat[i].F2, glm::vec3(1100.0, 0.0, -3100.0)), glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));
		m_mat[i].H = glm::rotate(m_mat[i].G1, glm::radians(m[i].brot), glm::vec3(1.0, 0.0, 0.0));
		m_mat[i].I = glm::rotate(glm::translate(m_mat[i].H, glm::vec3(653.0, 0.0, -1210.0)), glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
	}


	for(int i=0; i<3; i++){
		float pant_length = m[i].bpant+334.2;
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
		float pant_length = m[i].cpant + 148.0;
		float angle = -asinf((pant_length)/1420);
		glm::mat4 pant_origin = m_mat[i].D;

		pant_origin = glm::translate(pant_origin, glm::vec3(394.0, 0.0, -511.0));
		m_mat[i].pc1 = glm::rotate(pant_origin, angle, glm::vec3(0.0, 1.0, 0.0));
		m_mat[i].pc2 = glm::rotate(glm::translate(pant_origin, glm::vec3(pant_length, 0.0f, 0.0f)),
											-angle, glm::vec3(0.0, 1.0, 0.0));
	}

}
