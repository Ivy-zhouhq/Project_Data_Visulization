#pragma once
#include <glm/glm.hpp>

struct Tetrahedron
{
	Tetrahedron() {}
	Tetrahedron(const glm::vec3& v0,
		const glm::vec3& v1,
		const glm::vec3& v2,
		const glm::vec3& v3)
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
		v[3] = v3;

		glm::mat3 T(v[0] - v[3], v[1] - v[3], v[2] - v[3]);
		T_inv = glm::inverse(T);
	}

	float Volume() const;
	glm::vec4 BarycentricCoord(const glm::vec3& coord) const;

	glm::vec3 v[4];
	glm::mat3 T_inv;
};

inline float Tetrahedron::Volume() const
{
	glm::vec3 a = v[1] - v[0];
	glm::vec3 b = v[2] - v[0];
	glm::vec3 c = v[3] - v[0];

	return glm::abs(glm::dot(a, glm::cross(b,c))) / 6.0f;
}

inline glm::vec4 Tetrahedron::BarycentricCoord(const glm::vec3& coord) const
{
	// TODO: Implement!
	// Also, feel free to add some member-variable to the class itself
	// if you feel like something can be precomputed.

	glm::vec4 bc(T_inv * (coord - v[3]), 0.f);
	bc.w = 1.0f - bc.x - bc.y - bc.z;
	return bc;
}