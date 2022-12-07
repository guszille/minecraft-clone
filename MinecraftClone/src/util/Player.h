#pragma once

#include <vector>
#include <algorithm>

#include <glm/glm.hpp>

#include "../graphics/core/Shader.h"

#include "../physics/AABB.h"

#include "../world/World.h"

#include "Camera.h"

class Player
{
public:
	float m_Speed, m_JumpSpeed, m_Range;

	bool m_Jumping;

public:
	Player(const glm::vec3& position, const glm::vec3& direction, float speed, float jumpSpeed, float range);
	~Player();

	const glm::mat4& GetViewMatrix();

	const glm::vec3& GetPosition();
	const glm::vec3& GetViewDirection();

	void CalculatePosition(Direction movementDirection, float deltaTime);
	void CalculateViewDirection(float xRotationOffset, float yRotationOffset);

	void Update(World* world, float gravity, float friction, float deltaTime);
	void Render(Shader* shaderProgram);

	AABB GetAABB();

	void ApplyHorizontalVelocity(Direction movementDirection);
	void ApplyVerticalVelocity(Direction movementDirection);

	void ResolveHorizontalMovement();

private:
	Camera m_Camera;

	glm::vec3 m_Position, m_Velocity;

	std::vector<glm::vec3> m_MovementIntentions;

	unsigned int m_VAO, m_VBO, m_EBO;

	void InitializeRenderData();
};
