#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RaycasterGPU.h"
#include "Render.h"
#include <cstdio>

RaycasterGPU::RaycasterGPU()
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

    shader_ = CreateShaderProgram("shaders/raycast.vert", "shaders/raycast.frag");
    if (shader_ == GL_INVALID_VALUE) {
        printf("RaycasterGPU: Failed to create shader, terminating... \n");
        exit(-1);
    }
}

RaycasterGPU::~RaycasterGPU()
{
    glDeleteProgram(shader_);
}

void RaycasterGPU::Render(const RenderParam& param, GLuint tex_vol, GLuint tex_tf, const glm::vec3& size, float stepsize)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tex_vol);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, tex_tf);

    glUseProgram(shader_);

    glm::vec3 view_dir[4];
    CalcViewDirections(view_dir, param);

    glm::vec3 view_pos = glm::vec3(glm::inverse(param.view_matrix) * glm::vec4(0, 0, 0, 1));

    GLint tex_vol_loc	= glGetUniformLocation(shader_, "tex_vol");
    GLint tex_tf_loc	= glGetUniformLocation(shader_, "tex_tf");
    GLint view_dir_loc	= glGetUniformLocation(shader_, "view_dir");
    GLint view_pos_loc	= glGetUniformLocation(shader_, "view_pos");
    GLint size_loc		= glGetUniformLocation(shader_, "size");
    GLint stepsize_loc	= glGetUniformLocation(shader_, "stepsize");

    glUniform1i(tex_vol_loc, 0);
    glUniform1i(tex_tf_loc, 1);
    glUniform3fv(view_dir_loc, 4, glm::value_ptr(view_dir[0]));
    glUniform3fv(view_pos_loc, 1, glm::value_ptr(view_pos));
    glUniform3fv(size_loc, 1, glm::value_ptr(size));
    glUniform1f(stepsize_loc, stepsize);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_3D, 0);
    glBindTexture(GL_TEXTURE_1D, 0);
}