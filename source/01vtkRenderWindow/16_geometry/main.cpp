
/**
 * 101. vtkCubeSource           立方体
 *
 * 201. vtkDiskSource           圆盘
 * 202. vtkRegularPolygonSource 圆 多边形（实心圆、圆圈）
 * 203. vtkSphereSource         球
 *
 * 301. vtkLineSource           线段
 * 302. vtkLineSource           虚线
 * 303. vtkLineSource           折线
 * 304. vtkLine                 vtkCellArray->InsertNextCell(vtkLine);
 *
 * 401. vtkArrowSource          箭头
 * 402. vtkArrowSource          设置箭头的位置、方向
 *
 * 501. vtkPointSource          在一个球体内生成一堆点精灵
 * 502. vtkBoundedPointSource   在一个立方体包围盒内生成一堆顶点
 *
 * 601. vtkPlaneSource          生成一个平面（矩形、四边形、菱形）
 *
 * 901. vtkParametricSuperEllipsoid vtkParametricFunctionSource 超椭球体
 * 902. vtkRectilinearGrid 通过给定XYZ方向上的坐标，生成一个体数据
 */

#define TEST902

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

#ifdef TEST203

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    // 两个Resolution最小值为3，如果实参小于3则强制设置为3
    vtkNew<vtkSphereSource> source;
    source->SetRadius(5.0);         // 设置半径
    source->SetPhiResolution(30);   // 纬度方向，从北极到南极点的个数（不是绕纬线一圈）
    source->SetThetaResolution(30); // 经度方向，绕经线一圈顶点的个数，
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->LightingOff();
    actor->GetProperty()->EdgeVisibilityOn();

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

#endif // TEST203

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

#ifdef TEST303

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(2., 1., 0.);
    points->InsertNextPoint(4., 0., 0.);
    points->InsertNextPoint(6., 1., 0.);
    points->InsertNextPoint(8., 0., 0.);

    vtkNew<vtkLineSource> source;
    source->SetPoint1(0.0, 0.0, 0.0); // 起点，没用
    source->SetPoint2(9.0, 0.0, 0.0); // 终点，没用
    source->SetPoints(points);        // 线的顶点由points定义
    source->Update();

    std::cout << "number of points: " << source->GetOutput()->GetNumberOfPoints() << '\n';
    std::cout << "number of cells: " << source->GetOutput()->GetNumberOfCells() << '\n';

    for (vtkIdType i = 0; i < source->GetOutput()->GetNumberOfPoints(); ++i)
    {
        double pt[3] {};
        source->GetOutput()->GetPoint(i, pt);
        std::cout << "Point " << i << " : " << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
    }

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

#endif // TEST303

#ifdef TEST304

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int argc, char* argv[])
{
    // 创建三个坐标点
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(1.0, 0.0, 0.0); // 返回第一个点的ID：0
    points->InsertNextPoint(0.0, 0.0, 1.0); // 返回第二个点的ID：1
    points->InsertNextPoint(0.0, 0.0, 0.0); // 返回第三个点的ID：2

    // 每两个坐标点之间分别创建一条线
    // SetId()的第一个参数是线段的端点ID，第二个参数是连接的点的ID
    vtkNew<vtkLine> line0;
    line0->GetPointIds()->SetId(0, 0);
    line0->GetPointIds()->SetId(1, 1);

    vtkNew<vtkLine> line1;
    line1->GetPointIds()->SetId(0, 1);
    line1->GetPointIds()->SetId(1, 2);

    vtkNew<vtkLine> line2;
    line2->GetPointIds()->SetId(0, 2);
    line2->GetPointIds()->SetId(1, 0);

    // 创建单元数组，用于存储以上创建的线段
    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->SetLines(lines);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polydata);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> render;
    render->AddActor(actor);
    render->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(render);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);

    renderWindow->Render();
    iren->Start();

    return 0;
}

#endif // TEST304

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

#ifdef TEST402

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDataSetMapper.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

/// @brief 通过指定起始位置和终点位置创建一个箭头
/// @param start
/// @param end
/// @return
vtkSmartPointer<vtkDataSetMapper> createArrow(double* start, double* end)
{
    auto arrowSource = vtkSmartPointer<vtkArrowSource>::New();

    // Compute a basis
    double normalizedX[3];
    double normalizedY[3];
    double normalizedZ[3];

    // The X axis is a vector from start to end
    vtkMath::Subtract(end, start, normalizedX);
    double length = vtkMath::Norm(normalizedX);
    vtkMath::Normalize(normalizedX);

    // The Z axis is an arbitrary vector cross X
    vtkNew<vtkMinimalStandardRandomSequence> rng;
    rng->SetSeed(8775070);
    double arbitrary[3];
    for (auto i = 0; i < 3; ++i)
    {
        rng->Next();
        arbitrary[i] = rng->GetRangeValue(-10, 10);
    }
    vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
    vtkMath::Normalize(normalizedZ);

    // The Y axis is Z cross X
    vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
    vtkNew<vtkMatrix4x4> matrix;

    // Create the direction cosine matrix
    matrix->Identity();
    for (auto i = 0; i < 3; i++)
    {
        matrix->SetElement(i, 0, normalizedX[i]);
        matrix->SetElement(i, 1, normalizedY[i]);
        matrix->SetElement(i, 2, normalizedZ[i]);
    }

    // Apply the transforms
    vtkNew<vtkTransform> transform;
    transform->Translate(start);
    transform->Concatenate(matrix);
    transform->Scale(length, length, length);

    // Transform the polydata
    vtkNew<vtkTransformPolyDataFilter> transformPD;
    transformPD->SetTransform(transform);
    transformPD->SetInputConnection(arrowSource->GetOutputPort());

    vtkNew<vtkDataSetMapper> arrowMapper;
    arrowMapper->SetInputConnection(transformPD->GetOutputPort());

    return arrowMapper;
}

int main()
{
    // 自定义位置
    double start[] = { 1, 0, 10 };
    double end[]   = { 20, -10, 10 };

    vtkNew<vtkActor> actor;
    actor->SetMapper(createArrow(start, end));
    actor->GetProperty()->SetColor(1., 0., 0.);

    //-------------------------------------------------------------------------
    // 创建两个球，用来标记箭头的起点和终点
    vtkNew<vtkActor> startActor;
    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(start);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        startActor->SetMapper(mapper);
        startActor->GetProperty()->SetColor(0, 1, 0);
    }

    vtkNew<vtkActor> endActor;
    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(end);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        endActor->SetMapper(mapper);
        endActor->GetProperty()->SetColor(0, 0, 1);
    }

    //-------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(startActor);
    renderer->AddActor(endActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST402

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

#ifdef TEST902

#include <array>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    // XYZ方向上坐标的范围
    std::array<double, 5> x = { 0, 1, 2, 3, 4 };
    std::array<double, 4> y = { 5, 6, 7, 8 };
    std::array<double, 3> z = { 9, 10, 11 };

    vtkNew<vtkDoubleArray> xCoords;
    for (auto&& i : x)
    {
        xCoords->InsertNextValue(i);
    }
    vtkNew<vtkDoubleArray> yCoords;
    for (auto&& i : y)
    {
        yCoords->InsertNextValue(i);
    }
    vtkNew<vtkDoubleArray> zCoords;
    for (auto&& i : z)
    {
        zCoords->InsertNextValue(i);
    }

    vtkNew<vtkRectilinearGrid> rgrid;
    rgrid->SetDimensions(int(x.size()), int(y.size()), int(z.size()));
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    auto nc = rgrid->GetNumberOfCells();  // 单元都是VTK_VOXEL（立方体），由8个顶点构成
    auto np = rgrid->GetNumberOfPoints(); // 顶点的个数是网格三个方向上纬度的乘积
    std::cout << "Points:\t" << np << "\nCells:\t" << nc << '\n';

    vtkNew<vtkDataSetMapper> rgridMapper;
    rgridMapper->SetInputData(rgrid);

    vtkNew<vtkActor> wireActor;
    wireActor->SetMapper(rgridMapper);
    wireActor->GetProperty()->SetRepresentationToWireframe();
    wireActor->GetProperty()->SetColor(1., 0., 0.);

    double bounds[6] {};
    wireActor->GetBounds(bounds);
    std::cout << "Bounds: " << bounds[0] << ' ' << bounds[1] << ' ' << bounds[2] << ' ' << bounds[3] << ' ' << bounds[4] << ' ' << bounds[5] << '\n';

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(wireActor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST902
