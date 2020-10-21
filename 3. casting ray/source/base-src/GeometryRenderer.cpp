#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GeometryRenderer.h"

GeometryRenderer::GeometryRenderer()
{
	glGenVertexArrays(1, &vao_2D_);
	glGenVertexArrays(1, &vao_3D_);
	glGenBuffers(1, &vbo_pos_);
	glGenBuffers(1, &vbo_col_);

	glBindVertexArray(vao_2D_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_col_);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(vao_3D_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_col_);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	shader_multi_color_ = CreateShaderProgram("shaders/geom_multi_color.vert", "shaders/geom_multi_color.frag");
	shader_single_color_ = CreateShaderProgram("shaders/geom_single_color.vert", "shaders/geom_single_color.frag");
}

GeometryRenderer::~GeometryRenderer()
{
	glDeleteVertexArrays(1, &vao_2D_);
	glDeleteVertexArrays(1, &vao_3D_);
	glDeleteBuffers(1, &vbo_pos_);
	glDeleteBuffers(1, &vbo_col_);
	glDeleteProgram(shader_multi_color_);
	glDeleteProgram(shader_single_color_);
}

void GeometryRenderer::Render(GLenum primitive_type,
							  const glm::vec2* points,
							  int size,
							  const glm::vec4& rgba)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec2), points, GL_DYNAMIC_DRAW);

	GLuint shader = shader_single_color_;

	glUseProgram(shader);

	GLint view_loc = glGetUniformLocation(shader, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader, "proj_matrix");
	GLint color_loc = glGetUniformLocation(shader, "color");

	glm::mat4 T = glm::translate(glm::mat4(), glm::vec3(-1.f, -1.f, 0.f));
	glm::mat4 S = glm::scale(glm::mat4(), glm::vec3(2.f, 2.f, 1.f));

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(T * S));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
	glUniform4fv(color_loc, 1, glm::value_ptr(rgba));

	glBindVertexArray(vao_2D_);
	glDrawArrays(primitive_type, 0, size);
	glBindVertexArray(0);

	glUseProgram(0);
}

void GeometryRenderer::Render(GLenum primitive_type,
	const glm::vec2* points,
	const glm::vec4* colors,
	int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec2), points, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_col_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec4), colors, GL_DYNAMIC_DRAW);

	GLuint shader = shader_multi_color_;

	glUseProgram(shader);

	GLint view_loc = glGetUniformLocation(shader, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader, "proj_matrix");

	glm::mat4 T = glm::translate(glm::mat4(), glm::vec3(-1.f, -1.f, 0.f));
	glm::mat4 S = glm::scale(glm::mat4(), glm::vec3(2.f, 2.f, 1.f));

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(T * S));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	glBindVertexArray(vao_2D_);
	glDrawArrays(primitive_type, 0, size);
	glBindVertexArray(0);

	glUseProgram(0);
}


void GeometryRenderer::Render(const RenderParam& param,
							  GLenum primitive_type,
							  const glm::vec3* points,
							  int size,
							  const glm::vec4& rgba)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), points, GL_DYNAMIC_DRAW);

	GLuint shader = shader_single_color_;

	glUseProgram(shader);

	GLint view_loc = glGetUniformLocation(shader, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader, "proj_matrix");
	GLint color_loc = glGetUniformLocation(shader, "color");

	glm::mat4 view_matrix = param.view_matrix * param.model_matrix;
	glm::mat4 proj_matrix = glm::perspective(param.fov, param.AspectRatio(), param.near_, param.far_);

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
	glUniform4fv(color_loc, 1, glm::value_ptr(rgba));

	glBindVertexArray(vao_3D_);
	glDrawArrays(primitive_type, 0, size);
	glBindVertexArray(0);

	glUseProgram(0);
}

void GeometryRenderer::Render(const RenderParam& param,
	GLenum primitive_type,
	const glm::vec3* points,
	const glm::vec4* colors,
	int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec3), points, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_col_);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(glm::vec4), colors, GL_DYNAMIC_DRAW);

	GLuint shader = shader_multi_color_;

	glUseProgram(shader);

	GLint view_loc = glGetUniformLocation(shader, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader, "proj_matrix");

	glm::mat4 view_matrix = param.view_matrix * param.model_matrix;
	glm::mat4 proj_matrix = glm::perspective(param.fov, param.AspectRatio(), param.near_, param.far_);

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));

	glBindVertexArray(vao_3D_);
	glDrawArrays(primitive_type, 0, size);
	glBindVertexArray(0);

	glUseProgram(0);
}

void GeometryRenderer::Render(const RenderParam& param,
	GLenum primitive_type,
	GLuint vao,
	int size,
	const glm::vec4& rgba)
{
	glUseProgram(shader_single_color_);

	GLint view_loc = glGetUniformLocation(shader_single_color_, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader_single_color_, "proj_matrix");
	GLint color_loc = glGetUniformLocation(shader_single_color_, "color");

	glm::mat4 view_matrix = param.view_matrix * param.model_matrix;
	glm::mat4 proj_matrix = glm::perspective(param.fov, param.AspectRatio(), param.near_, param.far_);

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
	glUniform4fv(color_loc, 1, glm::value_ptr(rgba));

	glBindVertexArray(vao);
	glDrawArrays(primitive_type, 0, size);
	glBindVertexArray(0);

	glUseProgram(0);
}