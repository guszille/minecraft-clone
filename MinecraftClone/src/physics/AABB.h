#pragma once

#include <glm/glm.hpp>

class AABB
{
public:
	glm::vec3 m_Origin, m_Min, m_Max;

public:
	AABB(const glm::vec3& origin, const glm::vec3& min, const glm::vec3 max);
	~AABB();

private:
	// Nothing.
};
