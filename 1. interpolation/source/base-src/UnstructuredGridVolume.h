#pragma once
#include <vector>
#include "Volume.h"
#include "AABB.h"
#include "Tetrahedron.h"

// This is a sparse Unstructured Grid Volume that
// contains data stored in points and additional tetrahedrons
// that determine the neighborhood for the interpolation.
class UnstructuredGridVolume : public Volume
{
public:
	static bool LoadNode(UnstructuredGridVolume* vol, const char* node_file);
	static bool LoadEle(UnstructuredGridVolume* vol, const char* ele_file);
	static bool LoadFace(UnstructuredGridVolume* vol, const char* face_file);

	UnstructuredGridVolume() {}
	~UnstructuredGridVolume() {}

	const AABB& GetBounds() const { return point_bounds_; }
	const glm::vec4* GetData() const { return data_.data(); }
	const glm::vec3* GetPoints() const { return points_.data(); }
	const glm::ivec4* GetTetraIndices() const { return tetra_indices_.data(); }
	const Tetrahedron* GetTetras() const { return tetras_.data(); }
	const glm::ivec3* GetFaces() const { return faces_.data(); }
	const glm::vec4* GetPlanes() const { return planes_.data(); }

	void CreatePlanes();
	void CreateTetras();

	// Get the voxel value using nearest neighbor
	virtual glm::vec4 GetNearest(const glm::vec3& coord) const;

	// Get the voxel value using linear interpolation
	virtual glm::vec4 GetLinear(const glm::vec3& coord) const;

private:
	// Bounds of the entire dataset
	AABB point_bounds_;

	// The actual data of each point
	std::vector<glm::vec4> data_;

	// The position of each point
	std::vector<glm::vec3> points_;

	// Indices to the points of the Tetras
	std::vector<glm::ivec4> tetra_indices_;

	// Tetrahedrons
	std::vector<Tetrahedron> tetras_;

	// Convex Hull Faces
	std::vector<glm::ivec3> faces_;

	// Convex Hull Planes
	std::vector<glm::vec4> planes_;
};

