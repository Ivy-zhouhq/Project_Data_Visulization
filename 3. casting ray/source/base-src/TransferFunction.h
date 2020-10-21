#pragma once

#include <vector>
#include <glm/glm.hpp>

struct ControlPoint
{
	float val;
	glm::vec4 rgba;

	void SetPos(const glm::vec2& p)
	{
		val = p.x;
		rgba.w = p.y;
	}

	void SetColor(const glm::vec3& c)
	{
		rgba = glm::vec4(c, rgba.w);
	}

	glm::vec2 GetPos() const { return glm::vec2(val, rgba.w); }
	glm::vec3 GetColor() const { return glm::vec3(rgba); }

	bool operator == (const ControlPoint& other)
	{
		return (this->val == other.val) && (this->rgba == other.rgba);
	}
};

class TransferFunction
{
public:
	void InsertControlPoint(const ControlPoint& p);
	void RemoveControlPoint(int index);
	void Clear() { points_.clear(); }

	int SetPosition(int point_idx, const glm::vec2& pos);
	void SetColor(int point_idx, const glm::vec4& color);

	// Returns the interpolated value for the transfer function
	glm::vec4 RGBA(float val) const;

	// Generates interpolated texture data with a specified resolution
	void CreateTextureData(glm::vec4* data, int resolution) const;

	std::vector<ControlPoint>& points() { return points_; }
private:
	std::vector<ControlPoint> points_;
};