#include <iostream>
#include "Camera.h"

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(
		this->Position,
		this->Position + Front,
		this->WorldUp
	);
}

void Camera::updatePosition(glm::vec3 deltaPosition)
{
	this->Position += deltaPosition.z * this->Front;
	this->Position += deltaPosition.x * this->Right;
}

void Camera::updateRotation(double delta_xpos, double delta_ypos)
{
	this->Yaw += delta_xpos * this->MouseSensitivity;
	this->Pitch += delta_ypos * this->MouseSensitivity;
	this->updateCameraVectors();
}

void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = glm::cos(glm::radians(Yaw));
	front.z = glm::sin(glm::radians(Yaw));
	front.y = this->Front.y;
	std::cout << "FRONT: (" << this->Front.x << ", " << this->Front.y << ", " << this->Front.z << ")" << std::endl;
	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}