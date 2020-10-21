#include <algorithm>
#include <string>
#include <fstream>
#include "UniformGridVolume.h"

using glm::vec3;
using glm::vec4;
using glm::ivec3;

using std::string;

inline int Index(const ivec3& coord, const ivec3& res)
{
	return res.x * res.y * coord.z + res.x * coord.y + coord.x;
}

inline float Interpolate(float v0, float v1, float t)
{
	return (1.f - t) * v0 + t * v1;
}

inline vec4 GetVec4(const float* data, int idx, int data_dim)
{
	vec4 val(data[idx]);
	for (int i = 1; i < data_dim; ++i)
		val[i] = data[idx + i];

	return val;
}

glm::vec3 UniformGridVolume::GetNormalizedDim() const
{
	vec3 dim = vec3(res_) * cell_dim_;
	float max_dim = glm::max(dim.x, glm::max(dim.y, dim.z));
	if (max_dim == 0.f)
		return vec3(0.f);

	return dim / max_dim;
}

// Get the voxel value of the nearest neighbor
// coord is the normalized coordinate in volume space [0,1]
vec4 UniformGridVolume::GetNearest(const vec3& coord) const
{
	if (data_.size() == 0)
		return vec4(0);

	vec3 c = glm::clamp(coord, 0.f, 1.f);
	ivec3 res = GetRes();
	// Use round to get the nearest point
	ivec3 p = glm::round(c * vec3(res - 1));

	// Data is stored linearly as Z > Y > X, calculate the 1D-index for the point.
	size_t idx = Index(p, res) * data_dim_;
	assert(idx < data_.size());
		
	return GetVec4(data_.data(), idx, data_dim_);
}

// Get the voxel value using linear interpolation
// coord is the normalized coordinate in volume space [0,1]
vec4 UniformGridVolume::GetLinear(const vec3& coord) const
{
	if (data_.size() == 0)
		return vec4(0);

	// Clamp to nearly one to avoid issues
	vec3 c = glm::clamp(coord, 0.f, 0.9999f);
	ivec3 res = GetRes();
	ivec3 p = glm::floor(c * vec3(res - 1));
	vec3 cc = glm::fract(c * vec3(res - 1));

	// Get all samples
	vec4 v000 = GetVec4(data_.data(), Index(p + ivec3(0, 0, 0), res) * data_dim_, data_dim_);
	vec4 v100 = GetVec4(data_.data(), Index(p + ivec3(1, 0, 0), res) * data_dim_, data_dim_);
	vec4 v010 = GetVec4(data_.data(), Index(p + ivec3(0, 1, 0), res) * data_dim_, data_dim_);
	vec4 v110 = GetVec4(data_.data(), Index(p + ivec3(1, 1, 0), res) * data_dim_, data_dim_);
	vec4 v001 = GetVec4(data_.data(), Index(p + ivec3(0, 0, 1), res) * data_dim_, data_dim_);
	vec4 v101 = GetVec4(data_.data(), Index(p + ivec3(1, 0, 1), res) * data_dim_, data_dim_);
	vec4 v011 = GetVec4(data_.data(), Index(p + ivec3(0, 1, 1), res) * data_dim_, data_dim_);
	vec4 v111 = GetVec4(data_.data(), Index(p + ivec3(1, 1, 1), res) * data_dim_, data_dim_);

	// Interpolate x
	vec4 v00 = glm::mix(v000, v100, cc.x);
	vec4 v01 = glm::mix(v010, v110, cc.x);
	vec4 v10 = glm::mix(v001, v101, cc.x);
	vec4 v11 = glm::mix(v011, v111, cc.x);

	// Interpolate y
	vec4 v0 = glm::mix(v00, v01, cc.y);
	vec4 v1 = glm::mix(v10, v11, cc.y);

	// Interpolate z
	vec4 v = glm::mix(v0, v1, cc.z);

	return v;
}

// Get the gradient using forward difference
glm::vec3 UniformGridVolume::GetGradientForward(const glm::vec3& coord) const
{
	// TODO: Implement me!
	return vec3(0);
}

// Get the gradient using central difference
glm::vec3 UniformGridVolume::GetGradientCentral(const glm::vec3& coord) const
{
	// TODO: Or me!
	return vec3(0);
}

bool UniformGridVolume::LoadDat(UniformGridVolume* vol, const char* dat_file)
{
	if (vol == nullptr) {
		printf("Nullpointer was passed. \n");
		return false;
	}
	string filename = dat_file;
	string ending = filename.substr(filename.size()-3, 3);
	string path = filename.substr(0, filename.find_last_of("/\\"));

	if (ending != "dat" && ending != "DAT") {
		printf("Cannot load '%s' Only .dat files are supported.\n", ending.c_str());
		return false;
	}

	std::ifstream ifs(dat_file);
	if (!ifs.is_open()) {
		printf("Could not open file '%s'. \n", filename.c_str());
		return false;
	}

	string str, raw_file, format_str, obj_model;
	ivec3 res;
	vec3 cell_dim(1.f);

	while (ifs >> str && ifs.good()) {
		if (str == "ObjectFileName:")
			ifs >> raw_file;
		else if (str == "Resolution:") {
			ifs >> res.x; ifs >> res.y; ifs >> res.z;
		}
		else if (str == "SliceThickness:") {
			ifs >> cell_dim.x; ifs >> cell_dim.y; ifs >> cell_dim.z;
		}
		else if (str == "Format:")
			ifs >> format_str;
		else if (str == "ObjectModel:")
			ifs >> obj_model;
	}
	ifs.close();

	if(raw_file == "" || res == ivec3(0) || format_str == "") {
		printf("Could find all required fields in dat file. \n");
		return false;
	}

	Format format;
	if(format_str == "UCHAR")
		format = UCHAR;
	else if(format_str == "USHORT")
		format = USHORT;
	else if(format_str == "USHORT_12")
		format = USHORT_12;
	else if(format_str == "UINT")
		format = UINT;
	else {
		printf("Format '%s' is not supported. \n", format_str.c_str());
		return false;
	}

	int data_dim = 1;
	if (obj_model == "I")
		data_dim = 1;
	else if (obj_model == "RG" || obj_model == "XY")
		data_dim = 2;
	else if (obj_model == "RGB" || obj_model == "XYZ")
		data_dim = 3;
	else if (obj_model == "RGBA" || obj_model == "XYZW")
		data_dim = 4;

	vol->cell_dim_ = cell_dim;
	raw_file = path + "/" + raw_file;
	return LoadRaw(vol, raw_file.c_str(), res, format, data_dim);
}

bool UniformGridVolume::LoadRaw(UniformGridVolume* vol,
	const char* raw_file,
	const ivec3& res,
	Format format,
	int data_dim)
{
	std::ifstream ifs(raw_file, std::ios::binary);
	if (!ifs.is_open()) {
		printf("Could not open file '%s'. \n", raw_file);
		return false;
	}

	ifs.seekg(0, std::ios::end);
	int data_size = (int)ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	char* raw_data = new char[data_size];
	ifs.read(raw_data, data_size);

	vol->res_ = res;
	int vol_size = res.x * res.y * res.z * data_dim;
	vol->data_.resize(vol_size);
	vol->data_dim_ = data_dim;

	if(format == UCHAR) {
		int size = std::min(data_size, vol_size);
		unsigned char* ptr = (unsigned char*)raw_data;
		for (int i = 0; i < size; ++i) {
			vol->data_[i] = ptr[i] / 255.0f;
		}
	} else if(format == USHORT || format == USHORT_12) {
		int size = std::min(data_size / (int)sizeof(short), vol_size);
		unsigned short* ptr = (unsigned short*)raw_data;
		float max_val = format == USHORT ? 65535.0f : 4095.0f;
		for (int i = 0; i < size; ++i) {
			vol->data_[i] = ptr[i] / max_val;
		}
	} else if(format == UINT) {
		int size = std::min(data_size / (int)sizeof(int), vol_size);
		unsigned int* ptr = (unsigned int*)raw_data;
		for (int i = 0; i < size; ++i) {
			vol->data_[i] = ptr[i] / 4294967295.0f;
		}
	}

	delete[] raw_data;
	return true;
}

void UniformGridVolume::Resample(UniformGridVolume* dst, const UniformGridVolume& src, const ivec3& res)
{
	if (!dst) {
		printf("Nullpointer was passed \n");
		return;
	}

	int size = res.x * res.y * res.z;
	if (size < 1) {
		printf("Resolution was less than or equal to zero \n");
		return;
	}

	dst->res_ = res;
	dst->data_.resize(size);
	vec3 res_scl = 1.f / vec3(res-1);
	for (int z=0; z<res.z; ++z) {
		for (int y=0; y<res.y; ++y) {
			for (int x=0; x<res.x; ++x) {
				dst->data_[Index(ivec3(x,y,z), res)] = src.GetLinear(vec3(x,y,z) * res_scl).x;
			}
		}
	}
}