
/**
 * 101. vtkCubeSource           立方体
 *
 * 201. vtkDiskSource           圆盘
 *
 * 301. vtkRegularPolygonSource 圆
 *
 * 401. vtkArrowSource          箭头
 *
 * 501. vtkPointSource          在一个球体内生成一堆点精灵
 * 502. vtkBoundedPointSource   在一个立方体包围盒内生成一堆顶点
 *
 * 601. vtkPlaneSource          生成一个平面（矩形、四边形、菱形）
 *
 * 901. vtkParametricSuperEllipsoid vtkParametricFunctionSource 超椭球体
 *
 */

#define TEST502

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

#ifdef TEST201

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

#endif // TEST201

#ifdef TEST301

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkRegularPolygonSource> source;
    source->SetNumberOfSides(10); // 设置边数，值越大越接近圆形
    source->SetRadius(5.0);       // 设置半径
    source->GeneratePolygonOff(); // 不生成多边形，默认开启
    source->GeneratePolylineOn(); // 生成圆圈边框
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

#endif // TEST301

#ifdef TEST401

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkArrowSource> source;
    source->SetShaftRadius(.1);     // 轴半径
    source->SetShaftResolution(10); // 轴分辨率
    source->SetTipResolution(10);   // 尖端分辨率
    source->SetTipLength(.3);       // 尖端高度，轴的长度为1
    source->SetTipRadius(.2);       // 尖端半径
    source->SetInvert(false);       // 是否翻转
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

#endif // TEST401

#ifdef TEST501

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    // 在一个指定的球内部（或者外壳）上随机生成指定个数的点精灵
    vtkNew<vtkPointSource> source;
    source->SetCenter(0.0, 0.0, 0.0);
    source->SetNumberOfPoints(1000);    // 点的个数
    source->SetDistributionToUniform(); // 球的内部均匀分布
    source->SetDistributionToShell();   // 只有球的外壳上分布，内部没有
    source->SetRadius(10.);             // 点云所在球的半径（不是点的半径，点就是一个点，没有半径）
    source->Update();

    source->GetOutput()->Print(std::cout);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(5.f);

    auto bounds = actor->GetBounds();
    std::cout << bounds[0] << '\t' << bounds[1] << '\n' << bounds[2] << '\t' << bounds[3] << '\n' << bounds[4] << '\t' << bounds[5] << '\n';

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

#endif // TEST501

#ifdef TEST502

#include <numeric>
#include <vector>
#include <vtkActor.h>
#include <vtkBoundedPointSource.h>
#include <vtkCellArray.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    // 生成的只有顶点数据，没有单元数据
    vtkNew<vtkBoundedPointSource> source;
    source->SetNumberOfPoints(10000);                 // 点的个数
    source->SetBounds(-10., 10., -10., 10., -2., 2.); // 包围盒范围
    source->Update();

    // 将每一个顶点设置为一个单元（点精灵）
    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 10000; ++i)
    {
        vtkIdType pts = i;
        cells->InsertNextCell(1, &pts);
    }

    source->GetOutput()->SetVerts(cells);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(5.f);

    auto bounds = actor->GetBounds();
    std::cout << bounds[0] << '\t' << bounds[1] << '\n' << bounds[2] << '\t' << bounds[3] << '\n' << bounds[4] << '\t' << bounds[5] << '\n';

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

#endif // TEST502

#ifdef TEST601

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{

    vtkNew<vtkPlaneSource> source;
    source->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION); // 设置输出点的精度（单精度、双精度）
    source->SetResolution(10, 10);     // 由多少个格子组成，注意是格子不是线的个数，格子比线的个数少1（X和Y方向）
    source->SetNormal(0., 0., 1.);     // 设置法线，默认是(0,0,1)
    source->SetPoint1(5., -10., 0.);   // 右下角的顶点
    source->SetPoint2(-5., 10., 0.);   // 左上角的顶点
    source->SetOrigin(-10., -10., 0.); // 左下角的起始顶点（其他的顶点以这个顶点为标志，通过偏移得到）
    source->SetCenter(0., 0., 0.);     // 平面的中心，Origin Center Point1 Point2会互相影响
    source->Update();

    // 四个顶点的顺序依次是（Resolution为1时的四个顶点）：左下 右下 左上 右上
    for (vtkIdType i = 0; i < source->GetOutput()->GetNumberOfPoints(); ++i)
    {
        double pt[3] {};
        source->GetOutput()->GetPoint(i, pt);
        std::cout << "point " << i << " : " << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
    }

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(5.f);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    auto bounds = actor->GetBounds();
    std::cout << bounds[0] << '\t' << bounds[1] << '\n' << bounds[2] << '\t' << bounds[3] << '\n' << bounds[4] << '\t' << bounds[5] << '\n';

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

#endif // TEST601

#ifdef TEST901

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

#endif // TEST901
