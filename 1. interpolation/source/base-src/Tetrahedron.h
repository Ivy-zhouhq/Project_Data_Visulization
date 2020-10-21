#pragma once
#include <glm/glm.hpp>

struct Tetrahedron      
{
    Tetrahedron() {}
    Tetrahedron(
		const glm::vec3& v0,
        const glm::vec3& v1,
        const glm::vec3& v2,
        const glm::vec3& v3)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
        v[3] = v3;
    }

    float Volume() const;
	float TripleProduct(const glm::vec3& triple1, const glm::vec3& triple2, const glm::vec3& triple3) const;
    glm::vec4 BarycentricCoord(const glm::vec3& coord) const;

    glm::vec3 v[4];
};

inline float Tetrahedron::Volume() const
{
    glm::vec3 a = v[1] - v[0];
    glm::vec3 b = v[2] - v[0];
    glm::vec3 c = v[3] - v[0];

    return glm::abs(glm::dot(a, glm::cross(b,c))) / 6.0f;
}

inline float Tetrahedron::TripleProduct(const glm::vec3& triple1, const glm::vec3& triple2, const glm::vec3& triple3) const
{
	return glm::dot(triple1, glm::cross(triple2, triple3)) / 6.0f;
}

inline glm::vec4 Tetrahedron::BarycentricCoord(const glm::vec3& coord) const
{
    // TODO: Implement!
    // Also, feel free to add some member-variable to the class itself
    // if you feel like something can be precomputed.
	
	glm::vec3 vap = coord - v[0];
	glm::vec3 vbp = coord - v[1];
	glm::vec3 vab = v[1] - v[0];
	glm::vec3 vac = v[2] - v[0];
	glm::vec3 vad = v[3] - v[0];
	glm::vec3 vbc = v[2] - v[1];
	glm::vec3 vbd = v[3] - v[1];

	// Computes the scalar triple product
	float va6 = TripleProduct(vbp, vbd, vbc);
	float vb6 = TripleProduct(vap, vac, vad);
	float vc6 = TripleProduct(vap, vad, vab);
	float vd6 = TripleProduct(vap, vab, vac);
	float v6 = Volume();

	return glm::vec4(va6/v6, vb6/v6, vc6/v6, vd6/v6);
	
}