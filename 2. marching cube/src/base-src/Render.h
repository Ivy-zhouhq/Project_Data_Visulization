#pragma once
#include <glm/glm.hpp>

struct RenderParam
{
    RenderParam() :
        res(512), fov(glm::radians(60.f)), near_{ 0.1f }, far_(100.0f) {}

    glm::mat4 model_matrix;
    glm::mat4 view_matrix;

    glm::ivec2 res;
    float fov;
    float near_;
    float far_;

    float AspectRatio() const { return (float)res.x / (float)res.y; }
};

void CalcViewDirections(glm::vec3 view_dirs[4], const RenderParam& param);