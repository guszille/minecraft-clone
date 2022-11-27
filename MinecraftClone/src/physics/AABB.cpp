#include "AABB.h"

AABB::AABB(const glm::vec3& origin, const glm::vec3& min, const glm::vec3 max)
	: m_Origin(origin), m_Min(min), m_Max(max)
{
}

AABB::~AABB()
{
}
