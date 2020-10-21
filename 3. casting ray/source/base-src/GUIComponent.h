#pragma once
#include <glm/glm.hpp>

class GUIComponent
{
public:
	GUIComponent(const glm::vec2& top_left = glm::vec2(0),
				 const glm::vec2& bottom_right = glm::vec2(0)) :
		interacting_(false),
		min_box_(top_left),
		max_box_(bottom_right),
		visible_(true)
	{}

	virtual void Modify(const glm::vec2& screen_res,
		const glm::vec2& mouse_pos,
		const glm::vec2& mouse_vel,
		const glm::vec2& mouse_scroll,
		bool mouse_down[8]) {}

	virtual void Render(const glm::vec2& screen_res) {}

	void SetMinBox(const glm::vec2& top_left)		{ min_box_ = top_left; }
	void SetMaxBox(const glm::vec2& bottom_right)	{ max_box_ = bottom_right; }
	void SetVisisble(bool val)						{ visible_ = val; }

	const glm::vec2& GetMinBox() const { return min_box_; }
	const glm::vec2& GetMaxBox() const { return max_box_; }
	bool Visible() const { return visible_; }
	bool Interacting() const { return interacting_; }

	void Move(const glm::vec2& m)
	{
		min_box_ += m;
		max_box_ += m;
	}

	glm::vec2 Size() const { return max_box_ - min_box_; }

	glm::vec2 ScreenToLocal(const glm::vec2& screen_coord) const
	{
		return (screen_coord - min_box_) / Size();
	}

	glm::vec2 LocalToScreen(const glm::vec2& local_coord) const
	{
		return local_coord * Size() + min_box_;
	}

	bool Overlap(const glm::vec2& point) const
	{
		if (point.x < min_box_.x || point.x > max_box_.x) return false;
		if (point.y < min_box_.y || point.y > max_box_.y) return false;
		return true;
	}
protected:
	bool interacting_;

private:
	glm::vec2 min_box_;
	glm::vec2 max_box_;

	bool visible_;
	
};