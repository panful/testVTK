#include "mainWindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char** argv)
{
    // 从渲染窗口拖出viewer 报错：ERROR: In ...\VTK-9.1.0\Rendering\OpenGL2\vtkOpenGLFramebufferObject.cxx, line 367
    // vtkOpenGLFramebufferObject(000002445318B250) : failed at glDeleteFramebuffers 16 OpenGL errors detected
    // 需要设置：QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QSurfaceFormat vtkFormat;
    vtkFormat.setRenderableType(QSurfaceFormat::OpenGL);
    vtkFormat.setVersion(3, 2);
    vtkFormat.setProfile(QSurfaceFormat::CoreProfile);
    vtkFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    vtkFormat.setRedBufferSize(8);
    vtkFormat.setGreenBufferSize(8);
    vtkFormat.setBlueBufferSize(8);
    vtkFormat.setDepthBufferSize(8);
    vtkFormat.setAlphaBufferSize(8);
    vtkFormat.setStencilBufferSize(0);
    vtkFormat.setStereo(false);
    vtkFormat.setSamples(0);

    QSurfaceFormat::setDefaultFormat(vtkFormat);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QApplication app(argc, argv);

    LeoMainWindow window;
    window.setMinimumSize(800, 600);
    window.show();

    return app.exec();
}