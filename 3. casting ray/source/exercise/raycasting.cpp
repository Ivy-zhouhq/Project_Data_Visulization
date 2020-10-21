#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL.h"
#include "Render.h"
#include "UniformGridVolume.h"
#include "TransferFunctionGUI.h"
#include "RaycasterGPU.h"
#include "RaycasterCPU.h"
#include "Transform.h"

#undef min
#undef max

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ivec2;
using glm::ivec3;

using glm::mat4;

enum class RenderMode
{
    RENDER_CPU = 0,
    RENDER_GPU = 1
};

void SetupRenderParam(RenderParam* param);
void SetupWindowTitle(GLFWwindow* window, RenderMode rm, float stepsize, float lod);

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseButtonCallback(GLFWwindow*, int, int, int);
void CursorPosCallback(GLFWwindow*, double, double);
void ScrollCallback(GLFWwindow*, double, double);

bool g_keyhit[GLFW_KEY_LAST];
bool g_keydown[GLFW_KEY_LAST];
bool g_mousedown[8];
vec2 g_mousepos;
vec2 g_mousevel;
vec2 g_mousescroll;

int main(int argc, char* argv[])
{
    // Setup static render param
    RenderParam param;
    param.res = ivec2(1280, 720);
    param.fov = glm::radians(60.f);
    param.near_ = 0.1f;
    param.far_ = 100.f;

    // Create window and set callbacks for input
    GLFWwindow* window = CreateGLFWWindow(param.res.x, param.res.y, "Ray Casting", 3, 3);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Some constants for setting up gui
    const int PADDING = 10;
    const int TF_GUI_WIDTH = 256;
    const int TF_GUI_HEIGHT = 128;

    // Transfer function gui
    vec2 tf_min = vec2(PADDING, param.res.y - TF_GUI_HEIGHT - PADDING);
    vec2 tf_max = tf_min + vec2(TF_GUI_WIDTH, TF_GUI_HEIGHT);
    TransferFunctionGUI tf_gui(tf_min, tf_max);

    // Raycaster renderer objects
    RaycasterGPU rc_gpu;
    RaycasterCPU rc_cpu;

    // Volume
    UniformGridVolume vol;
    UniformGridVolume::LoadDat(&vol, "media/uniform/foot.dat");
    ivec3 res = vol.GetRes();
    vec3 scale = vol.GetNormalizedDim();
    int max_res = glm::max(res.x, glm::max(res.y, res.z));

    // Textures
    GLuint tex_vol = 0;
    glGenTextures(1, &tex_vol);
    GLuint tex_tf = tf_gui.GetTexture();

    glBindTexture(GL_TEXTURE_3D, tex_vol);
    int levels = glm::max(1, (int)glm::log2((float)max_res) + 1);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, levels);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, res.x, res.y, res.z, 0, GL_RED, GL_FLOAT, vol.GetData());
    glGenerateMipmap(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, 0);

    // Stepsize
    // TODO: Calculate a value for stepsize that holds true to the Nyquist Criteria (Sr >= 2*Fs)
    // The rendered size of the box is normalized, so the maximum length along any dimension is 1
    // The variable res holds the resolution of the current model.
    float stepsize = 0.5f/max_res;

    // Scaler for stepsize, this is changed with A and Z
    float stepscale = 5.f;

    // Level of detail for volume lookup
    float lod = 0.f;

    // Render mode currently used
    RenderMode mode = RenderMode::RENDER_GPU;

    // Main loop
    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        // Update title
        SetupWindowTitle(window, mode, stepscale, lod);

        // Update window size
        glfwGetWindowSize(window, &param.res.x, &param.res.y);

        // Set min and max box for GUI (needed when resizing window)
        tf_min = vec2(PADDING, param.res.y - TF_GUI_HEIGHT - PADDING);
        tf_max = tf_min + vec2(TF_GUI_WIDTH, TF_GUI_HEIGHT);
        tf_gui.SetMinBox(tf_min);
        tf_gui.SetMaxBox(tf_max);

        // Modify gui given input
        tf_gui.Modify(param.res, g_mousepos, g_mousevel, g_mousescroll, g_mousedown);

        // Swap between modes
        if (g_keydown[GLFW_KEY_1])
            mode = RenderMode::RENDER_GPU;
        if (g_keydown[GLFW_KEY_2])
            mode = RenderMode::RENDER_CPU;

        if (g_keyhit[GLFW_KEY_A])
            ++stepscale;
        if (g_keyhit[GLFW_KEY_Z])
            --stepscale;
        stepscale = glm::clamp(stepscale, 1.f, 10.f);

        if (g_keyhit[GLFW_KEY_UP])
            lod += 0.25f;
        if (g_keyhit[GLFW_KEY_DOWN])
            lod -= 0.25f;
        lod = glm::clamp(lod, 0.f, 5.f);

        glUseProgram(rc_gpu.GetShader());
        int loc = glGetUniformLocation(rc_gpu.GetShader(), "lod");
        glUniform1f(loc, lod);
        glUseProgram(0);

        // Only change viewpoint when not interacting with gui component
        if (!tf_gui.Interacting())
            SetupRenderParam(&param);

        // Clear the drawbuffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        float dt = stepsize * stepscale;

        // Render
        if (mode == RenderMode::RENDER_CPU)
            rc_cpu.Render(param, vol, tf_gui.GetTransferFunction(), scale, dt);
        else if (mode == RenderMode::RENDER_GPU)
            rc_gpu.Render(param, tex_vol, tex_tf, scale, dt);

        // Render transfer function
        tf_gui.Render(param.res);

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
    static float theta = 0.25f * pi, phi = -0.75f * pi, r = 1.5f;

    // Change Radius
    r -= g_mousescroll.y * 0.2f;

    // Change angles
    if (g_mousedown[0]) {
        phi -= g_mousevel.x * 0.0025f;
        theta -= g_mousevel.y * 0.0025f;
    }

    // Clamp variables
    r = glm::clamp(r, 0.1f, 3.f);
    theta = glm::clamp(theta, 0.01f * pi, 0.99f * pi);
    if (phi > 2.f * pi) phi -= two_pi;
    if (phi < 0.f) phi += two_pi;

    // Spherical to Cartesian
    // Y up
    //vec3 pos(r * sinf(theta) * sinf(phi), r * cosf(theta), r * sinf(theta) * cosf(phi));
    // Z up
    vec3 pos(r * sinf(theta) * cosf(phi), r * sinf(theta) * sinf(phi), r * cosf(theta));

    // Create a transform that looks at (0,0,0) from pos
    Transform t = Transform::LookAt(pos, vec3(0), vec3(0, 0, 1));

    // Setup view_matrix from transform
    param->view_matrix = t.InvMatrix();
}

void SetupWindowTitle(GLFWwindow* window, RenderMode rm, float stepscale, float lod)
{
    static double t0 = 0.f;
    static int frame = 0;

    double t = glfwGetTime();
    double dt = t - t0;
    if (dt > 0.5) {
        double fps = frame / dt;
        frame = 0;
        t0 = t;
        const char* mode = (rm == RenderMode::RENDER_CPU) ? "CPU" : "GPU";
        char title[256];
        sprintf(title, "Raycaster %s, stepscale: %.2f, lod %.2f, fps: %.1f", mode, stepscale, lod, fps);
        glfwSetWindowTitle(window, title);
    }

    frame++;
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
    g_mousevel = vec2(x - old_x, y - old_y);
    old_x = x;
    old_y = y;
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    g_mousescroll = vec2(x, y);
}