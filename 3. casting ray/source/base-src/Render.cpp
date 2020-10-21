#include <algorithm>
#include "Render.h"

using glm::vec3;
using glm::mat4;

// Calculate the view directions that define the view frustum
void CalcViewDirections(vec3 view_dirs[4], const RenderParam& param)
{
	const mat4 inv_view_matrix = glm::inverse(param.view_matrix);

	const float theta = param.fov * 0.5f;
	const float n = param.near_;
	const float n_half_h = n * glm::sin(theta);
	const float n_half_w = n_half_h * param.AspectRatio();

	const vec3& right	= vec3(inv_view_matrix[0]);
	const vec3& up		= vec3(inv_view_matrix[1]);
	const vec3& forward = vec3(inv_view_matrix[2]);

	view_dirs[0] = -forward * n - up * n_half_h - right * n_half_w;
	view_dirs[1] = -forward * n + up * n_half_h - right * n_half_w;
	view_dirs[2] = -forward * n - up * n_half_h + right * n_half_w;
	view_dirs[3] = -forward * n + up * n_half_h + right * n_half_w;
}