#include <algorithm>
#include <thread>
#include "RaycasterCPU.h"
#include "AABB.h"
#include "Intersection.h"

using namespace glm;

inline vec3 WorldToVolumeCoord(const vec3& x, const vec3& min_box, const vec3& max_box)
{
    return (x - min_box) / (max_box - min_box);
}

inline vec3 InterpolateViewDir(vec3 view_dir[4], const vec2& tex_coord)
{
    vec3 btm_d = mix(view_dir[0], view_dir[2], tex_coord.x);
    vec3 top_d = mix(view_dir[1], view_dir[3], tex_coord.x);
    return normalize(mix(btm_d, top_d, tex_coord.y));
}

RaycasterCPU::RaycasterCPU()
{
    glGenTextures(1, &texture_);

    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

RaycasterCPU::~RaycasterCPU()
{
    glDeleteTextures(1, &texture_);
}

void RaycasterCPU::Render(const RenderParam& param, const UniformGridVolume& vol, const TransferFunction& tf, const vec3& size, float stepsize)
{
    // Possibly resize texture data buffer
    int req_size = param.res.x * param.res.y;
    if (req_size != tex_data_.size())
        tex_data_.resize(req_size);

    vec3 view_pos = vec3(glm::inverse(param.view_matrix) * vec4(0, 0, 0, 1));
    vec3 view_dir[4];
    CalcViewDirections(view_dir, param);

    // Ray origin, the view point
    vec3 ray_o = view_pos;

    // Bounding Box representing the bounds of our volume representation in the scene
    AABB aabb(vec3(-0.5f) * size, vec3(0.5f) * size);

    // Iterate over all output pixels
    for (int y = 0; y < param.res.y; ++y) {
        for (int x = 0; x < param.res.x; ++x) {
            // Calculate tex coord [0,1]
            vec2 tex_coord = vec2(x, y) / vec2(param.res - 1);

            // Interpolate ray dir from view dirs using tex coord
            vec3 ray_d = InterpolateViewDir(view_dir, tex_coord);

            // Reset Color and Alpha
            vec3 C(0.f);
            float A = 0.f;

            // Check for intersection between ray and vol-bounds
            float t0, t1;
            if (IntersectRayAABB(ray_o, ray_d, aabb, &t0, &t1))
            {
                float t = t0;
                while (t < t1) {
                // Initialize t to entry point
                

                // World Space Position
                vec3 p = ray_o + t * ray_d;

                // Volume Space Coordinates
                vec3 coord = WorldToVolumeCoord(p, aabb.min, aabb.max);

                // Intensity value of volume data
                float s = vol.GetLinear(coord).r;

                // Transfer function lookup
                vec4 color = tf.RGBA(s);

                // TODO: forward-to-back emission absorption
                // Hint: the lines above examples on how to
                // sample the volume and the transferfunction.
                if (color.a > 0) {
                    C = C + (1 - A) * color.a * vec3(color.r, color.g, color.b);
                    A = A + (1 - A) * color.a;
                }
                if (A >= 1) t = t1;
                // TODO: forward-to-back emission absorption
                // Hint: the lines above examples on how to
                // sample the volume and the transferfunction.
                t += stepsize;
            }
            }

            // Write Color to pixel
            tex_data_[y * param.res.x + x] = vec4(C, A);
        }
    }

    // Write texture data
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        param.res.x, param.res.y, 0,
        GL_RGBA, GL_FLOAT, tex_data_.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    // Render to backbuffer
    tex_renderer_.Render(texture_);
}