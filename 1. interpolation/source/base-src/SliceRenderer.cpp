#include <algorithm>
#include "SliceRenderer.h"

using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::vec4;

SliceRenderer::SliceRenderer()
{
	glGenTextures(1, &texture_id_);

	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

SliceRenderer::~SliceRenderer()
{
	glDeleteTextures(1, &texture_id_);
}

void SliceRenderer::Render(Dim dim, float val, const RenderParam& param, const Volume* vol, Interpolation ip)
{
	// Possibly resize texture data buffer
	int req_size = param.res.x * param.res.y;
	if (req_size != tex_data_.size())
		tex_data_.resize(req_size);

	// Update texture data
	for (int y = 0; y < param.res.y; ++y) {
		for (int x = 0; x < param.res.x; ++x) {
			vec2 screen_coord = vec2(x, y) / vec2(param.res - 1);
			vec3 vol_coord(screen_coord.x, screen_coord.y, val);
			std::swap(vol_coord[dim], vol_coord[2]);

			int id = y * param.res.x + x;
			if (ip == NEAREST)
				tex_data_[id] = vol->GetNearest(vol_coord);
			else if (ip == LINEAR)
				tex_data_[id] = vol->GetLinear(vol_coord);
		}
	}

	// Write texture data
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
		param.res.x, param.res.y, 0,
		GL_RGBA, GL_FLOAT, tex_data_.data());
	glBindTexture(GL_TEXTURE_2D, 0);

	// Render to backbuffer
	tex_renderer_.Render(texture_id_);
}