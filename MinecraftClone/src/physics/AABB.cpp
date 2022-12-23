#include "AABB.h"

AABB::AABB(const glm::vec3& origin, const glm::vec3& min, const glm::vec3 max, float knotOffset)
	: m_Origin(origin), m_Min(min), m_Max(max), m_KnotOffset(knotOffset)
{
}

AABB::~AABB()
{
}
