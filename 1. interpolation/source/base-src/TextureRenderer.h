#pragma once
#include <glm/glm.hpp>
#include "GL.h"

class TextureRenderer
{
public:
	TextureRenderer();
	~TextureRenderer();

	void Render(GLuint texture,
				const glm::vec2& min_coord = glm::vec2(0),
				const glm::vec2& max_coord = glm::vec2(1));
private:
	GLuint shader_;
	GLuint vao_;
	GLuint vbo_;
};