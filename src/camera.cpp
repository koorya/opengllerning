#include "camera.h"

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

    if(direction == Camera_movement::FORWARD)
        this->Position += velosity * this->Direction;
    if(direction == Camera_movement::BACKWARD)
        this->Position -= velosity * this->Direction;
    if(direction == Camera_movement::RIGHT)
        this->Position += velosity * glm::normalize(glm::cross(this->Direction, this->Up));
    if(direction == Camera_movement::LEFT)
        this->Position -= velosity * glm::normalize(glm::cross(this->Direction, this->Up));
    this->updateCameraVectors();
}

glm::mat4 Camera::getMatrix(){
    return glm::lookAt(this->Position, this->Position + this->Direction, this->Up);
}

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) : Direction(glm::vec3(0.0f, 0.0f, 1.0f)), MovementSpeed(3.0f), MouseSensitivity(0.1f), Zoom(45.0f) 
{
    this->WorldUp = up;
    this->Position = position;
    this->Pitch = pitch;
    this->Yaw = yaw;
    this->updateCameraVectors();
}

