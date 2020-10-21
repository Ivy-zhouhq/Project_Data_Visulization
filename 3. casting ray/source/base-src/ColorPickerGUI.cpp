#include <array>
#include <glm/gtx/color_space.hpp>
#include "ColorPickerGUI.h"
#include "GL.h"

using namespace glm;

static const int RES_X = 256;
static const int RES_Y = 128;

ColorPickerGUI::ColorPickerGUI(const vec2& top_left, const vec2& bottom_right) :
	GUIComponent(top_left, bottom_right),
	point_(0.5f, 0.5f),
	value_(1.f),
	dragging_(-1),
	prev_mouse_down_(false)
{
	tex_hs_ = CreateTexture2D(RES_X, RES_Y, GL_RGBA8);
	tex_v_ = CreateTexture2D(RES_X, 1, GL_RGBA8);

	glBindTexture(GL_TEXTURE_2D, tex_hs_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, tex_v_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	UpdateTextureHS();
	UpdateTextureV();
}

ColorPickerGUI::~ColorPickerGUI()
{
	glDeleteTextures(1, &tex_hs_);
	glDeleteTextures(1, &tex_v_);
}

void ColorPickerGUI::SetColor(const vec3& rgb)
{
	vec3 hsv = hsvColor(rgb);
	point_.x = hsv.x / 360.f;
	point_.y = hsv.y;
	value_ = hsv.z;

	UpdateTextureV();
}

vec3 ColorPickerGUI::Color() const {
	vec3 hsv(point_.x * 360.f, point_.y, value_);
	return rgbColor(hsv);
}

void ColorPickerGUI::Modify(const glm::vec2& screen_res,
							const glm::vec2& mouse_pos,
							const glm::vec2& mouse_vel,
							const glm::vec2& mouse_scroll,
							bool mouse_down[8])
{
	if (!Visible())
		return;

	if (mouse_down[0]) {
		vec2 p = mouse_pos;
		vec2 min_col = GetMinBox();
		vec2 max_col = GetMinBox() + Size() * vec2(1, 0.9f);
		vec2 min_val = GetMinBox() + Size() * vec2(0, 0.9f);
		vec2 max_val = GetMaxBox();

		if (!prev_mouse_down_) {
			if (p.x > min_col.x && p.x < max_col.x &&
				p.y > min_col.y && p.y < max_col.y)
			{
				// Inside of color area
				if (dragging_ < 0)
					dragging_ = 0;
			}
			else if (p.x > min_val.x && p.x < max_val.x &&
				p.y > min_val.y && p.y < max_val.y)
			{
				// Inside of value area
				if (dragging_ < 0)
					dragging_ = 1;
			}
		}

		// If dragging inside of color space
		if (dragging_ == 0) {
			point_ = (p - min_col) / (max_col - min_col);
			point_.y = 1.f - point_.y;
			point_ = glm::clamp(point_, vec2(0), vec2(1));
			UpdateTextureV();
		} // Dragging inside of value space
		else if (dragging_ == 1) {
			value_ = (p.x - GetMinBox().x) / Size().x;
			value_ = glm::clamp(value_, 0.f, 1.f);
		}
	}
	else
		dragging_ = -1;

	interacting_ = dragging_ != -1;
	prev_mouse_down_ = mouse_down[0];
}

void ColorPickerGUI::Render(const glm::vec2& screen_res)
{
	if (!Visible())
		return;

	// Determine texture space coordinates [0,1]
	vec2 rect_min = vec2(GetMinBox().x, screen_res.y - GetMaxBox().y) / screen_res;
	vec2 rect_max = vec2(GetMaxBox().x, screen_res.y - GetMinBox().y) / screen_res;
	vec2 rect_size = rect_max - rect_min;

	// Color Picker
	vec2 col_min = rect_min + rect_size * vec2(0, 0.1f);
	vec2 col_max = rect_max;
	vec2 col_size = col_max - col_min;

	// Saturation Slider
	vec2 sat_min = rect_min;
	vec2 sat_max = rect_min + rect_size * vec2(1, 0.1f);
	vec2 sat_size = sat_max - sat_min;

	// Render Textures
	texture_renderer_.Render(tex_hs_, col_min, col_max);
	texture_renderer_.Render(tex_v_, sat_min, sat_max);

	// Render Slider
	vec2 x[4];
	float o = value_;
	x[0] = sat_min + sat_size * vec2(o - 0.005f, 0);
	x[1] = sat_min + sat_size * vec2(o - 0.005f, 1);
	x[2] = sat_min + sat_size * vec2(o + 0.005f, 0);
	x[3] = sat_min + sat_size * vec2(o + 0.005f, 1);
	geometry_renderer_.Render(GL_TRIANGLE_STRIP, x, 4);

	// Draw Point Outline
	vec2 p = col_min + point_ * col_size;
	glPointSize(7.f);
	geometry_renderer_.Render(GL_POINTS, &p, 1, vec4(0,0,0,1));

	// Draw colored point
	glPointSize(5.f);
	geometry_renderer_.Render(GL_POINTS, &p, 1, vec4(Color(), 1));
}

void ColorPickerGUI::UpdateTextureHS()
{
	std::array<vec4, RES_X * RES_Y> data;

	for (int y = 0; y < RES_Y; ++y) {
		for (int x = 0; x < RES_X; ++x) {
			float h = ((float)x / (float)(RES_X - 1)) * 360.f;
			float s = (float)y / (float)(RES_Y - 1);
			float v = value_;

			vec3 hsv(h, s, v);
			data[y * RES_X + x] = vec4(rgbColor(hsv), 1.f);
		}
	}

	glBindTexture(GL_TEXTURE_2D, tex_hs_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RES_X, RES_Y, GL_RGBA, GL_FLOAT, data.data());
}

void ColorPickerGUI::UpdateTextureV()
{
	std::array<vec4, RES_X> data;

	vec3 hsv(point_.x * 360.f, point_.y, value_);
	for (int i = 0; i < RES_X; ++i) {
		hsv.z = i / (float)(RES_X - 1);
		data[i] = vec4(rgbColor(hsv), 1);
	}

	glBindTexture(GL_TEXTURE_2D, tex_v_);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RES_X, 1, GL_RGBA, GL_FLOAT, data.data());
}