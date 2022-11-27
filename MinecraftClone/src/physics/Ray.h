#pragma once

#include <tuple>

#include <glm/glm.hpp>

// An intersection can be defined by:
// 
//		- If it ocurred; and
//		- The position of the intersected chunk; and
//		- The position of the intersected block, inside the chunk (local position); and
//		- The block's global position on the world; and
//		- The block's intersected face.
//
typedef std::tuple<bool, std::pair<int, int>, glm::ivec3, glm::vec3, int> Intersection;

class Ray
{
public:
	glm::vec3 m_Origin, m_Direction, m_InverseDirection;

	float m_Length;

public:
	Ray(const glm::vec3& origin, const glm::vec3& direction, float length);
	~Ray();

private:
	// Nothing.
};
