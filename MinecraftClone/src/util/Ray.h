#pragma once

#include <tuple>

#include <glm/glm.hpp>

typedef std::tuple<bool, glm::vec3> Collision;

class Ray
{
public:
	glm::vec3 m_Origin, m_Direction;

	float m_Length;

public:
	Ray(const glm::vec3& origin, const glm::vec3& direction, float length);
	~Ray();

private:
	// Nothing.
};
