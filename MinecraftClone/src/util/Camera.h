#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection { FORWARD, BACKWARD, UP, DOWN, RIGHT, LEFT };

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float pitch = 0.0f, const float yaw = -90.0f);
	~Camera();

	const glm::mat4& GetViewMatrix();

	const glm::vec3& GetPosition();
	const glm::vec3& GetDirection();

	void SetPosition(CameraDirection direction, float speed);
	void SetDirection(float xOffset, float yOffset);

private:
	glm::vec3 m_Position, m_Direction, m_Up;
	glm::mat4 m_ViewMatrix;

	float m_Pitch, m_Yaw; // Euler angles.
};
