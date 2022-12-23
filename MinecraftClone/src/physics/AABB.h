#pragma once

#include <tuple>

#include <glm/glm.hpp>

// An collision can be defined by:
// 
//		- If it ocurred; and
//		- The block's collided face; and
//		- The distance between the collided block and the AABB, by each coordinate.
//
typedef std::tuple<bool, int, glm::vec3> Collision;

class AABB
{
public:
	glm::vec3 m_Origin, m_Min, m_Max;

	float m_KnotOffset;

public:
	AABB(const glm::vec3& origin, const glm::vec3& min, const glm::vec3 max, float knotOffset = 0.0f);
	~AABB();

private:
	// Nothing.
};
