#pragma once
#include <glm/glm.hpp>
#include "GUIComponent.h"
#include "TextureRenderer.h"
#include "GeometryRenderer.h"
#include "Render.h"

class ColorPickerGUI : public GUIComponent
{
public:
	ColorPickerGUI(const glm::vec2& top_left, const glm::vec2& bottom_right);
	~ColorPickerGUI();

	void SetColor(const glm::vec3& rgb);
	glm::vec3 Color() const;

	void Modify(const glm::vec2& screen_res,
		const glm::vec2& mouse_pos,
		const glm::vec2& mouse_vel,
		const glm::vec2& mouse_scroll,
		bool mouse_down[8]);
	void Render(const glm::vec2& screen_res);

private:
	void UpdateTextureHS();
	void UpdateTextureV();

	glm::vec2 point_;
	float value_;

	int dragging_;

	bool prev_mouse_down_;

	TextureRenderer texture_renderer_;
	GeometryRenderer geometry_renderer_;
	GLuint tex_hs_;
	GLuint tex_v_;
};