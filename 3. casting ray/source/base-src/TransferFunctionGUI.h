#pragma once
#include <glm/glm.hpp>
#include "Render.h"
#include "TransferFunction.h"
#include "TextureRenderer.h"
#include "GeometryRenderer.h"
#include "GUIComponent.h"
#include "ColorPickerGUI.h"

class TransferFunctionGUI : public GUIComponent
{
public:
	TransferFunctionGUI(const glm::vec2& top_left, const glm::vec2& bottom_right);
	~TransferFunctionGUI();

	void Modify(const glm::vec2& screen_res,
		const glm::vec2& mouse_pos,
		const glm::vec2& mouse_vel,
		const glm::vec2& mouse_scroll,
		bool mouse_down[8]);

	void Render(const glm::vec2& screen_res);

	void SetMinBox(const glm::vec2& min_box);
	void SetMaxBox(const glm::vec2& max_box);

	void SetTransferFunction(const TransferFunction& tf) { tf_ = tf; }
	TransferFunction& GetTransferFunction() { return tf_; }

	GLuint GetTexture() const { return tex_tf_; }

private:
	void UpdateTexture();
	int GetControlPoint(const glm::vec2& p);
	TransferFunction tf_;
	ColorPickerGUI cp_gui_;

	int drag_target_;
	int selected_point_;
	int prev_point_;

	GLuint tex_tf_;
	GLuint tex_bg_;

	TextureRenderer texture_renderer_;
	GeometryRenderer geometry_renderer_;

	bool prev_mouse_down_[8];
};