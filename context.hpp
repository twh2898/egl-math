#pragma once

#include <EGL/egl.h>
#include <GLES2/gl2.h>

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

class Context {
    EGLDisplay eglDpy;
    EGLint major, minor;
    EGLSurface eglSurf;
    EGLContext eglCtx;

    int width, height;

public:
    Context(int width, int height) : width(width), height(height) {
        eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(eglDpy, &major, &minor);

        EGLint numConfigs;
        EGLConfig eglCfg;
        eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

        EGLint pbufferAttribs[] = {
            EGL_WIDTH, width, EGL_HEIGHT, height, EGL_NONE,
        };
        eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

        eglBindAPI(EGL_OPENGL_API);

        eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, nullptr);
    }

    ~Context() {
        eglTerminate(eglDpy);
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    void makeCurrent() {
        eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
    }
};