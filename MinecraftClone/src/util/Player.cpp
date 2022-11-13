#include "Player.h"

Player::Player(const glm::vec3& position, const glm::vec3& direction, float speed, float range)
	: m_Camera(Camera(position, direction, glm::vec3(0.0f, 1.0f, 0.0f))), m_Speed(speed), m_Range(range)
{
}

Player::~Player()
{
}

const glm::mat4& Player::GetViewMatrix()
{
	return m_Camera.GetViewMatrix();
}

const glm::vec3& Player::GetPosition()
{
	return m_Camera.GetPosition();
}

const glm::vec3& Player::GetDirection()
{
	return m_Camera.GetDirection();
}

void Player::SetPosition(Camera::Direction direction, float deltaTime)
{
	m_Camera.SetPosition(direction, m_Speed * deltaTime);
}

void Player::SetDirection(float xOffset, float yOffset)
{
	m_Camera.SetDirection(xOffset, yOffset);
}
