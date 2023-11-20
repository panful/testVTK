
/**
 * 101. vtkCubeSource 立方体
 * 102. vtkDiskSource 圆盘
 * 201. vtkParametricSuperEllipsoid 超椭球体
 *
 */

#define TEST102

#ifdef TEST101

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkDiskSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkDiskSource> source;
    source->SetInnerRadius(3.0);              // 设置内半径
    source->SetOuterRadius(5.0);              // 设置外半径
    source->SetRadialResolution(20);          // 设置径向分辨率
    source->SetCircumferentialResolution(20); // 设置周向分辨率
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST102

#ifdef TEST201

#include <vtkActor.h>
#include <vtkElevationFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    vtkNew<vtkParametricSuperEllipsoid> surface;
    surface->SetN1(2.0);
    surface->SetN2(1.);

    vtkNew<vtkParametricFunctionSource> source;
    source->SetParametricFunction(surface);
    source->Update();

    // source->GetOutput()->Print(std::cout);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();

    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST201