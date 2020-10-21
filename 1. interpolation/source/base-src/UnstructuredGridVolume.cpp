#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include "UnstructuredGridVolume.h"
#include "AABB.h"
#include "Intersection.h"

using glm::vec3;
using glm::vec4;
using glm::ivec3;
using glm::ivec4;

void UnstructuredGridVolume::CreatePlanes()
{
    planes_.resize(faces_.size());
    for (size_t i = 0; i < faces_.size(); ++i) {
        vec3 a = points_[faces_[i][0]];
        vec3 b = points_[faces_[i][1]];
        vec3 c = points_[faces_[i][2]];
        vec3 n = glm::normalize(glm::cross(b - a, c - a));
        float d = glm::dot(n, a);

        planes_[i] = glm::vec4(n, d);
    }
}

void UnstructuredGridVolume::CreateTetras()
{
    tetras_.resize(tetra_indices_.size());
    for (size_t i = 0; i < tetra_indices_.size(); ++i) {
        const glm::ivec4& ti = tetra_indices_[i];

        const vec3& a = points_[ti[0]];
        const vec3& b = points_[ti[1]];
        const vec3& c = points_[ti[2]];
        const vec3& d = points_[ti[3]];

        tetras_[i] = Tetrahedron(a, b, c, d);
    }
}

// Get the voxel value using nearest neighbor
// coord is the normalized coordinate in volume space
vec4 UnstructuredGridVolume::GetNearest(const vec3& nc) const
{
    // Transform normalized coordinates nc into volume space
    vec3 coord = point_bounds_.min + nc * point_bounds_.Size();

    // Check if the coordinate is inside of the convex hull
    if (!PointInPolyhedron(coord, planes_.data(), (int)planes_.size()))
        return vec4(0);

    float min_d2 = FLT_MAX;
    int min_id = -1;

    // Brute force! Slow!
    // Check against all points to find the closest
    for (int i = 0; i < (int)points_.size(); ++i) {
        vec3 v = points_[i] - coord;
        float d2 = glm::dot(v, v);
        if (d2 < min_d2) {
            min_d2 = d2;
            min_id = i;
        }
    }

    if (min_id > -1)
        return data_[min_id];
 
    return vec4(0.f);
}



// Get the voxel value using linear interpolation
// coord is the normalized coordinate in volume space
glm::vec4 UnstructuredGridVolume::GetLinear(const vec3& nc) const
{
    // Transform normalized coordinates nc into volume space
    vec3 coord = point_bounds_.min + nc * point_bounds_.Size();

    // Check if the coordinate is inside of the convex hull
    if (!PointInPolyhedron(coord, planes_.data(), (int)planes_.size()))
        return vec4(0);

	// TODO: Implement Barycentric interpolation
	
	glm::vec4 f;
	for (size_t i = 0; i < tetra_indices_.size(); ++i) 
	{
		glm::vec4 bary_cord = tetras_[i].BarycentricCoord(vec3(coord));
		if ((bary_cord[0] >= 0) && (bary_cord[1] >= 0) && (bary_cord[2] >= 0) && (bary_cord[3] >= 0))
		{
			glm::vec4 indice = tetra_indices_[i];

			glm::vec4 f = data_[indice[0]] * bary_cord[0] + data_[indice[1]] * bary_cord[1]
				+ data_[indice[2]] * bary_cord[2] + data_[indice[3]] * bary_cord[3];
			return f;
		}	
	}
	return vec4(0.f);
}




bool UnstructuredGridVolume::LoadNode(UnstructuredGridVolume* vol, const char* node_file)
{
    if (vol == nullptr) {
        printf("A nullptr was passed. \n");
        return false;
    }

    std::ifstream node_stream(node_file);
    if (!node_stream.is_open()) {
        printf("'%s' could not be opened. \n", node_file);
        return false;
    }

    std::string line;
    getline(node_stream, line);
    std::stringstream ss(line);

    int node_count, node_dim, node_attr;
    ss >> node_count;
    ss >> node_dim;
    ss >> node_attr;

    if(node_dim != 3) {
        printf("Node dimensionality is different from 3. \n");
        return false;
    }

    // Only 0 to 4 attributes are supported per point
    node_attr = glm::clamp(node_attr, 0, 4);

    vol->data_.resize(node_count);
    vol->points_.resize(node_count);

    for (int i = 0; i < node_count; ++i) {
        getline(node_stream, line);
        std::stringstream ss(line);
        int idx;
        glm::vec3 pos;
        glm::vec4 attr;

        ss >> idx;
        ss >> pos.x; ss >> pos.y; ss >> pos.z;

        for (int a=0; a<node_attr; ++a)
            ss >> attr[a];

        vol->points_[i] = pos;
        vol->data_[i] = attr;
    }

    // Update Bounds
    vol->point_bounds_ = AABB(vol->points_.data(), (int)vol->points_.size());

    return true;
}

bool UnstructuredGridVolume::LoadEle(UnstructuredGridVolume* vol, const char* ele_file)
{
    std::ifstream ele_stream(ele_file);
    if (!ele_stream.is_open()) {
        printf("'%s' could not be opened. \n", ele_file);
        return false;
    }

    std::string line;
    getline(ele_stream, line);
    std::stringstream ss(line);

    int ele_count, ele_dim;
    ss >> ele_count;
    ss >> ele_dim;

    if(ele_dim != 4) {
        printf("Element dimensionality is different from 4. \n");
        return false;
    }

    vol->tetra_indices_.resize(ele_count);

    for (int i = 0; i < ele_count; ++i) {
        getline(ele_stream, line);
        std::stringstream ss(line);
        int idx;
        glm::ivec4 tetra;

        ss >> idx;
        ss >> tetra[0]; ss >> tetra[1]; ss >> tetra[2]; ss >> tetra[3];

        vol->tetra_indices_[i] = tetra;
    }
    return true;
}

bool UnstructuredGridVolume::LoadFace(UnstructuredGridVolume* vol, const char* face_file)
{
    std::ifstream face_stream(face_file);
    if (!face_stream.is_open()) {
        printf("'%s' could not be opened. \n", face_file);
        return false;
    }

    std::string line;
    getline(face_stream, line);
    std::stringstream ss(line);

    int face_count;
    ss >> face_count;

    vol->faces_.resize(face_count);

    for (int i = 0; i < face_count; ++i) {
        getline(face_stream, line);
        std::stringstream ss(line);
        int idx;
        glm::ivec3 face;

        ss >> idx;
        ss >> face[0]; ss >> face[1]; ss >> face[2];

        vol->faces_[i] = face;
    }
    return true;
}