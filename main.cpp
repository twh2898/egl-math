#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <fmt/core.h>

#include <fstream>
#include <string>

#include "Shader.hpp"
#include "vbo.hpp"

static const EGLint configAttribs[] = {EGL_SURFACE_TYPE,
                                       EGL_PBUFFER_BIT,
                                       EGL_BLUE_SIZE,
                                       8,
                                       EGL_GREEN_SIZE,
                                       8,
                                       EGL_RED_SIZE,
                                       8,
                                       EGL_ALPHA_SIZE,
                                       8,
                                       EGL_DEPTH_SIZE,
                                       8,
                                       EGL_RENDERABLE_TYPE,
                                       EGL_OPENGL_BIT,
                                       EGL_NONE};

static const int pbufferWidth = 90;
static const int pbufferHeight = 90;

static const EGLint pbufferAttribs[] = {
    EGL_WIDTH, pbufferWidth, EGL_HEIGHT, pbufferHeight, EGL_NONE,
};

void do_render();
std::vector<uint8_t> read_buffer();
void write_buffer(const std::string &, const std::vector<uint8_t> &);

int main() {
    fmt::print("Start\n");

    // 1. Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(eglDpy, &major, &minor);

    fmt::print("Display Initialized {}.{}\n", major, minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    fmt::print("Config chosen\n");

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

    fmt::print("Surface created\n");

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    fmt::print("Bound api\n");

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    fmt::print("Context created\n");

    // from now on use your OpenGL context

    auto shader = Shader::fromFragmentPath("../shader.frag");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);

    shader->bind();
    shader->setMat4("model", glm::mat4(1.0));
    shader->setMat4("mvp", glm::mat4(1.0));
    shader->setInt("width", pbufferWidth);
    shader->setInt("height", pbufferHeight);
    do_render();
    auto buff = read_buffer();
    write_buffer("output.csv", buff);

    // 6. Terminate EGL when finished
    eglTerminate(eglDpy);

    fmt::print("Terminated\n");

    return 0;
}

void do_render() {
    VBO vbo;
    vbo.loadFromPoints({
        Vertex({1, -1, 0}, {0, 0, 0}, {1, 0}),
        Vertex({-1, -1, 0}, {0, 0, 0}, {0, 0}),
        Vertex({-1, 1, 0}, {0, 0, 0}, {0, 1}),

        Vertex({1, -1, 0}, {0, 0, 0}, {1, 0}),
        Vertex({-1, 1, 0}, {0, 0, 0}, {0, 1}),
        Vertex({1, 1, 0}, {0, 0, 0}, {1, 1}),
    });

    vbo.draw();
}

int back_to_int(const std::vector<uint8_t> & buff, size_t index) {
    int res = 0;
    for (int i = 0; i < 4; i++) {
        res = (res << 8) | buff[index * 4 + i];
    }
    return res;
}

std::vector<uint8_t> read_buffer() {
    std::vector<uint8_t> buff(pbufferWidth * pbufferHeight * 4, 0);
    glReadPixels(0, 0, pbufferWidth, pbufferHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                 buff.data());

    fmt::print("Pixel 0 is {} {} {} {}\n", buff[0], buff[1], buff[2], buff[3]);
    fmt::print("That is {}\n", back_to_int(buff, 0));
    return buff;
}

void write_buffer(const std::string & filename, const std::vector<uint8_t> & buff) {
    std::ofstream os(filename);

    for (int r = 0; r < pbufferHeight; r++) {
        for (int c = 0; c < pbufferWidth; c++) {
            if (c > 0) {
                os << ", ";
            }
            os << back_to_int(buff, r * pbufferHeight + c);
        }
        os << std::endl;
    }
    os.close();
}