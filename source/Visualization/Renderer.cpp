#include "Renderer.hpp"
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_gl.h>
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#else
#if _WIN32
#include <GL/gl.h>
#include <CL/cl_gl.h>
#else
#include <GL/glx.h>
#include <CL/cl_gl.h>
#endif
#endif

using namespace fast;

Renderer::Renderer() {
    mWindow = -1;
    mLevel = -1;
}

void Renderer::setIntensityLevel(float level) {
    mLevel = level;
}

float Renderer::getIntensityLevel() {
    return mLevel;
}

void Renderer::setIntensityWindow(float window) {
    if(window <= 0)
        throw Exception("Intensity window has to be above 0.");
    mWindow = window;
}

float Renderer::getIntensityWindow() {
    return mWindow;
}



