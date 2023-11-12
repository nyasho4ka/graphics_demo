#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up;
	glm::vec3 Right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);	

	glm::mat4 getViewMatrix();
	void updatePosition(glm::vec3 deltaPosition);
	void updateRotation(double delta_xpos, double delta_ypos);
private:
	float MouseSensitivity = 0.05f;
	float MovementSpeed = 0.5f;
	float Zoom = 1.0f;
	float Yaw = -90.0f;
	float Pitch = 0.0f;
	void updateCameraVectors();
};