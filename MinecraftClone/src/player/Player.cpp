#include "Player.h"

Player::Player(const glm::vec3& position, const glm::vec3& direction, float speed, float jumpSpeed, float range)
	: m_Camera(Camera(position, direction, glm::vec3(0.0f, 1.0f, 0.0f))),
	  m_Position(position), m_Velocity(),
	  m_VAO(), m_VBO(), m_EBO(),
	  m_Grounded(false), m_Jumping(false), m_Speed(speed), m_JumpSpeed(jumpSpeed), m_Range(range)
{
	InitializeRenderData();
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
	return m_Camera.GetPosition(); // Or m_Position.
}

const glm::vec3& Player::GetViewDirection()
{
	return m_Camera.GetViewDirection();
}

void Player::CalculatePosition(Direction movementDirection, float deltaTime)
{
	m_Camera.CalculatePosition(movementDirection, m_Speed * deltaTime);

	m_Position = m_Camera.GetPosition();
}

void Player::CalculateViewDirection(float xRotationOffset, float yRotationOffset)
{
	m_Camera.CalculateViewDirection(xRotationOffset, yRotationOffset);
}

void Player::Update(World* world, float gravity, float friction, float deltaTime)
{
	if (m_Velocity.x != 0.0f)
	{
		if (m_Velocity.x > 0.0f)
		{
			m_Velocity.x = std::max(m_Velocity.x - (friction * deltaTime), 0.0f);
		}
		else if (m_Velocity.x < 0.0f)
		{
			m_Velocity.x = std::min(m_Velocity.x + (friction * deltaTime), 0.0f);
		}

		m_Position.x += m_Velocity.x * deltaTime;
	}

	m_Velocity.y = m_Velocity.y + (gravity * deltaTime);

	if (m_Velocity.y != 0.0f)
	{
		m_Position.y += m_Velocity.y * deltaTime;
	}

	if (m_Velocity.z != 0.0f)
	{
		if (m_Velocity.z > 0.0f)
		{
			m_Velocity.z = std::max(m_Velocity.z - (friction * deltaTime), 0.0f);
		}
		else if (m_Velocity.z < 0.0f)
		{
			m_Velocity.z = std::min(m_Velocity.z + (friction * deltaTime), 0.0f);
		}

		m_Position.z += m_Velocity.z * deltaTime;
	}

	AABB aabb = GetAABB();
	glm::vec3 aabbMaxDistanceOfCollision(1.0f, 2.0f, 1.0f); // Based on the knowing shape of the player's AABB.
	float aabbMaxRangeOfCollision = glm::length(aabbMaxDistanceOfCollision);

	std::vector<Collision> collisions = world->CheckCollisions(aabb, aabbMaxRangeOfCollision);
	bool collisionsChecked[6] = { false, false, false, false, false, false }; // Only one collision detection per direction.

	//	std::sort(std::begin(collisions), std::end(collisions), [](const Collision& c1,const Collision& c2) {
	//		return std::get<1>(c1) < std::get<1>(c2);
	//	});

	for (Collision c : collisions)
	{
		if (std::get<0>(c))
		{
			int collidedFace = std::get<1>(c);
			glm::ivec3 collidedNormal = Cube::s_Normals[collidedFace];

			if (!collisionsChecked[collidedFace])
			{
				if (collidedFace == BlockFace::TOP || collidedFace == BlockFace::BOTTOM)
				{
					if (collidedFace == BlockFace::TOP)
					{
						m_Jumping = false;
					}

					m_Position.y = m_Position.y + (collidedNormal.y * (aabbMaxDistanceOfCollision.y - std::get<2>(c).y));
					m_Velocity.y = 0.0f;
				}
				else if (collidedFace == BlockFace::RIGHT || collidedFace == BlockFace::LEFT)
				{
					m_Position.x = m_Position.x + (collidedNormal.x * (aabbMaxDistanceOfCollision.x - std::get<2>(c).x));
					m_Velocity.x = 0.0f;
				}
				else if (collidedFace == BlockFace::FRONT || collidedFace == BlockFace::BACK)
				{
					m_Position.z = m_Position.z + (collidedNormal.z * (aabbMaxDistanceOfCollision.z - std::get<2>(c).z));
					m_Velocity.z = 0.0f;
				}

				collisionsChecked[collidedFace] = true;
			}
		}
	}

	m_Grounded = collisionsChecked[BlockFace::TOP];

	m_Camera.SetPosition(m_Position);
}

void Player::Render(Shader* shaderProgram)
{
	int currentPolygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &currentPolygonMode);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(modelMatrix, m_Position);

	shaderProgram->Bind();
	shaderProgram->SetUniformMatrix4fv("uModelMatrix", modelMatrix);
	shaderProgram->SetUniformMatrix4fv("uViewMatrix", m_Camera.GetViewMatrix());
	shaderProgram->SetUniform3f("uColor", glm::vec3(0.0f, 0.0f, 1.0f));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, currentPolygonMode);

	shaderProgram->Unbind();
}

AABB Player::GetAABB()
{
	return AABB(m_Position - glm::vec3(0.0f, 1.0f, 0.0f), m_Position - glm::vec3(0.5f, 2.5f, 0.5f), m_Position + glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
}

void Player::ApplyHorizontalVelocity(Direction movementDirection)
{
	if (movementDirection == Direction::FORWARD || movementDirection == Direction::BACKWARD || movementDirection == Direction::RIGHT || movementDirection == Direction::LEFT)
	{
		glm::vec3 direction = m_Camera.CalculateDirection(movementDirection);

		m_MovementIntentions.push_back(direction);
	}
}

void Player::ApplyVerticalVelocity(Direction movementDirection)
{
	if (movementDirection == Direction::UP)
	{
		if (m_Grounded && !m_Jumping)
		{
			m_Velocity.y = m_JumpSpeed;

			m_Jumping = true;
		}
	}
}

void Player::ResolveHorizontalMovement()
{
	glm::vec3 direction(0.0f);

	for (int i = 0; i < m_MovementIntentions.size(); i++)
	{
		direction += m_MovementIntentions[i];
	}

	direction = glm::normalize(direction);

	m_Velocity.x = direction.x * m_Speed;
	m_Velocity.z = direction.z * m_Speed;

	m_MovementIntentions.clear();
}

void Player::InitializeRenderData()
{
	float vertices[] = {
		// front
		-0.5f, -2.5f,  0.5f,
		 0.5f, -2.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -2.5f, -0.5f,
		 0.5f, -2.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};

	unsigned int indices[] = {
		// front
		0, 1, 2, 2, 3, 0,
		// right
		1, 5, 6, 6, 2, 1,
		// back
		7, 6, 5, 5, 4, 7,
		// left
		4, 0, 3, 3, 7, 4,
		// bottom
		4, 5, 1, 1, 0, 4,
		// top
		3, 2, 6, 6, 7, 3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
