#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction { FORWARD, BACKWARD, UP, DOWN, RIGHT, LEFT };

class Camera
{
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float pitch = 0.0f, const float yaw = -90.0f);
	~Camera();

	const glm::mat4& GetViewMatrix();

	const glm::vec3& GetPosition();
	const glm::vec3& GetViewDirection();

	void SetPosition(const glm::vec3& position);
	void SetViewDirection(const glm::vec3& direction);

	void CalculatePosition(Direction direction, float factor);
	void CalculateViewDirection(float xRotationOffset, float yRotationOffset);

	glm::vec3 CalculateDirection(Direction direction);

private:
	glm::vec3 m_Position, m_ViewDirection, m_Up;
	glm::mat4 m_ViewMatrix;

	float m_Pitch, m_Yaw; // Euler angles.
};
