#pragma once
#include <vector>
#include "GL.h"
#include <glm/glm.hpp>

struct Mesh
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coord;
	};

	struct Triangle
	{
		unsigned int index[3];
	};

	Mesh() :
		vao_(0),
		vbo_(0),
		ibo_(0)
	{
		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);
		glGenBuffers(1, &ibo_);

		const char* p_offset = 0;
		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), p_offset + 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), p_offset + 12);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), p_offset + 24);

		glBindVertexArray(0);
	}

	~Mesh()
	{
		if (glIsVertexArray(vao_))
			glDeleteVertexArrays(1, &vao_);
		if (glIsBuffer(vbo_))
			glDeleteBuffers(1, &vbo_);
		if (glIsBuffer(ibo_))
			glDeleteBuffers(1, &ibo_);
	}

	void UpdateGLBuffers()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	GLuint vao() const { return vao_; }
	GLuint vbo() const { return vbo_; }
	GLuint ibo() const { return ibo_; }
private:
	GLuint vao_;
	GLuint vbo_;
	GLuint ibo_;
};