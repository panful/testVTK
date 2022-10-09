#include "viewer.h"

#include <vtkNew.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

#include <iostream>

LeoViewer::LeoViewer(QWidget* parent) :
    QVTKRenderWidget(parent)
{
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    renderer = vtkRenderer::New();

    this->setRenderWindow(window);
    window->AddRenderer(renderer);

    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkCubeSource> cube;
    cube->Update();
    vtkNew<vtkActor> actor;
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(cube->GetOutput());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    actor->GetProperty()->SetColor(0., 1., 0.);
    renderer->GetActiveCamera()->SetParallelProjection(false); // true平行投影  false透视投影
}

void LeoViewer::TestPrint()
{
    std::cout << "=== test\n";
}

void LeoViewer::SetColor(std::array<double, 3> color)
{
    std::cout << "===\n";
    auto ret = this;
    renderer->SetBackground(color.data());
    this->RenderWindow->Render();
}

