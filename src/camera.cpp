#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch, bool pm) : Direction(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed(3.0f), MouseSensitivity(0.1f), Zoom(45.0f) 
{
	this->WorldUp = up;
	this->Position = position;
	this->Pitch = pitch;
	this->Yaw = yaw;
	this->plane_moveing = pm;
	this->updateCameraVectors();
}


void Camera::updateCameraVectors(){
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

	this->Direction = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Direction, this->WorldUp));
	this->Up = glm::cross(this->Right, this->Direction);
}

void Camera::processKeyboard(Camera_movement direction, GLfloat delta_time){
	GLfloat velosity = this->MovementSpeed * delta_time;

	glm::vec3 shift = this->Direction;

	if(this->plane_moveing)
		shift = glm::normalize(shift - this->WorldUp*glm::dot(shift, this->WorldUp));

	if(direction == Camera_movement::FORWARD)
		this->Position += velosity * shift;
	if(direction == Camera_movement::BACKWARD)
		this->Position -= velosity * shift;
	if(direction == Camera_movement::RIGHT)
		this->Position += velosity * glm::normalize(glm::cross(shift, this->Up));
	if(direction == Camera_movement::LEFT)
		this->Position -= velosity * glm::normalize(glm::cross(shift, this->Up));
	this->updateCameraVectors();
}

void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset){
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;
	
	this->Yaw -= xoffset;
	this->Pitch += yoffset;
	
	if(this->Pitch > 89.0f)
		this->Pitch = 89.0f;
	if(this->Pitch < -89.0f)
		this->Pitch = -89.0f;

	this->updateCameraVectors();
}
void Camera::processMouseScroll(GLfloat yoffset){
	if(this->Zoom >= 1.0f && this->Zoom <= 45.0f)
		this->Zoom += yoffset;
	if(this->Zoom < 1.0f)
		this->Zoom = 1.0f;
	if(this->Zoom > 45.0f)
		this->Zoom = 45.0f;
	
}
glm::mat4 Camera::getMatrix(){
	return glm::lookAt(this->Position, this->Position + this->Direction, this->Up);
}
GLfloat Camera::getZoom(){
	return this->Zoom;
}

