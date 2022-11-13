#include "Ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction, float length)
	: m_Origin(origin), m_Direction(direction), m_Length (length)
{
}

Ray::~Ray()
{
}
