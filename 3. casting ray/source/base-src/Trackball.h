#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

float ProjectToSphere(float r, float x, float y)
{
	float z;

	float d2 = x*x + y*y;
	float r2 = r*r;

	if (d2 <= 0.5f * r2)
		z = sqrt(r2 - d2);
	else
		z = 0.5f * r2 / sqrt(d2);

    return z;
}

glm::quat Trackball(const glm::vec2& p0, const glm::vec2& p1, float BALLSIZE = 0.8f)
{
	if (p0 == p1)
		return glm::quat();

	glm::vec3 v0(p0.x, p0.y, ProjectToSphere(BALLSIZE, p0.x, p0.y));
	glm::vec3 v1(p1.x, p1.y, ProjectToSphere(BALLSIZE, p1.x, p1.y));

	v0 = glm::normalize(v0);
	v1 = glm::normalize(v1);

	glm::vec3 v = glm::cross(v0, v1);
	float theta = glm::acos(glm::min(1.0f, glm::dot(v0, v1)));

	return glm::angleAxis(theta, v);
}

