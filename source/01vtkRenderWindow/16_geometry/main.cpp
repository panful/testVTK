
/**
 * 101. vtkCubeSource           立方体
 *
 * 201. vtkDiskSource           圆盘
 * 202. vtkRegularPolygonSource 圆
 *
 * 301. vtkLineSource           线段
 * 302. vtkLineSource           虚线
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

#define TEST302

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

#ifdef TEST202

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

#endif // TEST202

#ifdef TEST301

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkLineSource> source;
    source->SetPoint1(1.0, 0.0, 0.0); // 起点
    source->SetPoint2(0.0, 1.0, 0.0); // 终点
    source->SetResolution(10);        // 线段的个数
    source->Update();

    std::cout << "number of points: " << source->GetOutput()->GetNumberOfPoints() << '\n';
    std::cout << "number of cells: " << source->GetOutput()->GetNumberOfCells() << '\n';

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

#ifdef TEST302

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>

#include <iostream>
#include <string>

namespace {
void StippledLine(vtkActor* actor, int lineStipplePattern, int lineStippleRepeat)
{
    vtkNew<vtkDoubleArray> tcoords;
    vtkNew<vtkImageData> image;
    vtkNew<vtkTexture> texture;

    // Create texture.
    int dimension = 16 * lineStippleRepeat;

    image->SetDimensions(dimension, 1, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 4);
    image->SetExtent(0, dimension - 1, 0, 0, 0, 0);
    unsigned char* pixel;
    pixel             = static_cast<unsigned char*>(image->GetScalarPointer());
    unsigned char on  = 255;
    unsigned char off = 0;
    for (int i = 0; i < 16; ++i)
    {
        unsigned int mask   = (1 << i);
        unsigned int bit    = (lineStipplePattern & mask) >> i;
        unsigned char value = static_cast<unsigned char>(bit);
        if (value == 0)
        {
            for (int j = 0; j < lineStippleRepeat; ++j)
            {
                *pixel       = on;
                *(pixel + 1) = on;
                *(pixel + 2) = on;
                *(pixel + 3) = off;
                pixel += 4;
            }
        }
        else
        {
            for (int j = 0; j < lineStippleRepeat; ++j)
            {
                *pixel       = on;
                *(pixel + 1) = on;
                *(pixel + 2) = on;
                *(pixel + 3) = on;
                pixel += 4;
            }
        }
    }
    vtkPolyData* polyData = dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());

    // Create texture coordnates.
    tcoords->SetNumberOfComponents(1);
    tcoords->SetNumberOfTuples(polyData->GetNumberOfPoints());
    for (int i = 0; i < polyData->GetNumberOfPoints(); ++i)
    {
        double value = static_cast<double>(i) * .5;
        tcoords->SetTypedTuple(i, &value);
    }

    polyData->GetPointData()->SetTCoords(tcoords);
    texture->SetInputData(image);
    texture->InterpolateOff();
    texture->RepeatOn();

    actor->SetTexture(texture);
}
} // namespace

// OpenGL2不再支持虚线，示例使用纹理贴图实现虚线
// https://examples.vtk.org/site/Cxx/Rendering/StippledLine/

int main(int, char*[])
{
    double p0[3] = { 1.0, 0.0, 0.0 };
    double p1[3] = { 5.0, 0.0, 0.0 };

    vtkNew<vtkLineSource> lines;
    lines->SetResolution(11);
    lines->SetPoint1(p0);
    lines->SetPoint2(p1);
    lines->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(lines->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(5);
    actor->GetProperty()->SetColor(1., 0., 0.);

    StippledLine(actor, 0xA1A1, 2);

    vtkNew<vtkRenderer> ren1;
    ren1->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(640, 480);
    renWin->SetWindowName("StippledLine");

    renWin->AddRenderer(ren1);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);
    ren1->AddActor(actor);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST302

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
