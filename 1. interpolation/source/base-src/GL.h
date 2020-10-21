#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLuint CreateShaderProgram(const char* vert_file, const char* frag_file);

GLuint CreateTexture1D(int size, GLenum internal_format = GL_RGBA8,
	GLenum data_format = GL_RGBA, GLenum data_type = GL_FLOAT, const void* data = nullptr);

GLuint CreateTexture2D(int width, int height, GLenum internal_format = GL_RGBA8,
	GLenum data_format = GL_RGBA, GLenum data_type = GL_FLOAT, const void* data = nullptr);

GLuint CreateTexture3D(int width, int height, int depth, GLenum internal_format = GL_RGBA8,
	GLenum data_format = GL_RGBA, GLenum data_type = GL_FLOAT, const void* data = nullptr);

GLFWwindow* CreateGLFWWindow(int res_x, int res_y, const char* title = "Window", int ver_major = 2, int ver_minor = 1);