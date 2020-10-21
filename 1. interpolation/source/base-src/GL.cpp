#include <cstdio>
#include <cstdlib>
#include "GL.h"


void glfwErrorCallback(int error, const char* description)
{
    printf("An GLFW error occurred (%d): %s\n", error, description);
}

bool ValidateShader(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            infoLog = (char *)malloc(infologLength);
            glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
        return false;
    }
    return true;
}

bool ValidateProgram(GLuint program)
{
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        int infologLength = 0;
        int charsWritten = 0;
        char *infoLog;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
        if (infologLength > 0) {
            infoLog = (char *)malloc(infologLength);
            glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
            printf("%s\n", infoLog);
            free(infoLog);
        }
        return false;
    }
    return true;
}

// Lighthouse OpenGL
char *TextFileRead(const char *fn) {
    FILE *fp;
    char *content = NULL;
    size_t count = 0;

    if (fn != NULL) {
        fp = fopen(fn, "rt");
        if (fp != NULL) {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

GLuint CreateShaderProgram(const char* vs_file, const char* fs_file)
{
    // SHADER 
    GLuint v, f;
    char *vs, *fs;

    // Create shader handlers
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);

    // Read source code from files
    vs = TextFileRead(vs_file);
    fs = TextFileRead(fs_file);

    if (vs == NULL) {
        printf("Vertex Shader (%s) could not be read. \n", vs_file);
        return GL_INVALID_VALUE;
    }

    if (fs == NULL) {
        printf("Fragment Shader (%s) could not be read. \n", fs_file);
        return GL_INVALID_VALUE;
    }

    const char * c_vs = vs;
    const char * c_fs = fs;

    // Set shader source
    glShaderSource(v, 1, &c_vs, NULL);
    glShaderSource(f, 1, &c_fs, NULL);

    free(vs); free(fs);

    // Compile all shaders
    glCompileShader(v);
    glCompileShader(f);

    ValidateShader(v);
    ValidateShader(f);

    // Create the program
    GLuint p = glCreateProgram();

    // Attach shaders to program
    glAttachShader(p, v);
    glAttachShader(p, f);

    // Link and set program to use
    glLinkProgram(p);
    ValidateProgram(p);

    return p;
}

GLFWwindow* CreateGLFWWindow(int res_x, int res_y, const char* title, int ver_major, int ver_minor)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return nullptr;
    }

    // Set up hints for context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver_minor);
    
    if (ver_major >= 3 && ver_minor >= 3) {
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    glfwSetErrorCallback(glfwErrorCallback);
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(res_x, res_y, title, nullptr, nullptr);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return nullptr;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        printf("Failed to initialize GLEW\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    return window;
}

GLuint CreateTexture1D(int size, GLenum internal_format,
    GLenum data_format, GLenum data_type, const void* data)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D, 0, internal_format, size, 0, data_format, data_type, data);
    glBindTexture(GL_TEXTURE_1D, 0);

    return tex;
}

GLuint CreateTexture2D(int width, int height, GLenum internal_format,
    GLenum data_format, GLenum data_type, const void* data)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, data_type, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}

GLuint CreateTexture3D(int width, int height, int depth, GLenum internal_format,
    GLenum data_format, GLenum data_type, const void* data)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D, 0, internal_format, width, height, depth, 0, data_format, data_type, data);
    glBindTexture(GL_TEXTURE_3D, 0);

    return tex;
}