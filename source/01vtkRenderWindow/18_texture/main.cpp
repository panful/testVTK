/**
 * 1. 纹理的基础使用
 *
 */

#define TEST1

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

int main()
{
    vtkNew<vtkJPEGReader> jpgReader;
    jpgReader->SetFileName("../resources/test3.jpg");
    jpgReader->Update();

    // 生成纹理
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpgReader->GetOutputPort());
    texture->InterpolateOn();

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(0, 1, 0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    // 纹理坐标
    vtkNew<vtkDoubleArray> texCoords;
    texCoords->SetNumberOfComponents(2);
    texCoords->InsertNextTuple2(0., 0.);
    texCoords->InsertNextTuple2(1., 0.);
    texCoords->InsertNextTuple2(1., 1.);
    texCoords->InsertNextTuple2(0., 1.);

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetTCoords(texCoords); // 给每个顶点设置纹理坐标

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture); // 使用纹理

    //-------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> rWinInt;
    rWinInt->SetRenderWindow(window);
    rWinInt->SetInteractorStyle(style);

    window->Render();
    rWinInt->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1
