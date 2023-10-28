#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up, const float pitch, const float yaw)
	: m_Position(position), m_ViewDirection(direction), m_Up(up), m_Pitch(pitch), m_Yaw(yaw)
{
	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewDirection, m_Up);
}

Camera::~Camera()
{
}

const glm::mat4& Camera::GetViewMatrix()
{
	return m_ViewMatrix;
}

const glm::vec3& Camera::GetPosition()
{
	return m_Position;
}

const glm::vec3& Camera::GetViewDirection()
{
	return m_ViewDirection;
}

void Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewDirection, m_Up);
}

void Camera::SetViewDirection(const glm::vec3& direction)
{
	m_ViewDirection = glm::normalize(direction);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewDirection, m_Up);
}

void Camera::CalculatePosition(Direction direction, float factor)
{
	m_Position += CalculateDirection(direction) * factor;

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewDirection, m_Up);
}

void Camera::CalculateViewDirection(float xRotationOffset, float yRotationOffset)
{
	m_Yaw += xRotationOffset;
	m_Pitch += yRotationOffset;

	m_Pitch = std::min(std::max(m_Pitch, -89.0f), 89.0f);

	glm::vec3 direction(0.0f);

	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_ViewDirection = glm::normalize(direction);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewDirection, m_Up);
}

glm::vec3 Camera::CalculateDirection(Direction direction)
{
	glm::vec3 R = glm::normalize(glm::cross(m_ViewDirection, m_Up));
	glm::vec3 F = glm::normalize(glm::cross(R, m_Up));
	glm::vec3 D = glm::vec3(0.0f);

	switch (direction)
	{
	case Direction::FORWARD:  D = -F;	 break;
	case Direction::BACKWARD: D = +F;	 break;
	case Direction::UP:		  D = +m_Up; break;
	case Direction::DOWN:	  D = -m_Up; break;
	case Direction::RIGHT:	  D = +R;	 break;
	case Direction::LEFT:	  D = -R;	 break;
	default: break;
	}

	return D;
}
