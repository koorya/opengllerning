
#ifndef CAMERA_H
#define CAMERA_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_movement{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera{
	glm::vec3 Position;
	glm::vec3 Direction;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	GLfloat Yaw;
	GLfloat Pitch;

	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
	bool plane_moveing;

public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = -90.0f, GLfloat pitch = 0.0f, bool pm = false);
	glm::mat4 getMatrix();
	void processKeyboard(Camera_movement direction, GLfloat delta_time);
	void processMouseMovement(GLfloat xoffset, GLfloat yoffset);
	void processMouseScroll(GLfloat yoffset);
	GLfloat getZoom();
	glm::vec3 getCamPos();
private:
	void updateCameraVectors();
};

#endif