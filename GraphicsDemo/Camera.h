#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

class Camera {
public:
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up;
	glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 getViewMatrix();
	void updatePosition(glm::vec3 deltaPosition);
private:
	void updateCameraVectors();
};