#pragma once

#include <tuple>

#include <glm/glm.hpp>

// An collision can be defined by:
// 
//		- If it ocurred; and
//		- The AABB's collided face; and
//		- The distance between the AABB and the collided block, by each coordinate.
//
typedef std::tuple<bool, int, glm::vec3> Collision;

class AABB
{
public:
	glm::vec3 m_Origin, m_Base, m_Min, m_Max;

public:
	AABB(const glm::vec3& origin, const glm::vec3& base, const glm::vec3& min, const glm::vec3 max);
	~AABB();

private:
	// Nothing.
};
