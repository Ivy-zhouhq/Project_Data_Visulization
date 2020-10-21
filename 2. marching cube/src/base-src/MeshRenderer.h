#pragma once
#include <glm/glm.hpp>
#include "GL.h"
#include "Mesh.h"
#include "Render.h"

class MeshRenderer
{
public:
	MeshRenderer();
	~MeshRenderer();

	void Render(const Mesh& m, const RenderParam& param);
private:
	GLuint shader_;
};