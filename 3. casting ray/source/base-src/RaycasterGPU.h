#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "GL.h"
#include "Render.h"
#include "Volume.h"

class RaycasterGPU
{
public:
	RaycasterGPU();
	~RaycasterGPU();

	GLuint GetShader() const { return shader_; }

	void Render(const RenderParam& param, GLuint tex_vol, GLuint tex_tf, const glm::vec3& size, float stepsize = 0.1f);
private:
	GLuint shader_;
	GLuint vao_;
	GLuint vbo_;
};