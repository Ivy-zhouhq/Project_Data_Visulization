#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <atomic>
#include "GL.h"
#include "UniformGridVolume.h"
#include "Volume.h"
#include "TextureRenderer.h"
#include "TransferFunction.h"
#include "Render.h"

class RaycasterCPU
{
public:
	RaycasterCPU();
	~RaycasterCPU();

	void Render(const RenderParam& param, const UniformGridVolume& vol, const TransferFunction& tf, const glm::vec3& size, float stepsize = 0.1f);

private:
	TextureRenderer tex_renderer_;
	std::vector<glm::vec4> tex_data_;
	GLuint texture_;
};