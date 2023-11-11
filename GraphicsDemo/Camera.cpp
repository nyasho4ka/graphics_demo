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
	this->Position += deltaPosition * this->Front;
	this->Position -= deltaPosition * this->Right;
}