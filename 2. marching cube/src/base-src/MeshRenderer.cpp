#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MeshRenderer.h"

using glm::vec2;
using glm::mat4;

MeshRenderer::MeshRenderer()
{
    shader_ = CreateShaderProgram("shaders/mesh.vert", "shaders/mesh.frag");
	if (shader_ == GL_INVALID_VALUE) {
		printf("TextureRenderer: Failed to create shader, terminating... \n");
		exit(-1);
	}
}

MeshRenderer::~MeshRenderer()
{
	glDeleteProgram(shader_);
}

void MeshRenderer::Render(const Mesh& mesh, const RenderParam& param)
{
	mat4 mv_matrix = param.view_matrix * param.model_matrix;
	mat4 proj_matrix = glm::perspective(param.fov, param.AspectRatio(), param.near_, param.far_);

	glUseProgram(shader_);

	GLint view_loc = glGetUniformLocation(shader_, "view_matrix");
	GLint proj_loc = glGetUniformLocation(shader_, "proj_matrix");

	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(mv_matrix));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));

	glBindVertexArray(mesh.vao());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo());
	glDrawElements(GL_TRIANGLES, mesh.triangles.size() * 3, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
	glBindVertexArray(0);

	glUseProgram(0);
}