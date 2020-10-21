#pragma once
#include <glm/glm.hpp>

// Abstract class for Volume data
class Volume
{
public:
	enum Interpolation
	{
		NEAREST = 0,
		LINEAR = 1
	};

	Volume() {}
	virtual ~Volume() {}

	// Get the voxel value using nearest neighbor
	virtual glm::vec4 GetNearest(const glm::vec3& coord) const = 0;

	// Get the voxel value using linear interpolation
	virtual glm::vec4 GetLinear(const glm::vec3& coord) const = 0;

};