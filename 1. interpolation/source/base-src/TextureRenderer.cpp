#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include "TextureRenderer.h"

using glm::vec2;

TextureRenderer::TextureRenderer()
{
    // VAO & VBO
    const float q_data[] = {
        -1, -1, 0, 1,
        -1,  1, 0, 1,
         1, -1, 0, 1,
         1,  1, 0, 1
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(q_data), q_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, 0, 0, 0);
    glBindVertexArray(0);

    shader_ = CreateShaderProgram("shaders/tex_quad.vert", "shaders/tex_quad.frag");
    if (shader_ == GL_INVALID_VALUE) {
        printf("TextureRenderer: Failed to create shader, terminating... \n");
        exit(-1);
    }
}

TextureRenderer::~TextureRenderer()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteProgram(shader_);
}

void TextureRenderer::Render(GLuint texture, const vec2& min_coord, const vec2& max_coord)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(shader_);

    GLint min_loc = glGetUniformLocation(shader_, "min_coord");
    GLint max_loc = glGetUniformLocation(shader_, "max_coord");
    GLint tex_loc = glGetUniformLocation(shader_, "texture0");
    
    glUniform2fv(min_loc, 1, glm::value_ptr(min_coord));
    glUniform2fv(max_loc, 1, glm::value_ptr(max_coord));
    glUniform1i(tex_loc, 0);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}