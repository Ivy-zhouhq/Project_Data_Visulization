#pragma once
#include <vector>
#include "GL.h"
#include "Render.h"
#include "Volume.h"
#include "TextureRenderer.h"

class SliceRenderer
{
public:
	enum Dim
	{
		DIM_X = 0,
		DIM_Y = 1,
		DIM_Z = 2
	};

	enum Interpolation
	{
		NEAREST = 0,
		LINEAR = 1
	};

	SliceRenderer();
	~SliceRenderer();

	void Render(Dim dim, float val, const RenderParam& param, const Volume* vol, Interpolation ip = NEAREST);

private:
	TextureRenderer tex_renderer_;
	std::vector<glm::vec4> tex_data_;
	GLuint texture_id_;
};