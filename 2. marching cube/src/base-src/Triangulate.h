#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "UniformGridVolume.h"
#include "Mesh.h"
#include "AABB.h"

// Function for triangulating a uniform grid using an iso value
bool Triangulate(Mesh* mesh,
	const UniformGridVolume& volume,
	float iso,
	const AABB& bounds = AABB(glm::vec3(-1.0f), glm::vec3(1.0f)));