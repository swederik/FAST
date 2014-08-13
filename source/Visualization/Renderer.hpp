#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include "SmartPointers.hpp"
#include "ProcessObject.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QResizeEvent>

//#include <GL/glew.h>
//#include "SurfaceRenderer.hpp"
//#include "Image.hpp"
//#include "DynamicImage.hpp"
//#include "HelperFunctions.hpp"
//#include "DeviceManager.hpp"
//#include "View.hpp"
//#include "Utility.hpp"
//#include <QCursor>

#include <QWidget>
#include <QWindow>

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

namespace fast {

class View;

class Renderer : public ProcessObject {
    public:
        typedef SharedPointer<Renderer> pointer;
        virtual void draw() = 0;
        virtual BoundingBox getBoundingBox() = 0;
        void setIntensityLevel(float level);
        float getIntensityLevel();
        void setIntensityWindow(float window);
        float getIntensityWindow();
        virtual void keyPressEvent(QKeyEvent* event) {};
        virtual void mouseMoveEvent(QMouseEvent* event, View* view) {};
        virtual void mousePressEvent(QMouseEvent* event) {};
        virtual void mouseReleaseEvent(QMouseEvent* event) {};
        virtual void resizeEvent(QResizeEvent* event) {};
        void set_viewport (const QWidget& frame, int x, int y, int w, int h) {
            #if QT_VERSION >= 0x050100
              int m = frame.windowHandle()->devicePixelRatio();
              glViewport(m*x, m*y, m*w, m*h);
            #else
              glViewport(x, y, w, h);
            #endif

        }
        
    protected:
        Renderer();

        // Level and window intensities
        float mWindow;
        float mLevel;
};

}



#endif /* RENDERER_HPP_ */
