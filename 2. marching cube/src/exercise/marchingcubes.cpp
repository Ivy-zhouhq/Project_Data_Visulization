#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GL.h"
#include "Intersection.h"
#include "UniformGridVolume.h"
#include "MeshRenderer.h"
#include "Triangulate.h"
#include "Transform.h"

using glm::ivec2;
using glm::ivec3;
using glm::vec2;
using glm::vec3;
using glm::quat;
using glm::mat3;
using glm::mat4;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseButtonCallback(GLFWwindow*, int, int, int);
void CursorPosCallback(GLFWwindow*, double, double);
void ScrollCallback(GLFWwindow*, double, double);

void SetupRenderParam(RenderParam* param);

bool g_keyhit[GLFW_KEY_LAST];
bool g_keydown[GLFW_KEY_LAST];
bool g_mousedown[16];
vec2 g_mousepos;
vec2 g_mousevel;
vec2 g_mousescroll;

int main(int argc, char* argv[])
{
    // Setupm static render param
    RenderParam param;
    param.res = ivec2(1024);
    param.fov = glm::radians(60.f);
    param.near_ = 0.1f;
    param.far_ = 100.f;

    // Create window and set callbacks for input
    GLFWwindow* window = CreateGLFWWindow(param.res.x, param.res.y, "Marching Cubes", 3, 3);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Create renderer
    MeshRenderer renderer;

    // Create and load Volume
    UniformGridVolume uniform_vol;
    UniformGridVolume::LoadDat(&uniform_vol, "media/uniform/nucleon.dat");

    // Resample Volume
    UniformGridVolume resampled_vol;
    UniformGridVolume::Resample(&resampled_vol, uniform_vol, ivec3(64));

    // Create mesh
    Mesh mesh;

    // Triangulate volume
    printf("Triangulating volume... ");
    Triangulate(&mesh, uniform_vol, 0.5f);
    printf("Done! %lu Triangles were created.\n", mesh.vertices.size());

    // Update OpenGL Buffer Data
    mesh.UpdateGLBuffers();

    // Main loop
    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        // Disable backface culling
        glDisable(GL_CULL_FACE);
        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Clear the drawbuffers
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        // Setup
        SetupRenderParam(&param);
        // Render
        renderer.Render(mesh, param);

        // Swap buffers
        glfwSwapBuffers(window);

        // Reset input
        g_mousevel = vec2(0);
        g_mousescroll = vec2(0);
        for (auto& k : g_keyhit)
            k = false;

        // Read input and handle events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void SetupRenderParam(RenderParam* param)
{
    // Constants for pi
    const float pi = glm::pi<float>();
    const float two_pi = 2.f * pi;

    // Variables for Spherical Coordinates
    static float theta = 0.f, phi = 0.f, r = 10.f;

    // Change Radius
    r -= g_mousescroll.y * 0.2f;
    
    // Change angles
    if (g_mousedown[0]) {
        phi -= g_mousevel.x * 0.0025f;
        theta -= g_mousevel.y * 0.0025f;
    }

    // Clamp variables
    r = glm::clamp(r, 0.1f, 5.f);
    theta = glm::clamp(theta, 0.01f * pi, 0.99f * pi);
    if (phi > 2.f * pi) phi -= two_pi;
    if (phi < 0.f) phi += two_pi;

    // Sphereical to Cartesian
    vec3 pos(r * sinf(theta) * sinf(phi), r * cosf(theta), r * sinf(theta) * cosf(phi));

    // Create a transform that looks at (0,0,0) from pos
    Transform t = Transform::LookAt(pos, vec3(0));

    // Setup view_matrix from transform
    param->view_matrix = t.InvMatrix();
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

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    g_mousedown[button] = (action == GLFW_PRESS);
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    static double old_x = x, old_y = y;
    g_mousepos = vec2(x, y);
    g_mousevel = vec2(x-old_x, y-old_y);
    old_x = x;
    old_y = y;
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    g_mousescroll = vec2(x, y);
}