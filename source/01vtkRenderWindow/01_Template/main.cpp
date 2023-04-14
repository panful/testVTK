/*
 * 1. 使用vtk自带模型
 * 2. 自定义vtkPolyData
 */

#define TEST2

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleRubberBand3D.h>
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

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);
    camera->SetFocalPoint(0, 0, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // render
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>
#include <iostream>

// 四条线段组成一个正方形
namespace {
std::array<float, 4 * 3> vertices {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0
};

std::array<long long, 4 * 2> indices {
    0, 1,
    1, 2,
    2, 3,
    3, 0
};
}

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    for (size_t i = 0; i < indices.size(); i += 2)
    {
        cells->InsertNextCell({ indices[i], indices[i + 1] });
    }

    polyData->SetPoints(points);
    polyData->SetLines(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // interactor syle
    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2
