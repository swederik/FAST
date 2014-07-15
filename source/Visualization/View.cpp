#include "View.hpp"
#include "Exception.hpp"
#include "DeviceManager.hpp"

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_gl.h>
#include <OpenGL/OpenGL.h>
#else
#if _WIN32
#include <GL/gl.h>
#include <GL/glu.h>
#include <CL/cl_gl.h>
#else
#include <GL/glx.h>
#include <GL/glu.h>
#include <CL/cl_gl.h>
#endif
#endif

#include <QCursor>

using namespace fast;

void View::addRenderer(Renderer::pointer renderer) {
    mRenderers.push_back(renderer);
}

View::View() {

    zNear = 0.1;
    zFar = 1000;
    fieldOfViewY = 45;
    isIn2DMode = false;
    mLeftMouseButtonIsPressed = false;
    mMiddleMouseButtonIsPressed = false;

    mFramerate = 25;
    // Set up a timer that will call update on this object at a regular interval
    timer = new QTimer(this);
    timer->start(1000/mFramerate); // in milliseconds
    timer->setSingleShot(false);
    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
}



void View::setMaximumFramerate(unsigned int framerate) {
    if(framerate == 0)
        throw Exception("Framerate cannot be 0.");

    mFramerate = framerate;
    timer->stop();
    timer->start(1000/mFramerate); // in milliseconds
    timer->setSingleShot(false);
}

void View::execute() {
}

void View::initializeGL() {
    // Update all renderes
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->update();
    }
    setOpenGLContext(OpenCLDevice::pointer(DeviceManager::getInstance().getDefaultVisualizationDevice())->getGLContext());
    // Set up viewport and projection transformation
    glMatrixMode(GL_PROJECTION);
    aspect = (float)this->width() / this->height();
    fieldOfViewX = aspect*fieldOfViewY;
    glViewport(0, 0, this->width(), this->height());
    gluPerspective(fieldOfViewY, aspect, zNear, zFar);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Initialize camera

    // Get bounding boxes of all objects
    Float3 min, max;
    Float3 centroid;
    BoundingBox box = mRenderers[0]->getBoundingBox();
    std::cout << box << std::endl;
    Float3 corner = box.getCorners()[0];
    min[0] = corner[0];
    max[0] = corner[0];
    min[1] = corner[1];
    max[1] = corner[1];
    min[2] = corner[2];
    max[2] = corner[2];
    for(int i = 0; i < mRenderers.size(); i++) {
        // Apply transformation to all b boxes
        // Get max and min of x and y coordinates of the transformed b boxes
        // Calculate centroid of all b boxes

        BoundingBox box = mRenderers[i]->getBoundingBox();
        Vector<Float3, 8> corners = box.getCorners();

        for(int j = 0; j < 8; j++) {
            for(uint k = 0; k < 3; k++) {
                if(corners[j][k] < min[k])
                    min[k] = corners[j][k];
                if(corners[j][k] > max[k])
                    max[k] = corners[j][k];
            }
        }
    }
    centroid[0] = (max.x()-min.x())*0.5;
    centroid[1] = (max.y()-min.y())*0.5;
    centroid[2] = (max.z()-min.z())*0.5;

    std::cout << "Centroid set to: " << centroid.x() << " " << centroid.y() << " " << centroid.z() << std::endl;

    // Initialize rotation point to centroid of object
    rotationPoint = centroid;

    // Calculate initiali translation of camera
    // Move centroid to z axis
    cameraPosition[0] = -centroid.x();
    cameraPosition[1] = -centroid.y();

    // Calculate z distance from origo
    float z_width = (max.x()-min.x())*0.5 / tan(fieldOfViewX*0.5);
    float z_height = (max.y()-min.y())*0.5 / tan(fieldOfViewY*0.5);
    cameraPosition[2] = -(z_width < z_height ? z_height : z_width) // minimum translation to see entire object
            -(max.z()-min.z()) // depth of the bounding box
            -50; // border

    originalCameraPosition = cameraPosition;

    std::cout << "Camera pos set to: " << cameraPosition.x() << " " << cameraPosition.y() << " " << cameraPosition.z() << std::endl;
}

void View::paintGL() {
    setOpenGLContext(OpenCLDevice::pointer(DeviceManager::getInstance().getDefaultVisualizationDevice())->getGLContext());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply camera movement
    glTranslatef(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());

    // Draw x, y and z axis
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(1000.0,0.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,1000.0,0.0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,1000.0);
    glEnd();

    // Apply global rotation
    glTranslatef(rotationPoint.x(),rotationPoint.y(),rotationPoint.z());
    glRotatef(rotation.x(), 1.0, 0.0, 0.0);
    glRotatef(rotation.y(), 0.0, 1.0, 0.0);
    glTranslatef(-rotationPoint.x(),-rotationPoint.y(),-rotationPoint.z());

    // Draw x, y and z axis
    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(1000.0,0.0,0.0);
    glColor3f(0.0,1.0,0.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,1000.0,0.0);
    glColor3f(0.0,0.0,1.0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,1000.0);
    glEnd();

    // Draw rotation point
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(rotationPoint.x(), rotationPoint.y(), rotationPoint.z());
    glEnd();

    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->update();
        mRenderers[i]->draw();
    }
}

void View::resizeGL(int width, int height) {
    setOpenGLContext(OpenCLDevice::pointer(DeviceManager::getInstance().getDefaultVisualizationDevice())->getGLContext());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    aspect = (float)width/height;
    fieldOfViewX = aspect*fieldOfViewY;
    gluPerspective(fieldOfViewY, aspect, zNear, zFar);
}

void View::keyPressEvent(QKeyEvent* event) {
    // Relay keyboard event info to renderers
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->keyPressEvent(event);
    }
}

void View::mouseMoveEvent(QMouseEvent* event) {

    if(mMiddleMouseButtonIsPressed) {
        float deltaX = event->x() - previousX;
        float deltaY = event->y() - previousY;

        float viewportWidth = tan((fieldOfViewX*M_PI/180)*0.5) * (-cameraPosition.z()) * 2;
        float viewportHeight = tan((fieldOfViewY*M_PI/180)*0.5) * (-cameraPosition.z()) * 2;
        float actualMovementX = (deltaX * (viewportWidth/width()));
        float actualMovementY = (deltaY * (viewportHeight/height()));
        cameraPosition[0] += actualMovementX;
        cameraPosition[1] -= actualMovementY;
        previousX = event->x();
        previousY = event->y();
    } else if(mLeftMouseButtonIsPressed) {
        int cx = width()/2;
        int cy = height()/2;

        if(event->x() == cx && event->y() == cy){ //The if cursor is in the middle
            return;
        }

        int diffx=event->x()-cx; //check the difference between the current x and the last x position
        int diffy=event->y()-cy; //check the difference between the current y and the last y position
        rotation[0] += (float)diffy/2; //set the xrot to xrot with the addition of the difference in the y position
        rotation[1] += (float)diffx/2;// set the xrot to yrot with the addition of the difference in the x position
        QCursor::setPos(mapToGlobal(QPoint(cx,cy)));
    }
    // Relay mouse event info to renderers
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->mouseMoveEvent(event, this);
    }
}

void View::mousePressEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        mLeftMouseButtonIsPressed = true;
    } else if(event->button() == Qt::MiddleButton) {
        previousX = event->x();
        previousY = event->y();
        mMiddleMouseButtonIsPressed = true;
    }
    // Relay mouse event info to renderers
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->mousePressEvent(event);
    }
}

void View::wheelEvent(QWheelEvent* event) {
    if(event->delta() > 0) {
        cameraPosition[2] += 10;
    } else if(event->delta() < 0) {
        cameraPosition[2] += -10;
    }
}

void View::mouseReleaseEvent(QMouseEvent* event) {
    if(event->button() == Qt::LeftButton) {
        mLeftMouseButtonIsPressed = false;
    } else if(event->button() == Qt::MiddleButton) {
        mMiddleMouseButtonIsPressed = false;
    }
    // Relay mouse event info to renderers
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->mouseReleaseEvent(event);
    }
}
void View::resizeEvent(QResizeEvent* event) {
    this->resize(event->size().width(), event->size().height());
    //this->resizeGL(event->size().width(), event->size().height());
    // Relay mouse event info to renderers
    for(unsigned int i = 0; i < mRenderers.size(); i++) {
        mRenderers[i]->resizeEvent(event);
    }
}