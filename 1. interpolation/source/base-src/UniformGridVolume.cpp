#include <algorithm>
#include <string>
#include <fstream>
#include "UniformGridVolume.h"
#include <iostream>
#include <unordered_map>

using glm::vec3;
using glm::vec4;
using glm::ivec3;

using std::string;

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
    // TODO: Use round to get the nearest point.
    

    
    c.x = (res.x-1) * c.x;
    c.y = (res.y -1) * c.y;
    c.z = (res.z -1) * c.z;
    ivec3 point = glm::round(c);
    
   

    
    // TODO: Data is stored linearly as Z > Y > X, calculate the 1D-index for the point. At each data point there
    // are data_dim_ values stored. One for each data component, so the index needs to be multiplied by that value
    size_t idx = point.x + (point.y * res.x) + (point.z * res.x * res.y);
    idx *= data_dim_;   

   
    assert(idx < data_.size());
        
    return GetVec4(data_.data(), static_cast<int>(idx), data_dim_);
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
    
    c.x *= (res.x - 1);
    c.y *= (res.y - 1);
    c.z *= (res.z - 1);
    // TODO: Calculate integer pixel position and fraction.
    glm::vec4 bounding_box[2][2][2];
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                    vec3 bounding_point = vec3( i  == 0 ? floor(c.x) : ceil(c.x), j == 0 ?  floor(c.y) : ceil(c.y), k == 0 ?  floor(c.z) : ceil(c.z));  
                    size_t idx = bounding_point.x + (bounding_point.y * res.x) + (bounding_point.z * res.x * res.y);
                    idx *= data_dim_;
                    bounding_box[i][j][k] = GetVec4(data_.data(), static_cast<int>(idx), data_dim_);
            }
        }
    } //calculates the function values of the sorrunding box
  
    glm::vec4 bounding_square[2][2];
    for (int j = 0; j < 2;++j) {               // i == 1
        for (int k = 0; k < 2; ++k) {
            float alpha_x = floor(c.x) == ceil(c.x) ? 0 :  (c.x - floor(c.x) / ( ceil(c.x) - floor(c.x)));
            bounding_square[j][k] = glm::mix(bounding_box[0][j][k], bounding_box[1][j][k], alpha_x);
        }
    }

    glm::vec4 bounding_line[2];
    for (int k = 0; k < 2; ++k) {
        float alpha_y = floor(c.y) == ceil(c.y) ? 0 : (c.y - floor(c.y) / (ceil(c.y) - floor(c.y)));
        bounding_line[k] = glm::mix(bounding_square[0][k], bounding_square[1][k], alpha_y);
    }

    float alpha_z = floor(c.z) == ceil(c.z) ? 0 :  (c.z - floor(c.z) / (ceil(c.z) - floor(c.z)));
    return glm::mix(bounding_line[0], bounding_line[1], alpha_z);
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
