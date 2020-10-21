#pragma once
#include <glm/glm.hpp>

#undef min
#undef max
// Axis Aligned Bounding Box
struct AABB
{
	static AABB Union(const AABB& a, const AABB& b);

	AABB(const glm::vec3& _min = glm::vec3(0), const glm::vec3& _max = glm::vec3(0)) :
		min(_min), max(_max) {}
	AABB(glm::vec3* vertex, int size);

	AABB operator + (const glm::vec3& v);
	AABB operator - (const glm::vec3& v);
	AABB operator * (const glm::vec3& v);
	AABB operator / (const glm::vec3& v);

	AABB operator * (float s);
	AABB operator / (float s);

	glm::vec3 Size() const { return max-min; }

	glm::vec3 min, max;
};

inline AABB::AABB(glm::vec3* vertex, int size)
{
	if (size <= 0)
		return;

	min = glm::vec3(FLT_MAX);
	max = glm::vec3(-FLT_MAX);
	for (int i=0; i<size; ++i) {
		min = glm::min(min, vertex[i]);
		max = glm::max(max, vertex[i]);
	}
}

inline AABB AABB::operator + (const glm::vec3& v)
{
	return AABB(this->min + v, this->max + v);
}

inline AABB AABB::operator - (const glm::vec3& v)
{
	return AABB(this->min - v, this->max - v);
}

inline AABB AABB::operator * (const glm::vec3& v)
{
	return AABB(this->min * v, this->max * v);
}

inline AABB AABB::operator / (const glm::vec3& v)
{
	return AABB(this->min / v, this->max / v);
}

inline AABB AABB::operator * (float s)
{
	return AABB(this->min * s, this->max * s);
}

inline AABB AABB::operator / (float s)
{
	return AABB(this->min / s, this->max / s);
}

inline AABB AABB::Union(const AABB& a, const AABB& b)
{
	return AABB(glm::min(a.min, b.min), glm::max(a.max, b.max));
}