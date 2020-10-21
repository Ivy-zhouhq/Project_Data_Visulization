#pragma once
#include <vector>
#include <limits>
#include <glm/glm.hpp>
#include "Volume.h"

class UniformGridVolume : public Volume
{
public:
    enum Format
    {
        UCHAR,
        USHORT,
        USHORT_12,
        UINT
    };

    static bool LoadDat(UniformGridVolume* dst, const char* dat_file);
    static bool LoadRaw(UniformGridVolume* dst, const char* raw_file, const glm::ivec3& res, Format format, int data_dim = 1);

    UniformGridVolume() : res_(0), cell_dim_(1), data_dim_(1) {}
    ~UniformGridVolume() {}

    // General get functionality
    const glm::ivec3& GetRes() const { return res_; }
    const glm::vec3& GetCellDim() const { return cell_dim_; }
    int GetDataDim() const { return data_dim_; }
    float* GetData() { return data_.data(); }
    const float* GetData() const { return data_.data(); }
    glm::vec3 GetNormalizedDim() const;

    // Get the voxel value using nearest neighbor
    virtual glm::vec4 GetNearest(const glm::vec3& coord) const;

    // Get the voxel value using linear interpolation
    virtual glm::vec4 GetLinear(const glm::vec3& coord) const;

    // Get the gradient using forward difference
    glm::vec3 GetGradientForward(const glm::vec3& coord) const;

    // Get the gradient using central difference
    glm::vec3 GetGradientCentral(const glm::vec3& coord) const;

private:
    glm::ivec3 res_;
    glm::vec3 cell_dim_;
    int data_dim_;
    std::vector<float> data_;
};