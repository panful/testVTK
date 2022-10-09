#include <QApplication>
#include "widget.h"

#ifndef INITIAL_OPENGL
#define INITIAL_OPENGL

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
#endif // INITIAL_OPENGL

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    VTKWidget w;
    w.setWindowTitle("origin");
    w.setMinimumSize(800, 600);
    w.show();

    return app.exec();
}
