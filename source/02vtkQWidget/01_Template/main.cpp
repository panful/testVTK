
#include <QApplication>
#include <QVTKRenderWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCubeSource.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QVTKRenderWidget* widget = new QVTKRenderWidget();

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    widget->setRenderWindow(renderWindow);

    vtkNew<vtkCubeSource> cubeSource;
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(cubeSource->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(0, .9, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(sphereActor);
    renderer->SetBackground(.1,.2,.3);
    widget->renderWindow()->AddRenderer(renderer);

    widget->setWindowTitle("Test");
    widget->resize(800, 600);
    widget->show();

    return     app.exec();
}