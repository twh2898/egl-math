#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <fmt/core.h>

#include <fstream>
#include <string>

#include "buffer.hpp"
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

static const int pbufferWidth = 2;
static const int pbufferHeight = 1;

static const EGLint pbufferAttribs[] = {
    EGL_WIDTH, pbufferWidth, EGL_HEIGHT, pbufferHeight, EGL_NONE,
};

const std::vector<int> one = {
    0, 1
};

const std::vector<int> two = {
    2, 3
};

void do_render();
std::vector<int> read_buffer();
void write_buffer(const std::string &, const std::vector<int> &);

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
    if (!shader)
        return 1;

    auto buff1 = Buffer::fromTable(one, pbufferWidth, pbufferHeight);
    if (!buff1)
        return 2;
    auto buff2 = Buffer::fromTable(two, pbufferWidth, pbufferHeight);
    if (!buff2)
        return 3;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->bind();
    shader->setMat4("model", glm::mat4(1.0));
    shader->setMat4("mvp", glm::mat4(1.0));
    shader->setInt("width", pbufferWidth);
    shader->setInt("height", pbufferHeight);
    glActiveTexture(GL_TEXTURE0);
    buff1->bind();
    shader->setInt("one", 0);
    glActiveTexture(GL_TEXTURE0+1);
    buff2->bind();
    shader->setInt("two", 1);
    fmt::print("{}\n", buff2->getTexId());
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

std::vector<int> read_buffer() {
    std::vector<int> buff(pbufferWidth * pbufferHeight * 4, 0);
    glReadPixels(0, 0, pbufferWidth, pbufferHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                 buff.data());
    
    fmt::print("Pixel 0 is {:#x}\n", buff[0]);
    fmt::print("Pixel 1 is {:#x}\n", buff[1]);

    return buff;
}

void write_buffer(const std::string & filename, const std::vector<int> & buff) {
    std::ofstream os(filename);

    for (int r = 0; r < pbufferHeight; r++) {
        for (int c = 0; c < pbufferWidth; c++) {
            if (c > 0) {
                os << ", ";
            }
            os << buff[r * pbufferHeight + c];
        }
        os << std::endl;
    }
    os.close();
}