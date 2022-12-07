#include "AABB.h"

AABB::AABB(const glm::vec3& origin, const glm::vec3& base, const glm::vec3& min, const glm::vec3 max)
	: m_Origin(origin), m_Base(base), m_Min(min), m_Max(max)
{
}

AABB::~AABB()
{
}
