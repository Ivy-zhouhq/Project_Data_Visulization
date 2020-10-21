#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GL.h"
#include "Intersection.h"
#include "UniformGridVolume.h"
#include "UnstructuredGridVolume.h"
#include "SliceRenderer.h"
#include "TextureRenderer.h"
#include "Tetrahedron.h"

using glm::ivec2;
using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::quat;
using glm::mat3;
using glm::mat4;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool g_keyhit[GLFW_KEY_LAST];
bool g_keydown[GLFW_KEY_LAST];

int main(int argc, char* argv[])
{
    RenderParam param;
    param.res = ivec2(512);

    GLFWwindow* window = CreateGLFWWindow(param.res.x, param.res.y, "Interpolation", 3, 3);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);

    SliceRenderer slice_renderer;
    UniformGridVolume uniform_vol;
    UnstructuredGridVolume unstructured_vol;

    UniformGridVolume::LoadDat(&uniform_vol, "media/uniform/nucleon.dat");
    UnstructuredGridVolume::LoadNode(&unstructured_vol, "media/unstructured/colorcube.node");
    UnstructuredGridVolume::LoadEle(&unstructured_vol, "media/unstructured/colorcube.ele");
    UnstructuredGridVolume::LoadFace(&unstructured_vol, "media/unstructured/colorcube.face");
    unstructured_vol.CreatePlanes();
    unstructured_vol.CreateTetras();

    SliceRenderer::Dim axis = SliceRenderer::DIM_X;
    SliceRenderer::Interpolation interpolation = SliceRenderer::NEAREST;
    float slice_pos = 0.5f;
    Volume* target_vol = &uniform_vol;

    bool update_screen = true;

    // Main loop
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
    {
        if (g_keydown[GLFW_KEY_A]) {
            slice_pos += 0.01f;
            update_screen = true;
        }

        if (g_keydown[GLFW_KEY_Z]) {
            slice_pos -= 0.01f;
            update_screen = true;
        }

        if (g_keyhit[GLFW_KEY_1]) {
            axis = SliceRenderer::DIM_X;
            update_screen = true;
        }
            
        if (g_keyhit[GLFW_KEY_2]) {
            axis = SliceRenderer::DIM_Y;
            update_screen = true;
        }

        if (g_keyhit[GLFW_KEY_3]) {
            axis = SliceRenderer::DIM_Z;
            update_screen = true;
        }

        if (g_keyhit[GLFW_KEY_SPACE]) {
            if (interpolation == SliceRenderer::LINEAR)
                interpolation = SliceRenderer::NEAREST;
            else
                interpolation = SliceRenderer::LINEAR;
            update_screen = true;
        }

        if (g_keyhit[GLFW_KEY_V])
        {
            if (target_vol == &uniform_vol)
                target_vol = &unstructured_vol;
            else
                target_vol = &uniform_vol;
            update_screen = true;
        }

        slice_pos = glm::clamp(slice_pos, 0.f, 1.f);
            
        if (update_screen) {
            char title[256];
            sprintf(title, "A[%i] X[%.2f] I[%s] V[%i]",
                axis, slice_pos,
                (interpolation == SliceRenderer::LINEAR) ? "L" : "N",
                target_vol == nullptr);
            glfwSetWindowTitle(window, title);



            slice_renderer.Render(axis, slice_pos, param, target_vol, interpolation);

            // Swap buffers
            glfwSwapBuffers(window);
            update_screen = false;
        }

        // Reset keyhit
        for (auto& k : g_keyhit)
            k = false;

        // Read input and handle events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    g_keydown[key] = (action != GLFW_RELEASE);
    g_keyhit[key] = (action == GLFW_PRESS);
}