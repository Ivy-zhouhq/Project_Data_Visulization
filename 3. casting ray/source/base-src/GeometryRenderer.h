#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Render.h"
#include "GL.h"
#include "GeometryRenderer.h"

class GeometryRenderer
{
public:
	GeometryRenderer();
	~GeometryRenderer();

	void Render(GLenum primitive_type, const glm::vec2* points, int size, const glm::vec4& rgba = glm::vec4(1));
	void Render(GLenum primitive_type, const glm::vec2* points, const glm::vec4* colors, int size);

	void Render(const RenderParam& param, GLenum primitive_type, const glm::vec3* points, int size, const glm::vec4& rgba = glm::vec4(1));
	void Render(const RenderParam& param, GLenum primitive_type, const glm::vec3* points, const glm::vec4* colors, int size);

	void Render(const RenderParam& param, GLenum primitive_type, GLuint vao, int size, const glm::vec4& rgba = glm::vec4(1));

private:
	GLuint shader_multi_color_;
	GLuint shader_single_color_;
	GLuint vao_2D_;
	GLuint vao_3D_;
	GLuint vbo_pos_;
	GLuint vbo_col_;
};