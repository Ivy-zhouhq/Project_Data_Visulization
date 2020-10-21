#pragma once
#include <glm/glm.hpp>
#include "AABB.h"
#include "Tetrahedron.h"

float DistancePointPlane(const glm::vec3& p, const glm::vec4& plane);

bool PointInAABB(const glm::vec3& p, const glm::vec3& min_box, const glm::vec3& max_box);
bool PointInAABB(const glm::vec3& p, const AABB& aabb);
bool PointInTetra(const glm::vec3& p, const Tetrahedron& tetra);
bool PointInPolyhedron(const glm::vec3& p, const glm::vec4* planes, int num_planes);

bool IntersectRayAABB(const glm::vec3& ray_o, const glm::vec3& ray_d, const glm::vec3& min_box, const glm::vec3& max_box, float* t0, float* t1);
bool IntersectRayAABB(const glm::vec3& ray_o, const glm::vec3& ray_d, const AABB& aabb, float* t0, float* t1);
bool IntersectTriangleAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& min_box, const glm::vec3& max_box);