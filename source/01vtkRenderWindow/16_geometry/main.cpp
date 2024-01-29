
/**
 * 101. vtkCubeSource           立方体
 *
 * 201. vtkDiskSource           圆盘
 * 202. vtkRegularPolygonSource 圆 多边形（实心圆、圆圈）
 * 203. vtkSphereSource         球
 * 204. vtkConeSource           圆锥
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
 * 701. vtkNonLinearCell        非线性单元
 * 702. vtkQuadraticTriangle    二次插值三角形
 * 703. vtkBezierTriangle       任意阶贝塞尔三角形
 * 704. vtkBiQuadraticTriangle  双二次插值三角形
 * 705. vtkLagrangeTriangle     基于拉格朗日插值的三角形
 * 706. vtkHigherOrderTriangle  高阶插值的三角形
 *
 * vtkParametricFunctionSource https://examples.vtk.org/site/Cxx/GeometricObjects/ParametricObjectsDemo/
 * 901. vtkParametricSuperEllipsoid 超椭球体
 * 902. vtkRectilinearGrid  通过给定XYZ方向上的坐标，生成一个体数据(有很多立方体堆积而成，一个立方体是一个单元)
 * 903. vtkParametricSpline 样条曲线 https://zhuanlan.zhihu.com/p/587486358
 * 904. vtkParametricTorus  圆环体
 */

#define TEST204

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

#ifdef TEST204

#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    vtkNew<vtkConeSource> source;
    source->SetCenter(0., 0., 0.);
    source->SetDirection(1., 0., 0.);
    source->SetHeight(2.);
    source->SetRadius(1.);
    source->SetResolution(5); // 圆锥的底座有几条棱
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

#endif // TEST204

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

#ifdef TEST701

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3DMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLabeledDataMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGrid.h>

#include <vtkBiQuadraticQuad.h>
#include <vtkBiQuadraticQuadraticHexahedron.h>
#include <vtkBiQuadraticQuadraticWedge.h>
#include <vtkBiQuadraticTriangle.h>
#include <vtkCubicLine.h>
#include <vtkQuadraticEdge.h>
#include <vtkQuadraticHexahedron.h>
#include <vtkQuadraticLinearQuad.h>
#include <vtkQuadraticLinearWedge.h>
#include <vtkQuadraticPolygon.h>
#include <vtkQuadraticPyramid.h>
#include <vtkQuadraticQuad.h>
#include <vtkQuadraticTetra.h>
#include <vtkQuadraticTriangle.h>
#include <vtkQuadraticWedge.h>
#include <vtkTriQuadraticHexahedron.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <string>
#include <vector>

// These functions return a vtkUnstructured grid corresponding to the object.
namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T>);
vtkSmartPointer<vtkUnstructuredGrid> MakeQuadraticPolygon();
} // namespace

int main(int, char*[])
{
    std::vector<std::string> titles;
    std::vector<vtkSmartPointer<vtkTextMapper>> textMappers;
    std::vector<vtkSmartPointer<vtkActor2D>> textActors;

    std::vector<vtkSmartPointer<vtkUnstructuredGrid>> uGrids;
    std::vector<vtkSmartPointer<vtkDataSetMapper>> mappers;
    std::vector<vtkSmartPointer<vtkActor>> actors;
    std::vector<vtkSmartPointer<vtkRenderer>> renderers;

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticEdge>::New()));
    titles.push_back("VTK_QUADRATIC_EDGE (= 21)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticTriangle>::New()));
    titles.push_back("VTK_QUADRATIC_TRIANGLE (= 22)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticQuad>::New()));
    titles.push_back("VTK_QUADRATIC_QUAD (= 23)");

    uGrids.push_back(MakeQuadraticPolygon());
    titles.push_back("VTK_QUADRATIC_POLYGON (= 36)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticTetra>::New()));
    titles.push_back("VTK_QUADRATIC_TETRA (= 24)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticHexahedron>::New()));
    titles.push_back("VTK_QUADRATIC_HEXAHEDRON (= 25)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticWedge>::New()));
    titles.push_back("VTK_QUADRATIC_WEDGE (= 26)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticPyramid>::New()));
    titles.push_back("VTK_QUADRATIC_PYRAMID (= 27)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkBiQuadraticQuad>::New()));
    titles.push_back("VTK_BIQUADRATIC_QUAD (= 28)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkTriQuadraticHexahedron>::New()));
    titles.push_back("VTK_TRIQUADRATIC_HEXAHEDRON (= 29)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticLinearQuad>::New()));
    titles.push_back("VTK_QUADRATIC_LINEAR_QUAD (= 30)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkQuadraticLinearWedge>::New()));
    titles.push_back("VTK_QUADRATIC_LINEAR_WEDGE (= 31)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkBiQuadraticQuadraticWedge>::New()));
    titles.push_back("VTK_BIQUADRATIC_QUADRATIC_WEDGE (= 32)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkBiQuadraticQuadraticHexahedron>::New()));
    titles.push_back("VTK_BIQUADRATIC_QUADRATIC_HEXAHEDRON (= 33)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkBiQuadraticTriangle>::New()));
    titles.push_back("VTK_BIQUADRATIC_TRIANGLE (= 34)");

    uGrids.push_back(MakeUnstructuredGrid(vtkSmartPointer<vtkCubicLine>::New()));
    titles.push_back("VTK_CUBIC_LINE (= 35)");

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(800, 600);
    renWin->SetWindowName("Isoparametric Cell");

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);
    iRen->SetInteractorStyle(style);

    // Create one sphere for all
    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(21);
    sphere->SetThetaResolution(21);
    sphere->SetRadius(.08);

    // Create one text property for all
    vtkNew<vtkTextProperty> textProperty;
    textProperty->SetFontSize(10);
    textProperty->SetJustificationToCentered();

    // Create and link the mappers actors and renderers together.
    for (unsigned int i = 0; i < uGrids.size(); ++i)
    {
        std::cout << "Creating: " << titles[i] << std::endl;
        textMappers.push_back(vtkSmartPointer<vtkTextMapper>::New());
        textActors.push_back(vtkSmartPointer<vtkActor2D>::New());

        textMappers[i]->GetTextProperty()->SetFontSize(10);
        textMappers[i]->GetTextProperty()->ShadowOn();
        mappers.push_back(vtkSmartPointer<vtkDataSetMapper>::New());
        actors.push_back(vtkSmartPointer<vtkActor>::New());
        renderers.push_back(vtkSmartPointer<vtkRenderer>::New());
        mappers[i]->SetInputData(uGrids[i]);
        actors[i]->SetMapper(mappers[i]);
        actors[i]->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
        actors[i]->GetProperty()->EdgeVisibilityOn();
        actors[i]->GetProperty()->SetLineWidth(3);
        actors[i]->GetProperty()->SetOpacity(.5);
        renderers[i]->AddViewProp(actors[i]);

        textMappers[i]->SetInput(titles[i].c_str());
        textActors[i]->SetMapper(textMappers[i]);
        textActors[i]->SetPosition(50, 10);
        renderers[i]->AddViewProp(textActors[i]);

        // Label the points
        vtkSmartPointer<vtkLabeledDataMapper> labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
        labelMapper->SetInputData(uGrids[i]);
        vtkSmartPointer<vtkActor2D> labelActor = vtkSmartPointer<vtkActor2D>::New();
        labelActor->SetMapper(labelMapper);
        renderers[i]->AddViewProp(labelActor);

        // Glyph the points
        vtkSmartPointer<vtkGlyph3DMapper> pointMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
        pointMapper->SetInputData(uGrids[i]);
        pointMapper->SetSourceConnection(sphere->GetOutputPort());
        pointMapper->ScalingOff();
        pointMapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> pointActor = vtkSmartPointer<vtkActor>::New();
        pointActor->SetMapper(pointMapper);
        pointActor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Banana").GetData());
        pointActor->GetProperty()->SetSpecular(.6);
        pointActor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
        pointActor->GetProperty()->SetSpecularPower(100);
        renderers[i]->AddViewProp(pointActor);

        renWin->AddRenderer(renderers[i]);
    }

    // Setup the viewports
    int xGridDimensions = 4;
    int yGridDimensions = 4;
    int rendererSize    = 240;

    renWin->SetSize(rendererSize * xGridDimensions, rendererSize * yGridDimensions);

    for (int row = 0; row < yGridDimensions; row++)
    {
        for (int col = 0; col < xGridDimensions; col++)
        {
            int index = row * xGridDimensions + col;

            // (xmin, ymin, xmax, ymax)
            // (xmin, ymin, xmax, ymax)
            double viewport[4] = { static_cast<double>(col) / xGridDimensions, static_cast<double>(yGridDimensions - (row + 1)) / yGridDimensions,
                static_cast<double>(col + 1) / xGridDimensions, static_cast<double>(yGridDimensions - row) / yGridDimensions };

            if (index > int(actors.size()) - 1)
            {
                // Add a renderer even if there is no actor.
                // This makes the render window background all the same color.
                vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
                ren->SetBackground(colors->GetColor3d("SlateGray").GetData());
                ren->SetViewport(viewport);
                renWin->AddRenderer(ren);
                continue;
            }

            renderers[index]->SetViewport(viewport);
            renderers[index]->SetBackground(colors->GetColor3d("SlateGray").GetData());
            renderers[index]->ResetCamera();
            renderers[index]->GetActiveCamera()->Azimuth(30);
            renderers[index]->GetActiveCamera()->Elevation(-30);
            renderers[index]->ResetCameraClippingRange();
        }
    }

    iRen->Initialize();
    renWin->Render();
    iRen->Start();

    return EXIT_SUCCESS;
}

namespace {
template <typename T>
vtkSmartPointer<vtkUnstructuredGrid> MakeUnstructuredGrid(vtkSmartPointer<T> aCell)
{
    double* pcoords = aCell->GetParametricCoords();
    for (int i = 0; i < aCell->GetNumberOfPoints(); ++i)
    {
        aCell->GetPointIds()->SetId(i, i);
        aCell->GetPoints()->SetPoint(i, *(pcoords + 3 * i), *(pcoords + 3 * i + 1), *(pcoords + 3 * i + 2));
    }

    vtkSmartPointer<vtkUnstructuredGrid> ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
    ug->SetPoints(aCell->GetPoints());
    ug->InsertNextCell(aCell->GetCellType(), aCell->GetPointIds());
    return ug;
}

vtkSmartPointer<vtkUnstructuredGrid> MakeQuadraticPolygon()
{
    vtkNew<vtkQuadraticPolygon> quadraticPolygon;

    quadraticPolygon->GetPointIds()->SetNumberOfIds(8);
    quadraticPolygon->GetPointIds()->SetId(0, 0);
    quadraticPolygon->GetPointIds()->SetId(1, 1);
    quadraticPolygon->GetPointIds()->SetId(2, 2);
    quadraticPolygon->GetPointIds()->SetId(3, 3);
    quadraticPolygon->GetPointIds()->SetId(4, 4);
    quadraticPolygon->GetPointIds()->SetId(5, 5);
    quadraticPolygon->GetPointIds()->SetId(6, 6);
    quadraticPolygon->GetPointIds()->SetId(7, 7);

    quadraticPolygon->GetPoints()->SetNumberOfPoints(8);
    quadraticPolygon->GetPoints()->SetPoint(0, 0.0, 0.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(1, 2.0, 0.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(2, 2.0, 2.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(3, 0.0, 2.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(4, 1.0, 0.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(5, 2.0, 1.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(6, 1.0, 2.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(7, 0.0, 1.0, 0.0);
    quadraticPolygon->GetPoints()->SetPoint(5, 3.0, 1.0, 0.0);

    vtkSmartPointer<vtkUnstructuredGrid> ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
    ug->SetPoints(quadraticPolygon->GetPoints());
    ug->InsertNextCell(quadraticPolygon->GetCellType(), quadraticPolygon->GetPointIds());
    return ug;
}
} // namespace

#endif // TEST701

#ifdef TEST702

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkQuadraticTriangle.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
    vtkNew<vtkQuadraticTriangle> aCell;

    double* pcoords = aCell->GetParametricCoords();
    for (int i = 0; i < aCell->GetNumberOfPoints(); ++i)
    {
        aCell->GetPointIds()->SetId(i, i);
        aCell->GetPoints()->SetPoint(i, *(pcoords + 3 * i), *(pcoords + 3 * i + 1), *(pcoords + 3 * i + 2));
    }

    std::cout << "NonLinearCell points:\t" << aCell->GetNumberOfPoints() << '\n';
    std::cout << "NonLinearCell edges:\t" << aCell->GetNumberOfEdges() << '\n';
    std::cout << "NonLinearCell faces:\t" << aCell->GetNumberOfFaces() << '\n';

    vtkNew<vtkUnstructuredGrid> ug;
    ug->SetPoints(aCell->GetPoints());
    ug->InsertNextCell(aCell->GetCellType(), aCell->GetPointIds());

    std::cout << "UnsGrid points:\t" << ug->GetNumberOfPoints() << '\n';
    std::cout << "UnsGrid cells:\t" << ug->GetNumberOfCells() << '\n';

    //---------------------------------------------------------------------
    // vtkQuadraticTriangle 是 vtkUnstructuredGrid 类型的网格单元
    // 会在 vtkDataSetMapper 中通过 vtkDataSetSurfaceFilter 转换为 vtkPolyData 数据然后渲染
    // 一个 vtkQuadraticTriangle 渲染出来实际有4个单元
    vtkNew<vtkDataSetSurfaceFilter> filter;
    filter->SetInputData(ug);
    filter->Update();

    std::cout << "PolyData points: " << filter->GetOutput()->GetNumberOfPoints() << '\n';
    std::cout << "PolyData cells: " << filter->GetOutput()->GetNumberOfCells() << '\n';

    //---------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(ug);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
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

#endif // TEST702

#ifdef TEST703

#include <vtkActor.h>
#include <vtkBezierTriangle.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
    int nPoints = 15;
    vtkNew<vtkBezierTriangle> triangle;
    triangle->GetPointIds()->SetNumberOfIds(nPoints);
    triangle->GetPoints()->SetNumberOfPoints(nPoints);
    triangle->Initialize();
    double* points = triangle->GetParametricCoords();
    for (vtkIdType i = 0; i < nPoints; i++)
    {
        triangle->GetPointIds()->SetId(i, i);
        triangle->GetPoints()->SetPoint(i, &points[3 * i]);
    }

    std::cout << "Cell points:\t" << triangle->GetNumberOfPoints() << '\n';
    std::cout << "Cell edges:\t" << triangle->GetNumberOfEdges() << '\n';
    std::cout << "Cell faces:\t" << triangle->GetNumberOfFaces() << '\n';

    vtkNew<vtkUnstructuredGrid> ug;
    ug->SetPoints(triangle->GetPoints());
    ug->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());

    std::cout << "Grid points:\t" << ug->GetNumberOfPoints() << '\n';
    std::cout << "Grid cells:\t" << ug->GetNumberOfCells() << '\n';

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(ug);

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

#endif // TEST703

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

#ifdef TEST903

#include <vtkActor.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    // 样条曲线的顶点
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(2., 1., 0.);
    points->InsertNextPoint(3., 0., 0.);

    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(points);

    vtkNew<vtkParametricFunctionSource> functionSource;
    functionSource->SetUResolution(20); // 设置线段个数
    functionSource->SetParametricFunction(spline);
    functionSource->Update();

    auto nc = functionSource->GetOutput()->GetNumberOfCells();
    auto np = functionSource->GetOutput()->GetNumberOfPoints();
    std::cout << "Points:\t" << np << "\nCells:\t" << nc << '\n';

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(functionSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST903

#ifdef TEST904

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    vtkNew<vtkParametricTorus> surface;

    vtkNew<vtkParametricFunctionSource> source;
    source->SetUResolution(20);
    source->SetVResolution(20);
    source->SetParametricFunction(surface);
    source->Update();

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

#endif // TEST904
