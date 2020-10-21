#include <glm/glm.hpp>
#include <array>
#include <vector>
#include "TransferFunctionGUI.h"
#include "GL.h"

using namespace glm;

const int PADDING = 10;
const int CP_GUI_WIDTH = 128;
const int CP_GUI_HEIGHT = 128;
const int TEX_RES = 512;

TransferFunctionGUI::TransferFunctionGUI(const glm::vec2& top_left, const glm::vec2& bottom_right) :
	GUIComponent(top_left, bottom_right),
	cp_gui_(top_left - vec2(0, CP_GUI_HEIGHT + PADDING), top_left - vec2(0, CP_GUI_HEIGHT + PADDING) + vec2(CP_GUI_WIDTH, CP_GUI_HEIGHT)),
	drag_target_(-1),
	selected_point_(-1),
	prev_point_(-1)
{
	// Create default control points for TF
	ControlPoint p0, p1;
	p0.SetColor(vec3(0));
	p0.SetPos(vec2(0));
	p1.SetColor(vec3(1));
	p1.SetPos(vec2(1));

	tf_.InsertControlPoint(p0);
	tf_.InsertControlPoint(p1);

	// Create texture and update it
	tex_tf_ = CreateTexture1D(TEX_RES, GL_RGBA8);
	UpdateTexture();

	// Create BG texture
	std::vector<vec4> tex_data(TEX_RES * (TEX_RES / 2), vec4(0.2f));
	tex_bg_ = CreateTexture2D(TEX_RES, TEX_RES / 2, GL_RGBA8, GL_RGBA, GL_FLOAT, tex_data.data());
}


TransferFunctionGUI::~TransferFunctionGUI()
{
	glDeleteTextures(1, &tex_tf_);
}

void TransferFunctionGUI::Modify(const glm::vec2& screen_res,
								 const glm::vec2& mouse_pos,
								 const glm::vec2& mouse_vel,
								 const glm::vec2& mouse_scroll,
								 bool mouse_down[8])
{
	if (!Visible())
		return;

	// Pass modify to color picker
	cp_gui_.Modify(screen_res, mouse_pos, mouse_vel, mouse_scroll, mouse_down);

	// Calculate local pos
	vec2 lp = ScreenToLocal(mouse_pos);
	lp.y = 1.f - lp.y;
	lp = glm::clamp(lp, 0.f, 1.f);

	// Handle left mouse
	if (mouse_down[0])
	{
		if (!cp_gui_.Interacting())
			selected_point_ = -1;

		if (!prev_mouse_down_[0] && !cp_gui_.Interacting()) {
			int i = GetControlPoint(mouse_pos);

			if (i == -1 && Overlap(mouse_pos)) {
				ControlPoint p;
				p.SetPos(lp);
				p.SetColor(vec3(tf_.RGBA(p.val)));
				tf_.InsertControlPoint(p);
				i = GetControlPoint(mouse_pos);
			}

			drag_target_ = i;
			selected_point_ = i;
		}
	}
	// Handle right mouse
	else if (mouse_down[1]) {
		if (!prev_mouse_down_[0] && Overlap(mouse_pos)) {
			// Try to delete point
			int i = GetControlPoint(mouse_pos);
			if (i > -1) {
				tf_.RemoveControlPoint(i);
				selected_point_ = -1;
			}
		}
	}
	else
		drag_target_ = -1;

	// User is dragging a point
	if (drag_target_ > -1) {
		drag_target_ = tf_.SetPosition(drag_target_, lp);
		selected_point_ = drag_target_;

		vec3 color = vec3(tf_.points()[selected_point_].rgba);
		cp_gui_.SetColor(color);
	}

	if (selected_point_ > -1) {
		vec4 rgba = tf_.points()[selected_point_].rgba;
		tf_.points()[selected_point_].rgba = vec4(cp_gui_.Color(), rgba.w);
		cp_gui_.SetVisisble(true);
		UpdateTexture();
	}
	else
		cp_gui_.SetVisisble(false);

	interacting_ = (drag_target_ > -1) || cp_gui_.Interacting();

	// If the user is interacting, update texture data
	if (interacting_)
		UpdateTexture();



	prev_point_ = selected_point_;

	for (int i = 0; i < 8; ++i)
		prev_mouse_down_[i] = mouse_down[i];
}

void TransferFunctionGUI::Render(const glm::vec2& screen_res)
{
	if (!Visible())
		return;

	cp_gui_.Render(screen_res);

	// Calculate bounds
	vec2 rect_min = vec2(GetMinBox().x, screen_res.y - GetMaxBox().y) / screen_res;
	vec2 rect_max = vec2(GetMaxBox().x, screen_res.y - GetMinBox().y) / screen_res;
	vec2 rect_size = rect_max - rect_min;

	// Render background
	texture_renderer_.Render(tex_bg_, rect_min, rect_max);

	// If no points, then draw a ramp
	int num_points = (int)tf_.points().size();
	if (num_points == 0) {
		vec2 p[2] = { rect_min, rect_max };
		geometry_renderer_.Render(GL_LINE_STRIP, p, 2, vec4(1));
		return;
	}

	std::vector<vec2> points(num_points);
	std::vector<vec4> colors(num_points);
	std::vector<vec2> line_points(num_points+2);
	
	// Prepare point, color and line data
	for (int i = 0; i < num_points; ++i) {
		vec4 rgba = tf_.points()[i].rgba;
		float val = tf_.points()[i].val;

		vec2 pos = rect_min + vec2(val, rgba.w) * rect_size;
		vec4 col = vec4(vec3(rgba), 1);

		points[i] = pos;
		colors[i] = col;
		line_points[i + 1] = pos;
	}

	line_points.front() = line_points[1];
	line_points.front().x = rect_min.x;

	line_points.back() = line_points[line_points.size() - 2];
	line_points.back().x = rect_max.x;

	// Draw lines
	geometry_renderer_.Render(GL_LINE_STRIP, line_points.data(), line_points.size(), vec4(1));

	// Draw selected point frame
	if (selected_point_ > -1) {
		glPointSize(9.f);
		vec2 p = rect_min + tf_.points()[selected_point_].GetPos() * rect_size;
		geometry_renderer_.Render(GL_POINTS, &p, 1, vec4(1));
	}

	// Draw point frame
	glPointSize(7.f);
	geometry_renderer_.Render(GL_POINTS, points.data(), num_points, vec4(1));
	
	// Draw colored points
	glPointSize(5.f);
	geometry_renderer_.Render(GL_POINTS, points.data(), colors.data(), num_points);
}

void TransferFunctionGUI::SetMinBox(const vec2& min_box)
{

	GUIComponent::SetMinBox(min_box);
	vec2 cp_min(min_box - vec2(0, CP_GUI_HEIGHT + PADDING));
	cp_gui_.SetMinBox(cp_min);
}

void TransferFunctionGUI::SetMaxBox(const vec2& max_box)
{
	GUIComponent::SetMaxBox(max_box);
	vec2 cp_max(GetMinBox() - vec2(0, CP_GUI_HEIGHT + PADDING) + vec2(CP_GUI_WIDTH, CP_GUI_HEIGHT));
	cp_gui_.SetMaxBox(cp_max);
}

void TransferFunctionGUI::UpdateTexture()
{
	std::array<vec4, TEX_RES> tex_data;
	tf_.CreateTextureData(tex_data.data(), TEX_RES);
	glBindTexture(GL_TEXTURE_1D, tex_tf_);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, TEX_RES, GL_RGBA, GL_FLOAT, tex_data.data());
	glBindTexture(GL_TEXTURE_1D, 0);
}

// Gets an index to a control point if found within radii of mouse_pos
int TransferFunctionGUI::GetControlPoint(const vec2& mouse_pos)
{
	const float SEL_RAD2 = 6 * 6;

	for (int i = 0; i < (int)tf_.points().size(); ++i) {
		vec2 p = tf_.points()[i].GetPos();
		p.y = 1.f - p.y;
		vec2 pos = LocalToScreen(p);
		vec2 d = mouse_pos - pos;
		float dist2 = d.x * d.x + d.y * d.y;
		if (dist2 < SEL_RAD2)
			return i;
	}
	return -1;
}