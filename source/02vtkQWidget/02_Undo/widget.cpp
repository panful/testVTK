#include "widget.h"
#include "Style.h"

#include "vtkGenericOpenGLRenderWindow.h"
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCubeSource.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"
#include "vtkNamedColors.h"

#include <QDebug>

VTKWidget::VTKWidget(QWidget* parent):
    QVTKRenderWidget(parent)
{
    qDebug() << "=== test ===";


    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    this->setRenderWindow(renderWindow);

    // Sphere
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->Update();
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    // VTK Renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(sphereActor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());
    renderer->ResetCamera();

    // Connect VTK with Qt
    this->renderWindow()->AddRenderer(renderer);

    vtkNew<Style> style;
    this->renderWindow()->GetInteractor()->SetInteractorStyle(style);
}


bool VTKWidget::event(QEvent* evt)
{
    return QVTKRenderWidget::event(evt);
}