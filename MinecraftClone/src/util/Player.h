#pragma once

#include <glm/glm.hpp>

#include "Camera.h"

#include "../physics/AABB.h"

class Player
{
public:
	float m_Speed, m_Range;

public:
	Player(const glm::vec3& position, const glm::vec3& direction, float speed, float range);
	~Player();

	const glm::mat4& GetViewMatrix();

	const glm::vec3& GetPosition();
	const glm::vec3& GetDirection();

	void SetPosition(Camera::Direction direction, float deltaTime);
	void SetDirection(float xOffset, float yOffset);

	void Update(float deltaTime);

	AABB GetAABB();

private:
	Camera m_Camera;
};
