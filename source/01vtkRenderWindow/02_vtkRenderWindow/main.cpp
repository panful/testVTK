
/*
* mapper负责把数据映射到图像
整个大剧院<—->vtkRenderWindow的渲染窗口。
剧院中舞台<—->vtkRenderer的渲染场景。
舞台上演员<—->vtkActor的渲染对象。一个render可以有多个actor
表演中互动<—->vtkRenderWindowInteractor的交互。
交互的方式<—->vtkInteractorSyle的交互器样式。
演员的特性<—->vtkActor的不同属性vtkProperty。
观众的视角<—->vtkCamera。
*/

/*
01.鼠标左键弹起后修改actor颜色
02.绘制点，GetCell()函数崩溃的问题
03.（顶点 + 索引）绘制立方体（和opengl ebo方式类似），每个顶点添加一个颜色值
04.简单的平面，使用顶点属性数据设置颜色 vtkDataArray
05.通过定义三个指定x-y-z方向坐标的阵列来创建直线栅格。
06.批量生成一大堆线条
07.点构成线条，使用索引
08.光照
09.绘制点，线，面，类似Opengl 顶点+索引的绘制方式
10.2D网格绘制
11.拟合样条曲线生成柱状体
12.
13.多窗口使用相同的数据同一个相机（即多个窗口显示完全一样的东西，交互也是同步的）
16.
17.
18.带流场参数的立方体
19.vtkDistancePolyDataFilter 计算两个vtkPolyData的距离

21.
22.将点（向量）投影到平面
23.鼠标双击事件 DoubleClick
24.
25.多边形三角化，减少三角形的数量，图像和原来保持不变
26.纹理，光照

32.
33.vtkMaskPoints 对输入的数据进行挑拣筛选

38.plot https://kitware.github.io/vtk-examples/site/Cxx/Plotting/LinePlot/

43.reverse access ,从经过算法(vtkAlgorithm,vtkPolyDataAlgorithm)变换的数据中获取源数据（vtkPolyData,vtkDataSet）
44.vtkChart https://kitware.github.io/vtk-examples/site/Cxx/DataStructures/OctreeTimingDemo/
45.从Actor中获取polyData，并修改polyData的属性会改变原来Actor的状态，深拷贝可以不改变源Actor状态

51.利用vtk橡皮筋交互模式，实现鼠标绘制矩形

58.窗口大小改变回调函数 resizeWindowEvent
59.微信群里文件
60.文字显示，汉字
61.2D橡皮筋交互模式，重载鼠标事件  vtkCallbackCommand::SetCallback  使用回调函数替代vtkCommand::Execute()
62.

64.DICOM  MPR 医学ct图四象限

66.shared actor 多个vtkRenderWindow共享actor


*/

#define TEST65

// 在cmake加上vtk_module_autoinit就不需要在此处再初始化vtk模块
// #include <vtkAutoInit.h>
// VTK_MODULE_INIT(vtkRenderingOpenGL2);
// VTK_MODULE_INIT(vtkInteractionStyle);
// VTK_MODULE_INIT(vtkRenderingFreeType);
// VTK_MODULE_INIT(vtkRenderingContextOpenGL2);

#ifdef TEST1

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>

namespace {
// Define interaction style
class InteractorStyle : public vtkInteractorStyleRubberBandPick
{
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

    virtual void OnLeftButtonUp() override
    {
        SelectedActor->GetProperty()->SetColor(1, 0, 0);

        Superclass::OnLeftButtonUp();
    }

    void SetActor(vtkSmartPointer<vtkActor> actor)
    {
        SelectedActor = actor;
    }

private:
    vtkSmartPointer<vtkActor> SelectedActor;
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    // 如果要建立管道连接，请使用SetInputConnection
    // 如果要处理独立数据集，请使用SetInputData
    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    // cubeActor->SetTexture()

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置相机焦点

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小

    auto style = InteractorStyle::New();
    style->SetActor(cubeActor);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TSET1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>
#include <iostream>

namespace {
std::array<float, 3> vertices {
    10, 10, 10,
    // 1,0,0,
    // 1,1,0,
    // 0,1,0
};

std::array<long long, 1> indices {
    0,
    // 1,
    // 2,
    // 3,
};
} // namespace

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    for (size_t i = 0; i < indices.size(); i++)
    {
        cells->InsertNextCell({ indices[i] }); // 这种插入方式有的时候会有问题，导致 GetCell(0)函数崩溃
        cells->InsertNextCell(1, 0);
    }

    polyData->SetPoints(points);
    polyData->SetVerts(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(5);

    actor->Print(std::cout);
    actor->GetMapper()->GetInput()->Print(std::cout);
    double bounds[6] { 0 };
    actor->GetMapper()->GetInput()->GetCellBounds(0, bounds);
    auto cell = actor->GetMapper()->GetInput()->GetCell(0);
    cell->GetBounds(bounds);

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

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main()
{
    vtkNew<vtkNamedColors> colors;

    std::array<std::array<double, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } }, { { 0, 1, 0 } }, { { 0, 0, 1 } },
        { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering
        = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } }, { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };

    // We'll create the building blocks of polydata including data attributes.
    vtkNew<vtkPolyData> cube;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> polys;
    vtkNew<vtkFloatArray> scalars;

    // Load the point, cell, and data attributes.
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);
    }
    for (auto&& i : ordering)
    {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    // We now assign the pieces to the vtkPolyData.
    cube->SetPoints(points);
    cube->SetPolys(polys);
    cube->GetPointData()->SetScalars(scalars);

    // Now we'll look at it.
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(cube->GetScalarRange());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // The usual rendering stuff.
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);
    camera->SetFocalPoint(0, 0, 0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkPolyLine.h>

#include <array>

int main()
{
    vtkNew<vtkNamedColors> colors;

    // https://blog.csdn.net/zy2317878/article/details/78744825
    vtkNew<vtkDoubleArray> pcoords;    // 用来设置顶点的属性数据
    pcoords->SetNumberOfComponents(3); // 设置元组的个数为3，默认为1
    pcoords->SetNumberOfTuples(4);     // 设置总的元组个数为10
    // Assign each tuple. There are 5 specialized versions of SetTuple:
    // SetTuple1 SetTuple2 SetTuple3 SetTuple4 SetTuple9
    // These take 1, 2, 3, 4 and 9 components respectively.
    std::array<std::array<double, 3>, 4> pts = { { { { 0.0, 0.0, 0.0 } }, { { 0.0, 1.0, 0.0 } }, { { 1.0, 0.0, 0.0 } }, { { 1.0, 1.0, 0.0 } } } };
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        pcoords->SetTuple(i, pts[i].data());
    }

    // Create vtkPoints and assign pcoords as the internal data array.
    vtkNew<vtkPoints> points;
    points->SetData(pcoords);

    // Create the cells. In this case, a triangle strip with 2 triangles
    // (which can be represented by 4 points).
    vtkNew<vtkCellArray> strips;
    strips->InsertNextCell(4);
    strips->InsertCellPoint(0);
    strips->InsertCellPoint(1);
    strips->InsertCellPoint(2);
    strips->InsertCellPoint(3);

    // Create an integer array with 4 tuples. Note that when using
    // InsertNextValue (or InsertNextTuple1 which is equivalent in
    // this situation), the array will expand automatically.
    vtkNew<vtkIntArray> temperature;
    temperature->SetName("Temperature");
    temperature->InsertNextValue(10);
    temperature->InsertNextValue(20);
    temperature->InsertNextValue(30);
    temperature->InsertNextValue(40);

    // Create a double array.
    vtkNew<vtkDoubleArray> vorticity;
    vorticity->SetName("Vorticity");
    vorticity->InsertNextValue(2.7);
    vorticity->InsertNextValue(4.1);
    vorticity->InsertNextValue(5.3);
    vorticity->InsertNextValue(3.4);

    // Create the dataset. In this case, we create a vtkPolyData
    vtkNew<vtkPolyData> polydata;
    // Assign points and cells
    polydata->SetPoints(points);
    polydata->SetStrips(strips);
    // Assign scalars
    polydata->GetPointData()->SetScalars(temperature);
    // Add the vorticity array. In this example, this field
    // is not used.
    polydata->GetPointData()->AddArray(vorticity);

    // Create the mapper and set the appropriate scalar range
    // (default is (0,1)
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polydata);
    mapper->SetScalarRange(0, 40);

    // Create an actor.
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // Create the rendering objects.
    vtkNew<vtkRenderer> ren;
    ren->AddActor(actor);
    ren->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    renWin->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}
#endif // TEST4

#ifdef TEST5

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

int main()
{
    vtkNew<vtkNamedColors> colors;

    std::array<double, 47> x = { { -1.22396, -1.17188, -1.11979, -1.06771, -1.01562, -0.963542, -0.911458, -0.859375, -0.807292, -0.755208, -0.703125,
        -0.651042, -0.598958, -0.546875, -0.494792, -0.442708, -0.390625, -0.338542, -0.286458, -0.234375, -0.182292, -0.130209, -0.078125, -0.026042,
        0.0260415, 0.078125, 0.130208, 0.182291, 0.234375, 0.286458, 0.338542, 0.390625, 0.442708, 0.494792, 0.546875, 0.598958, 0.651042, 0.703125,
        0.755208, 0.807292, 0.859375, 0.911458, 0.963542, 1.01562, 1.06771, 1.11979, 1.17188 } };
    std::array<double, 33> y = { { -1.25, -1.17188, -1.09375, -1.01562, -0.9375, -0.859375, -0.78125, -0.703125, -0.625, -0.546875, -0.46875,
        -0.390625, -0.3125, -0.234375, -0.15625, -0.078125, 0, 0.078125, 0.15625, 0.234375, 0.3125, 0.390625, 0.46875, 0.546875, 0.625, 0.703125,
        0.78125, 0.859375, 0.9375, 1.01562, 1.09375, 1.17188, 1.25 } };
    std::array<double, 44> z = { { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.75, 1.8, 1.9, 2, 2.1,
        2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.75, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.75, 3.8, 3.9 } };

    // 通过定义三个指定x-y-z方向坐标的阵列来创建直线栅格。
    vtkNew<vtkFloatArray> xCoords;
    for (auto&& i : x)
    {
        xCoords->InsertNextValue(i);
    }
    vtkNew<vtkFloatArray> yCoords;
    for (auto&& i : y)
    {
        yCoords->InsertNextValue(i);
    }
    vtkNew<vtkFloatArray> zCoords;
    for (auto&& i : z)
    {
        zCoords->InsertNextValue(i);
    }

    // The coordinates are assigned to the rectilinear grid. Make sure that
    // the number of values in each of the XCoordinates, YCoordinates,
    // and ZCoordinates is equal to what is defined in SetDimensions().
    //
    vtkNew<vtkRectilinearGrid> rgrid;
    rgrid->SetDimensions(int(x.size()), int(y.size()), int(z.size()));
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    // Extract a plane from the grid to see what we've got.
    vtkNew<vtkRectilinearGridGeometryFilter> plane;
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 46, 16, 16, 0, 43);

    vtkNew<vtkPolyDataMapper> rgridMapper;
    rgridMapper->SetInputConnection(plane->GetOutputPort());

    vtkNew<vtkActor> wireActor;
    wireActor->SetMapper(rgridMapper);
    wireActor->GetProperty()->SetRepresentationToWireframe();
    wireActor->GetProperty()->SetColor(colors->GetColor3d("Indigo").GetData());

    // Create the usual rendering stuff.
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(wireActor);
    renderer->SetBackground(1, 1, 1);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Elevation(60.0);
    renderer->GetActiveCamera()->Azimuth(30.0);
    renderer->GetActiveCamera()->Zoom(1.0);
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());

    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST5

#ifdef TEST6

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkFloatArray.h>
#include <vtkHedgeHog.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>

#include <array>

int main()
{
    vtkNew<vtkNamedColors> colors;

    float rMin = 0.5, rMax = 1.0, deltaRad, deltaZ;
    std::array<int, 3> dims = { { 13, 11, 11 } };
    // std::array<int, 3> dims = { { 2,3,4 } };

    // Create the structured grid.
    vtkNew<vtkStructuredGrid> sgrid;
    sgrid->SetDimensions(dims.data());

    // We also create the points and vectors. The points
    // form a hemi-cylinder of data.
    vtkNew<vtkFloatArray> vectors;
    vectors->SetNumberOfComponents(3);
    vectors->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
    vtkNew<vtkPoints> points;
    points->Allocate(dims[0] * dims[1] * dims[2]);

    deltaZ   = 2.0 / (dims[2] - 1);
    deltaRad = (rMax - rMin) / (dims[1] - 1);
    float x[3], v[3];
    v[2] = 0.0;
    for (auto k = 0; k < dims[2]; k++)
    {
        x[2]        = -1.0 + k * deltaZ;
        int kOffset = k * dims[0] * dims[1];
        for (auto j = 0; j < dims[1]; j++)
        {
            float radius = rMin + j * deltaRad;
            int jOffset  = j * dims[0];
            for (auto i = 0; i < dims[0]; i++)
            {
                float theta = i * vtkMath::RadiansFromDegrees(15.0);
                x[0]        = radius * cos(theta);
                x[1]        = radius * sin(theta);
                v[0]        = -x[1];
                v[1]        = x[0];
                int offset  = i + jOffset + kOffset;
                points->InsertPoint(offset, x);
                vectors->InsertTuple(offset, v);
            }
        }
    }
    sgrid->SetPoints(points);
    sgrid->GetPointData()->SetVectors(vectors);

    // We create a simple pipeline to display the data.
    vtkNew<vtkHedgeHog> hedgehog;
    hedgehog->SetInputData(sgrid);
    hedgehog->SetScaleFactor(0.1);

    vtkNew<vtkPolyDataMapper> sgridMapper;
    sgridMapper->SetInputConnection(hedgehog->GetOutputPort());
    vtkNew<vtkActor> sgridActor;
    sgridActor->SetMapper(sgridMapper);
    sgridActor->GetProperty()->SetColor(colors->GetColor3d("Indigo").GetData());

    // Create the usual rendering stuff
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(sgridActor);
    renderer->SetBackground(colors->GetColor3d("Cornsilk").GetData());
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Elevation(60.0);
    renderer->GetActiveCamera()->Azimuth(30.0);
    renderer->GetActiveCamera()->Zoom(1.0);
    renWin->SetSize(600, 600);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}
#endif // TEST6

#ifdef TEST7

#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>

#include <vtkCellType.h>

int main(int argc, char* argv[])
{
    /*这种方式每一条线都要创建指针，建议使用例9方式填充数据*/

    // 创建三个坐标点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(1.0, 0.0, 0.0); // 返回第一个点的ID：0
    points->InsertNextPoint(0.0, 0.0, 1.0); // 返回第二个点的ID：1
    points->InsertNextPoint(0.0, 0.0, 0.0); // 返回第三个点的ID：2

    // 每两个坐标点之间分别创建一条线
    // SetId()的第一个参数是线段的端点ID，第二个参数是连接的点的ID
    vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();
    line0->GetPointIds()->SetId(0, 0);
    line0->GetPointIds()->SetId(1, 1);

    vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
    line1->GetPointIds()->SetId(0, 1);
    line1->GetPointIds()->SetId(1, 2);

    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
    line2->GetPointIds()->SetId(0, 2);
    line2->GetPointIds()->SetId(1, 0);

    // 创建单元数组，用于存储以上创建的线段
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);

    // 将点和线加入到数据集中，前者定义数据集的几何结构（顶点），后者定义拓扑结构（索引）
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polydata);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
    render->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(render);

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(style);

    iren->Initialize();
    iren->Start();

    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <iostream>

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;

    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);
    // cubeActor->GetProperty()->

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置相机焦点

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->GradientBackgroundOn();
    renderer->SetBackground(.8, .92, .97);
    renderer->SetBackground2(.49, .78, .92);

    renderer->AddActor(cubeActor);

    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    auto lights   = renderer->GetLights();
    auto numLiths = lights->GetNumberOfItems();
    std::cout << numLiths << '\n';

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST8

#ifdef TEST9

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkInteractorStyleUser.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>

#define _LINE

int main()
{
#ifdef _CUBE
    // 立方体
    std::array<std::array<double, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } }, { { 0, 1, 0 } }, { { 0, 0, 1 } },
        { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering
        = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } }, { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };
#endif // _CUBE

#ifdef _LINE
    // 三条线段
    std::array<std::array<double, 3>, 4> pts         = { { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } } };
    std::array<std::array<vtkIdType, 2>, 3> ordering = { { { 0, 1 }, { 1, 2 }, { 2, 3 } } };
#endif // _LINE

#ifdef _TRIANGLE
    // 两个三角形构成一个长方形
    std::array<std::array<double, 3>, 4> pts = { { { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } } }; // 左下，左上，右上，右下
    std::array<std::array<vtkIdType, 3>, 2> ordering = { { { 0, 1, 2 }, { 2, 3, 0 } } };
#endif // _TRIANGLE

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPolyData> cube;      // 可以理解为opengl的vao
    vtkNew<vtkPoints> points;      // 顶点
    vtkNew<vtkCellArray> polys;    // 索引
    vtkNew<vtkFloatArray> scalars; // vtkDataArray 属性数据

    // 顶点，可以理解为opengl的vertices
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i); // 第一个参数为顶点索引，第二个为实际值
    }
    // 索引，可以理解为opengl的indices
    for (auto&& i : ordering)
    {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    // 设置几何结构
    cube->SetPoints(points);

    // 设置拓扑结构，即设置图元类型，类似的函数还有SetVerts（点）等
    // cube->SetPolys(polys);  //多边形，面
    cube->SetLines(polys); //_LINE:三条线段（即首尾不相连），_TRIANGLE:四条线段（即首尾相连）
    // cube->SetStrips(polys); //三角形带，面
    // cube->SetVerts(polys);  //点

    // 设置每个顶点的参数，可以理解为云图中数据的w分量
    // cube->GetPointData()->SetScalars(scalars);

    // Mapper用来将输入的数据转换为几何图元（点、线、多边形）进行渲染
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);

    // 属性数据设置方法：https://blog.csdn.net/zy2317878/article/details/78744825
    // 获取流场参数的最大最小值，返回double数组
    auto scalar = cube->GetScalarRange();
    double myScalarRange[] { 0, 2 }; // 可以理解为流场数据范围，最小值0为红色，最大值2为蓝色，那么1就为绿色
    cubeMapper->SetScalarRange(cube->GetScalarRange());
    // cubeMapper->SetScalarRange(myScalarRange);

    // 设置图元颜色
    vtkNew<vtkProperty> cubeProperty;
    cubeProperty->SetColor(0, 255, 0);

    // Actor负责渲染场景中数据的可视化表达，二维空间中的数据通过vtkActor2D表达
    // Actor依赖于Mapper对象和vtkPropety对象，Mapper负责存放数据和渲染信息等；vtkProperty负责控制颜色，不透明度等属性
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    // cubeActor->SetTexture(); //设置纹理，具体用法可以看QWidgetVTK代码
    cubeActor->SetProperty(cubeProperty);

    // 相机 常用函数Dolly()，Roll()，Azimuth()，Yaw()，Elevation()，Pitch()和Zoom()
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置焦点位置

    vtkNew<vtkRenderer> renderer;   // 负责管理场景的渲染过程，组成对象包括Actor，照相机，光照
    vtkNew<vtkRenderWindow> renWin; // 窗口，一个窗口可以有多个vtkRenderer，不同的render可以在不同的视口
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren; // 提供平台独立的响应鼠标，键盘，时钟时间的交互机制
    iren->SetRenderWindow(renWin);          // 设置渲染窗口

    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();
    iren->SetInteractorStyle(style); // 定义交互器样式，默认为vtkInteractorStyleSwitch

    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // 背景颜色，可以设置为渐变色
    renderer->SetBackground(colors->GetColor3d("Black").GetData());

    renWin->SetSize(600, 600); // 窗口大小

    // interact with data
    renWin->Render();
    iren->Start(); // 程序进入事件响应循环，交互器处于等待状态

    return EXIT_SUCCESS;
}

#endif // TEST9

#ifdef TEST10

#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkInteractorStyleUnicam.h>
#include <vtkInteractorStyleUser.h>

using namespace std;

int main(int argc, char* argv[])
{
    size_t vercount = 4;
    size_t indcount = 4;
    // size_t indcount = 6;
    float vertices[]       = { 0., 0., 1., 0., 1., 1., 2., 1. }; // 共四个2D顶点
    unsigned int indices[] = { 0, 1, 2, 3 };
    // unsigned int indices[] = { 0,1,1,2,2,3 };

    vtkPoints* points   = vtkPoints::New();    // 顶点
    vtkCellArray* cells = vtkCellArray::New(); // 索引

    for (size_t i = 0; i < vercount; i++)
    {
        points->InsertNextPoint(vertices[i * 2 + 0], vertices[i * 2 + 1], 0);
    }

    for (size_t i = 0; i < indcount; i += 2)
    {
        cells->InsertNextCell({ indices[i], indices[i + 1] });
    }

    // 渲染机制未知，需要同时设置点坐标与点坐标对应的verts
    // verts中的id必须与点坐标对应
    vtkPolyData* polyData = vtkPolyData::New();
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    // 下面为正常的可视化流程，可设置的点云颜色、大小等
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputData(polyData);

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    // 设置颜色与点大小
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    // actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetLineWidth(5);

    vtkRenderer* renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    // 设置背景颜色
    renderer->SetBackground(1, 1, 1);

    vtkRenderWindow* renderWindow = vtkRenderWindow::New();
    renderWindow->AddRenderer(renderer);

    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renderWindow);

    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New(); // 左键旋转，滚轮缩放，滚轮拖动，右键缩放

    // style->set

    iren->SetInteractorStyle(style);

    iren->Initialize();
    iren->Start();

    points->Delete();
    polyData->Delete();
    mapper->Delete();
    actor->Delete();
    renderer->Delete();
    renderWindow->Delete();
    iren->Delete();
    style->Delete();

    return 0;
}

// cells->SetData();
// cells->ExportLegacyFormat();
// cells->ImportLegacyFormat();
// cells->AppendLegacyFormat();
// cells->InsertNextCell({ 2,1,2 });  // 第一个参数是单元顶点的个数

#endif // TEST10

#ifdef TEST11

#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNamedColors.h>
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
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>

int main(int, char*[])
{
    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

    // Create three points. We will join (Origin and P0) with a red line and (Origin and P1) with a green line
    double origin[3] = { 0.0, 0.0, 0.0 };
    double p0[3]     = { 0, 3, 2 };
    double p1[3]     = { 5, 4.4, 3.6 };
    double p2[3]     = { 10, 4.4, 4.2 };
    double p3[3]     = { 15, 4.4, 4.8 };

    // Create a vtkPoints object and store the points in it
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(origin);
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);

    vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetPoints(points);
    vtkSmartPointer<vtkParametricFunctionSource> functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    functionSource->SetParametricFunction(spline);
    functionSource->Update();

    vtkSmartPointer<vtkTubeFilter> Tube = vtkSmartPointer<vtkTubeFilter>::New();
    Tube->SetInputConnection(functionSource->GetOutputPort());
    Tube->SetRadius(0.5);
    Tube->SetNumberOfSides(20);
    Tube->Update();
    // Setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(Tube->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    // Setup render window, renderer, and interactor
    vtkSmartPointer<vtkRenderer> renderer         = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);

    renderer->SetBackground(colors->GetColor3d("Silver").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST11

#ifdef TEST13

// https://blog.csdn.net/calmreason/article/details/88712248

#include "vtkActor.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkContourRepresentation.h"
#include "vtkContourWidget.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"

static vtkSmartPointer<vtkRenderer> renderer01;
static vtkSmartPointer<vtkRenderer> renderer02;
static vtkSmartPointer<vtkRenderWindow> renderWindow01;
static vtkSmartPointer<vtkRenderWindow> renderWindow02;

vtkSmartPointer<vtkContourWidget> contourWidget1 = vtkSmartPointer<vtkContourWidget>::New();
vtkSmartPointer<vtkContourWidget> contourWidget2 = vtkSmartPointer<vtkContourWidget>::New();

class vtkMyCameraCallback : public vtkCommand
{
public:
    static vtkMyCameraCallback* New()
    {
        return new vtkMyCameraCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        vtkCamera* camera = static_cast<vtkCamera*>(caller);
        double camPos[3];
        double focalPos[3];
        double upVector[3];
        camera->GetPosition(camPos);
        camera->GetFocalPoint(focalPos);
        camera->GetViewUp(upVector);

        // 遍历所有renderer，都设置一遍最新的相机
        renderer01->SetActiveCamera(camera);
        renderer02->SetActiveCamera(camera);
        renderWindow01->Render();
        renderWindow02->Render();
    }

    vtkSmartPointer<vtkRenderer> m_renderer;
};

class vtkMyContourCallback : public vtkCommand
{
public:
    static vtkMyContourCallback* New()
    {
        return new vtkMyContourCallback;
    }

    void Execute(vtkObject* caller, unsigned long eventId, void* callData)
    {
        vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
        m_contourWidget->GetContourRepresentation()->GetNodePolyData(polydata);

        contourWidget1->Initialize(polydata);
        contourWidget2->Initialize(polydata);
        renderWindow01->Render();
        renderWindow02->Render();
    }

    vtkSmartPointer<vtkContourWidget> m_contourWidget;
};

int main(int argc, char* argv[])
{
    // 制作数据polydata
    int numPts                        = 5;
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (int i = 0; i < numPts; i++)
    {
        // Create numPts points evenly spread around a circumference of radius 0.1
        const double angle = 2.0 * vtkMath::Pi() * i / numPts;
        std::cout << "ponit:" << static_cast<vtkIdType>(i) << " " << 0.1 * cos(angle) << " " << 0.1 * sin(angle) << " " << 0.0 << std::endl;
        points->InsertPoint(static_cast<vtkIdType>(i), 0.1 * cos(angle), 0.1 * sin(angle), 0.0);
    }
    // Create a cell array to connect the points into meaningful geometry
    vtkIdType* vertexIndices = new vtkIdType[numPts + 1];
    for (int i = 0; i < numPts; i++)
    {
        vertexIndices[i] = static_cast<vtkIdType>(i);
    }
    // Set the last vertex to 0; this means the last line segment will join the 19th point (vertices[19])
    // with the first one (vertices[0]), thus closing the circle.
    vertexIndices[numPts] = 0;
    for (int i = 0; i < numPts + 1; ++i)
    {
        std::cout << "line:" << vertexIndices[i] << " " << vertexIndices[i + 1] << endl;
    }
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(numPts + 1, vertexIndices);
    // Create polydata to hold the geometry just created, and populate it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);

    renderer01 = vtkSmartPointer<vtkRenderer>::New();
    renderer02 = vtkSmartPointer<vtkRenderer>::New();

    renderWindow01 = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow02 = vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow01->AddRenderer(renderer01);
    renderWindow02->AddRenderer(renderer02);

    vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor01 = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkRenderWindowInteractor> windowInteractor02 = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    windowInteractor01->SetRenderWindow(renderWindow01);
    windowInteractor02->SetRenderWindow(renderWindow02);

    contourWidget1->CreateDefaultRepresentation();
    contourWidget2->CreateDefaultRepresentation();

    // 渲染时使用同样的数据
    contourWidget1->SetInteractor(windowInteractor01);
    contourWidget2->SetInteractor(windowInteractor02);
    contourWidget1->On();
    contourWidget2->On();
    contourWidget1->Initialize(polydata);
    contourWidget2->Initialize(polydata);

    // 形状使用同样的数据来保持同步
    vtkSmartPointer<vtkMyContourCallback> contourCallBack1 = vtkSmartPointer<vtkMyContourCallback>::New();
    contourCallBack1->m_contourWidget                      = contourWidget1;
    vtkSmartPointer<vtkMyContourCallback> contourCallBack2 = vtkSmartPointer<vtkMyContourCallback>::New();
    contourCallBack2->m_contourWidget                      = contourWidget2;
    contourWidget1->AddObserver(vtkCommand::EndInteractionEvent, contourCallBack1); // 拖动控制点完成
    contourWidget2->AddObserver(vtkCommand::EndInteractionEvent, contourCallBack2); // 拖动控制点完成
    contourWidget1->AddObserver(vtkCommand::KeyReleaseEvent, contourCallBack1);     // 删除一个控制点
    contourWidget2->AddObserver(vtkCommand::KeyReleaseEvent, contourCallBack2);     // 删除一个拖动控制点
    contourWidget1->AddObserver(vtkCommand::InteractionEvent, contourCallBack1);    // 拖动一个控制点
    contourWidget2->AddObserver(vtkCommand::InteractionEvent, contourCallBack2);    // 拖动一个拖动控制点

    // 相机同步
    vtkSmartPointer<vtkMyCameraCallback> cameraCallback1 = vtkSmartPointer<vtkMyCameraCallback>::New();
    cameraCallback1->m_renderer                          = renderer01;
    vtkSmartPointer<vtkMyCameraCallback> cameraCallback2 = vtkSmartPointer<vtkMyCameraCallback>::New();
    cameraCallback2->m_renderer                          = renderer02;
    renderer01->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraCallback1);
    renderer02->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraCallback2);
    renderer01->ResetCamera();
    renderer02->ResetCamera();

    // 程序启动
    windowInteractor01->Initialize();
    windowInteractor02->Initialize();
    windowInteractor01->Start();
    windowInteractor02->Start();

    return 0;
}

#endif // TEST13



#ifdef TEST18

#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include <vtkLookupTable.h>

int main()
{
    int i;
    // 定义立方体的顶点坐标
    static float x[8][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } };
    // 定义单元，每4个顶点建立一个四边形单元，共计6个单元
    static vtkIdType pts[6][4] = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 0, 1, 5, 4 }, { 1, 2, 6, 5 }, { 2, 3, 7, 6 }, { 3, 0, 4, 7 } };
    // 创建对象
    vtkSmartPointer<vtkPolyData> cube   = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points   = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    // 存储顶点
    for (i = 0; i < 8; i++)
        points->InsertPoint(i, x[i]);
    // 设定单元
    for (i = 0; i < 6; i++)
        polys->InsertNextCell(4, pts[i]);

    // 存储标量值
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    // 设定每个顶点的标量值
    for (i = 0; i < 8; i++)
        scalars->InsertTuple1(i, i * 4);
    // 创建多边形数据
    cube->SetPoints(points);
    // 设定单元类型为多边形
    cube->SetPolys(polys);
    // 设定每个顶点的标量值
    cube->GetPointData()->SetScalars(scalars);
    // 定义颜色映射表
    vtkSmartPointer<vtkLookupTable> pColorTable = vtkSmartPointer<vtkLookupTable>::New();
    // 设置颜色表中的颜色
    pColorTable->SetNumberOfColors(256);
    pColorTable->SetHueRange(0.67, 0.0); // 色调范围从红色到蓝色
    pColorTable->Build();
    // 数据映射
    vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(0, 7);
    cubeMapper->SetLookupTable(pColorTable);
    vtkSmartPointer<vtkActor> cubeActor = vtkSmartPointer<vtkActor>::New();
    cubeActor->SetMapper(cubeMapper);

    vtkSmartPointer<vtkRenderer> renderer   = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);
    renderer->AddActor(cubeActor);
    renderer->SetBackground(1, 1, 1);
    renWin->SetSize(500, 500);
    renWin->Render();
    iren->Start();
    return 0;
}

#endif // TEST18

#ifdef TEST19

// https://gitlab.kitware.com/vtk/vtk/-/blob/v9.2.0/Filters/General/Testing/Cxx/TestDistancePolyDataFilter.cxx

#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkDistancePolyDataFilter.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>
#include <vtkSphereSource.h>

#include <iostream>

int main(int, char*[])
{
    vtkSmartPointer<vtkSphereSource> model1 = vtkSmartPointer<vtkSphereSource>::New();
    model1->SetPhiResolution(100);
    model1->SetThetaResolution(30);
    model1->SetRadius(2);
    model1->SetCenter(0.0, -1, 0.0);

    vtkSmartPointer<vtkSphereSource> model2 = vtkSmartPointer<vtkSphereSource>::New();
    model2->SetPhiResolution(100);
    model2->SetThetaResolution(30);
    model2->SetRadius(1);
    model2->SetCenter(0, 4, 0.0);

    vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter = vtkSmartPointer<vtkDistancePolyDataFilter>::New();

    distanceFilter->SetInputConnection(0, model1->GetOutputPort());
    distanceFilter->SetInputConnection(1, model2->GetOutputPort());
    distanceFilter->Update();

    auto scalars1 = distanceFilter->GetOutput()->GetPointData()->GetScalars();
    auto scalars2 = distanceFilter->GetSecondDistanceOutput()->GetPointData()->GetScalars();

    // 两个vtkPolyData之间的距离，可以为负值
    std::cout << "model 1 range: " << scalars1->GetRange()[0] << ',' << scalars1->GetRange()[1] << '\n';
    std::cout << "model 2 range: " << scalars2->GetRange()[0] << ',' << scalars2->GetRange()[1] << '\n';

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(distanceFilter->GetOutputPort());
    mapper->SetScalarRange(scalars1->GetRange()[0], scalars1->GetRange()[1]);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(distanceFilter->GetOutputPort(1));
    mapper2->SetScalarRange(scalars2->GetRange()[0], scalars2->GetRange()[1]);

    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);

    vtkSmartPointer<vtkScalarBarActor> scalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    // scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetLookupTable(mapper2->GetLookupTable());
    scalarBar->SetTitle("Distance");
    scalarBar->SetNumberOfLabels(5);
    scalarBar->SetTextPad(4);
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renWinInteractor->SetRenderWindow(renWin);

    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->AddActor2D(scalarBar);

    renWin->Render();
    distanceFilter->Print(std::cout);

    renWinInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST19



#ifdef TEST22

// https://blog.csdn.net/minmindianzi/article/details/87071213

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkGlyph3DMapper.h>
#include <vtkPlane.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char*[])
{
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    double origin[]                 = { 0, 0, 0 };
    double normal[]                 = { 0, 0, 1 };
    plane->SetOrigin(origin);
    plane->SetNormal(normal);

    double p[] = { 11.0, 8.3, 5.7 };
    double projected[3];
    plane->ProjectPoint(p, origin, normal, projected);

    char outputMsg[256] = { 0 };
    // sprintf_s(outputMsg, sizeof(outputMsg), "p(%f, %f, %f)", p[0], p[1], p[2]);
    sprintf_s(outputMsg, sizeof(outputMsg), "p(%f, %f, %f)\nprojected(%f, %f, %f)", p[0], p[1], p[2], projected[0], projected[1], projected[2]);

    vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetPosition2(100, 40);
    textActor->GetTextProperty()->SetFontSize(24);
    textActor->GetTextProperty()->SetColor(0, 1, 0);
    textActor->SetInput(outputMsg);

    vtkSmartPointer<vtkSphereSource> pointsSphere = vtkSmartPointer<vtkSphereSource>::New();
    pointsSphere->SetPhiResolution(21);
    pointsSphere->SetThetaResolution(21);
    pointsSphere->SetRadius(.5);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(p);
    points->InsertNextPoint(projected);

    vtkSmartPointer<vtkPolyData> pointsData = vtkSmartPointer<vtkPolyData>::New();
    pointsData->SetPoints(points);

    vtkSmartPointer<vtkGlyph3DMapper> pointsMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
    pointsMapper->SetInputData(pointsData);
    pointsMapper->SetSourceConnection(pointsSphere->GetOutputPort());

    vtkSmartPointer<vtkActor> pointsActor = vtkSmartPointer<vtkActor>::New();
    pointsActor->SetMapper(pointsMapper);
    pointsActor->GetProperty()->SetColor(1, 0, 0);

#define LINE_LEN 10.
    vtkSmartPointer<vtkAxesActor> axesActor = vtkSmartPointer<vtkAxesActor>::New();
    axesActor->SetPosition(0, 0, 0);
    axesActor->SetTotalLength(LINE_LEN, LINE_LEN, LINE_LEN);
    axesActor->SetShaftType(0);
    axesActor->SetAxisLabels(0);
    axesActor->SetCylinderRadius(0.02);

    vtkSmartPointer<vtkRenderer> renderer         = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(600, 600);
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor2D(textActor);
    renderer->AddActor(pointsActor);
    renderer->AddActor(axesActor);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST22

#ifdef TEST23

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <vtkActor.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <iostream>

namespace {

class MyRubberBand : public vtkInteractorStyleRubberBand3D
{
public:
    static MyRubberBand* New();
    vtkTypeMacro(MyRubberBand, vtkInteractorStyleRubberBand3D);

    void OnLeftButtonUp() override
    {
        std::cout << "left button up\n";
        // Superclass::OnLeftButtonUp();
    }

    void OnLeftButtonDown() override
    {
        std::cout << "left button down\n";
        Superclass::OnLeftButtonDown();
    }

    void OnLeftButtonDoubleClick() override
    {
        std::cout << "left button double click\n";
        Superclass::OnLeftButtonDoubleClick();
    }

    void OnMouseMove() override
    {
        Superclass::OnMouseMove();
    }

    void OnChar() override
    {
        auto ret = this->Interactor->GetKeyCode();
        std::cout << "press key: " << ret << '\n';
        Superclass::OnChar();
    }
};

vtkStandardNewMacro(MyRubberBand);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("Double Click");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Render
    renderWindow->SetSize(800, 600);
    renderWindow->Render();

    vtkNew<MyRubberBand> style;
    renderWindowInteractor->SetInteractorStyle(style);

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST23

#ifdef TEST25

#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadricDecimation.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>

#include <array>
#include <iostream>

// 四条线段组成一个正方形
namespace {
std::array<float, 16 * 3> vertices {
    0,
    0,
    0,
    0,
    1,
    0,
    0,
    2,
    0,
    0,
    3,
    0,

    1,
    0,
    0,
    1,
    1,
    0,
    1,
    2,
    0,
    1,
    3,
    0,

    2,
    0,
    0,
    2,
    1,
    0,
    2,
    2,
    0,
    2,
    3,
    0,

    3,
    0,
    0,
    3,
    1,
    0,
    3,
    2,
    0,
    3,
    3,
    0,
};

std::array<long long, 9 * 4> indices1 { 0, 1, 5, 4, 1, 2, 6, 5, 2, 3, 7, 6,

    4, 5, 9, 8, 5, 6, 10, 9, 6, 7, 11, 10,

    8, 9, 13, 12, 9, 10, 14, 13, 10, 11, 15, 14 };

std::array<long long, 18 * 3> indices2 { 0, 1, 5, 0, 5, 4,

    1, 2, 6, 1, 6, 5,

    2, 3, 7, 2, 7, 6,

    4, 5, 9, 4, 9, 8,

    5, 6, 10, 5, 10, 9,

    6, 7, 11, 6, 11, 10,

    8, 9, 13, 8, 13, 12,

    9, 10, 14, 9, 14, 13,

    10, 11, 15, 10, 15, 14 };
} // namespace

vtkSmartPointer<vtkPolyData> GenPolyData(int type)
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    if (type == 3)
    {
        for (size_t i = 0; i < indices2.size(); i += 3)
        {
            cells->InsertNextCell({ indices2[i], indices2[i + 1], indices2[i + 2] });
        }
    }
    else if (type == 4)
    {
        for (size_t i = 0; i < indices1.size(); i += 4)
        {
            cells->InsertNextCell({ indices1[i], indices1[i + 1], indices1[i + 2], indices1[i + 3] });
        }
    }

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    return polyData;
}

int main()
{
    auto trianglePoly = GenPolyData(3);
    auto quad         = GenPolyData(4);

    // 将多边形转为三角形
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputData(quad);
    triangleFilter->Update();

    // 减少网格中的三角形
    // 例如原来网格有十个三角形，经过处理之后只有五个三角形，但是绘制的图像没有变化
    // 输入必须是三角形，如果不是可以用vtkTriangleFilter进行三角化
    vtkNew<vtkQuadricDecimation> decimate;
    decimate->SetInputData(triangleFilter->GetOutput());
    // decimate->SetInputData(trianglePoly);
    decimate->AttributeErrorMetricOn();
    decimate->SetTargetReduction(.9);
    decimate->VolumePreservationOn();
    decimate->Update();

    vtkNew<vtkPolyData> decimated;
    decimated->ShallowCopy(decimate->GetOutput());

    std::cout << "after number of cells: " << decimate->GetOutput()->GetNumberOfCells() << '\n';
    std::cout << "after number of polys: " << decimate->GetOutput()->GetNumberOfPolys() << '\n';
    std::cout << "after number of points: " << decimate->GetOutput()->GetNumberOfPoints() << '\n';
    std::cout << "-----------------------------\n";
    std::cout << "before number of cells: " << trianglePoly->GetNumberOfCells() << '\n';
    std::cout << "before number of polys: " << trianglePoly->GetNumberOfPolys() << '\n';
    std::cout << "before number of points: " << trianglePoly->GetNumberOfPoints() << '\n';

    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(decimated);

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);
    cubeActor->GetProperty()->SetRepresentationToWireframe();

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST25

#ifdef TEST26

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkJPEGReader.h>
#include <vtkLight.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

int main()
{
    // 加载图片
    // vtkSmartPointer<vtkBMPReader>bmpReader = vtkSmartPointer<vtkBMPReader>::New();
    // bmpReader->SetFileName("test.bmp");
    vtkSmartPointer<vtkJPEGReader> jpgReader = vtkSmartPointer<vtkJPEGReader>::New();
    jpgReader->SetFileName("resource/test1.jpg");
    // 生成纹理
    vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
    texture->SetInputConnection(jpgReader->GetOutputPort());
    texture->InterpolateOn();

    // 生成一个中心在渲染场景原点的柱体
    vtkSmartPointer<vtkCylinderSource> cylinder = vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetHeight(3.0);
    cylinder->SetRadius(1.0);
    cylinder->SetResolution(10);

    // mapper 将输入的数据转换为几何图元(点、线、多边形)进行渲染
    vtkSmartPointer<vtkPolyDataMapper> cylinderMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // actor 派生自vtkProp类
    vtkSmartPointer<vtkActor> cylinderActor = vtkSmartPointer<vtkActor>::New();
    cylinderActor->SetMapper(cylinderMapper);

    // 使用纹理
    cylinderActor->SetTexture(texture);

    // 设置图像颜色
    // cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

    // render
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(cylinderActor);
    renderer->SetBackground(.1, .2, .3);
    // renderer->SetBackground2(1, 0, 0);

    // 光照
    vtkSmartPointer<vtkLight> myLight = vtkSmartPointer<vtkLight>::New();
    myLight->SetColor(0, 1, 0);
    myLight->SetPosition(0, 0, 1);
    myLight->SetFocalPoint(renderer->GetActiveCamera()->GetFocalPoint());
    renderer->AddLight(myLight);

    vtkSmartPointer<vtkLight> myLight2 = vtkSmartPointer<vtkLight>::New();
    myLight2->SetColor(0, 0, 1);
    myLight2->SetPosition(0, 0, -1);
    myLight2->SetFocalPoint(renderer->GetActiveCamera()->GetFocalPoint());
    renderer->AddLight(myLight2);

    // 相机
    vtkSmartPointer<vtkCamera> myCamera = vtkSmartPointer<vtkCamera>::New();
    myCamera->SetClippingRange(0.0475, 2.3786); // 前后裁剪平面
    myCamera->SetFocalPoint(0.0573, 0.2134, 0.0523);
    myCamera->SetPosition(0, 0, 5);
    myCamera->ComputeViewPlaneNormal();
    myCamera->SetViewUp(-0.2234, 0.9983, 0.0345);
    renderer->SetActiveCamera(myCamera);
    renderer->ResetCamera();

    // window
    vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();

    // interactor
    vtkSmartPointer<vtkRenderWindowInteractor> rWinInt = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    rWinInt->SetRenderWindow(window);

    window->AddRenderer(renderer);
    window->Render();
    rWinInt->Start();

    return EXIT_SUCCESS;
}

#endif // TEST26



#ifdef TEST33

#include <vtkAlgorithmOutput.h>
#include <vtkCellArray.h>
#include <vtkMaskFields.h>
#include <vtkMaskPoints.h>
#include <vtkMaskPointsFilter.h>
#include <vtkMaskPolyData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <array>

namespace {
std::array<float, 8 * 3> vertices { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0,

    0, 2, 0, 1, 2, 0, 1, 3, 0, 0, 3, 0 };

std::array<long long, 7 * 2> indices { 0, 1, 1, 2, 2, 3, 3, 4,

    4, 5, 5, 6, 6, 7 };
} // namespace

int main()
{
    // vtkMaskPoints 对dataset的vtkPoints数据进行筛选
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> lines;
        vtkNew<vtkCellArray> verts;

        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        for (size_t i = 0; i < indices.size(); i += 2)
        {
            lines->InsertNextCell({ indices[i], indices[i + 1] });
        }

        for (size_t i = 0; i < 8; i++)
        {
            vtkIdType pts = i;
            verts->InsertNextCell(1, &pts);
        }

        polyData->SetPoints(points);
        // polyData->SetLines(lines);
        polyData->SetVerts(verts);

        std::cout << "num points:\t" << polyData->GetNumberOfPoints() << '\n';
        std::cout << "num cells:\t" << polyData->GetNumberOfCells() << '\n';
        std::cout << "num lines:\t" << polyData->GetNumberOfLines() << '\n';
        std::cout << "num verts:\t" << polyData->GetNumberOfVerts() << '\n';

        for (size_t i = 0; i < polyData->GetNumberOfPoints(); i++)
        {
            auto pt = polyData->GetPoints()->GetPoint(i);
            std::cout << "point " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
        }

        std::cout << "------------------------------------\n";

        // 筛选数据
        vtkNew<vtkMaskPoints> maskPoints;
        maskPoints->SetInputData(polyData);
        maskPoints->SetOnRatio(3); // 提取第3n个顶点，n从0开始，3n小于顶点个数
        maskPoints->Update();

        if (auto outPolyData = maskPoints->GetOutput())
        {
            std::cout << "num points:\t" << outPolyData->GetNumberOfPoints() << '\n';
            std::cout << "num cells:\t" << outPolyData->GetNumberOfCells() << '\n';
            std::cout << "num lines:\t" << outPolyData->GetNumberOfLines() << '\n';
            std::cout << "num verts:\t" << outPolyData->GetNumberOfVerts() << '\n';

            for (size_t i = 0; i < outPolyData->GetNumberOfPoints(); i++)
            {
                auto pt = outPolyData->GetPoints()->GetPoint(i);
                std::cout << "point " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
            }
        }

        std::cout << "------------------------------------\n";

        // 获取源数据
        if (auto inPolyData = vtkPolyData::SafeDownCast(maskPoints->GetInput()))
        {
            std::cout << "num points:\t" << inPolyData->GetNumberOfPoints() << '\n';
            std::cout << "num cells:\t" << inPolyData->GetNumberOfCells() << '\n';
            std::cout << "num lines:\t" << inPolyData->GetNumberOfLines() << '\n';
            std::cout << "num verts:\t" << inPolyData->GetNumberOfVerts() << '\n';

            for (size_t i = 0; i < inPolyData->GetNumberOfPoints(); i++)
            {
                auto pt = inPolyData->GetPoints()->GetPoint(i);
                std::cout << "point " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
            }
        }
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    // vtkMaskPolyData 对dataset的单元数据（cell）进行筛选
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> lines;

        for (size_t i = 0; i < vertices.size(); i += 3)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
        }

        for (size_t i = 0; i < indices.size(); i += 2)
        {
            lines->InsertNextCell({ indices[i], indices[i + 1] });
        }

        polyData->SetPoints(points);
        polyData->SetLines(lines);

        std::cout << "num points:\t" << polyData->GetNumberOfPoints() << '\n';
        std::cout << "num cells:\t" << polyData->GetNumberOfCells() << '\n';
        std::cout << "num lines:\t" << polyData->GetNumberOfLines() << '\n';
        std::cout << "num verts:\t" << polyData->GetNumberOfVerts() << '\n';

        std::cout << "------------------------------------\n";

        vtkNew<vtkMaskPolyData> maskPolyData;
        maskPolyData->SetInputData(polyData);
        maskPolyData->SetOnRatio(2);
        maskPolyData->Update();

        if (auto outPolyData = maskPolyData->GetOutput())
        {
            std::cout << "num points:\t" << outPolyData->GetNumberOfPoints() << '\n';
            std::cout << "num cells:\t" << outPolyData->GetNumberOfCells() << '\n';
            std::cout << "num lines:\t" << outPolyData->GetNumberOfLines() << '\n';
            std::cout << "num verts:\t" << outPolyData->GetNumberOfVerts() << '\n';
        }
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    // vtkMaskFields
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkMaskFields> maskArrays;
        maskArrays->SetInputData(polyData);
        maskArrays->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, "Normals");
        maskArrays->CopyFieldOff(1, "");
        maskArrays->CopyAttributesOn();
        maskArrays->Update();
    }

    return 0;
}

#endif // TEST33

#ifdef TEST38

#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // Create a table with some points in it.
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> arrX;
    arrX->SetName("X Axis");
    table->AddColumn(arrX);

    vtkNew<vtkFloatArray> arrC;
    arrC->SetName("Cosine");
    table->AddColumn(arrC);

    vtkNew<vtkFloatArray> arrS;
    arrS->SetName("Sine");
    table->AddColumn(arrS);

    // Fill in the table with some example values.
    int numPoints = 69;
    float inc     = 7.5 / (numPoints - 1);
    table->SetNumberOfRows(numPoints);
    for (int i = 0; i < numPoints; ++i)
    {
        table->SetValue(i, 0, i * inc);
        table->SetValue(i, 1, cos(i * inc));
        table->SetValue(i, 2, sin(i * inc));
    }

    // Set up the view
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetWindowName("LinePlot");
    view->GetRenderer()->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Add multiple line plots, setting the colors etc.
    vtkNew<vtkChartXY> chart;
    view->GetScene()->AddItem(chart);
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(1.0);
    line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 2);
    line->SetColor(255, 0, 0, 255);
    line->SetWidth(5.0);
    line->Update();
    // For dotted line, the line type can be from 2 to 5 for different dash/dot
    // patterns (see enum in vtkPen containing DASH_LINE, value 2):
    // #ifndef WIN32
    //   line->GetPen()->SetLineType(vtkPen::DASH_LINE);
    // #endif
    // (ifdef-ed out on Windows because DASH_LINE does not work on Windows
    //  machines with built-in Intel HD graphics card...)

    // view->GetRenderWindow()->SetMultiSamples(0);

    // Start interactor
    view->GetRenderWindow()->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

#endif // TEST38

#ifdef TEST43

//
// This example demonstrates how to access the source object
// (e.g. vtkSphereSource) from the actor reversely.
//
// some standard vtk headers
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

// additionally needed vtk header for this example
#include <vtkAlgorithmOutput.h>

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // source
    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(0.5);
    // mapper
    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    // actor
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // renderer
    vtkNew<vtkRenderer> ren1;
    ren1->SetBackground(colors->GetColor3d("CadetBlue").GetData());

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("ReverseAccess");

    // add actor to the renderer
    ren1->AddActor(sphereActor);

    //
    // Now we retrieve the source object from vtkActor reversely,
    // meaning we don't use the spheresource object we instantiated
    // above directly,
    // instead we retrieve a reference to the spheresource through the
    // actor.
    // An advantage of this concept might be that we don't need to
    // maintain the source object anymore
    // in a more complex application.
    // To demonstrate that we can modify properties of the spheresource
    // through this reference
    // beside changing some properties of the actor (in this example we
    // change actor's x-position),
    // we change the radius of the spheresource as well.
    //
    // next two lines are the core lines for reverse access
    //
    vtkSmartPointer<vtkAlgorithm> algorithm = sphereActor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    auto srcReference                       = dynamic_cast<vtkSphereSource*>(algorithm.GetPointer());

    float origRadius = srcReference->GetRadius();
    for (int i = 0; i < 360; ++i)
    {
        // change radius of the spheresource
        srcReference->SetRadius(origRadius * (1 + sin((float)i / 180.0 * vtkMath::Pi())));
        // change x-position of the actor
        sphereActor->SetPosition(sin((float)i / 45.0 * vtkMath::Pi()) * 0.5, 0, 0);
        renWin->Render();
    }

    //
    // Thanks to the usage of vtkSmartPointer there is no explicit need
    // to free any objects at this point.
    //
    return EXIT_SUCCESS;
}

#endif // TEST43

#ifdef TEST44

#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkIdList.h>
#include <vtkMath.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOctreePointLocator.h>
#include <vtkPlot.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTable.h>
#include <vtkTimerLog.h>
#include <vtkXMLPolyDataReader.h>

#include <time.h>
#include <vector>

namespace {
// void RandomPointInBounds(vtkPolyData* polydata, double p[3]);
void RandomPointInBounds(vtkPolyData* polydata, double p[3], vtkMinimalStandardRandomSequence* rng);

double TimeOctree(vtkPolyData* polydata, int maxPoints, int numberOfTrials, vtkMinimalStandardRandomSequence* rng);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> reader;
    reader->SetThetaResolution(30);
    reader->SetPhiResolution(30);
    reader->Update();

    vtkNew<vtkMinimalStandardRandomSequence> rng;
    rng->SetSeed(8775070);
    // rng->SetSeed(0);

    std::cout << "Timing octree..." << std::endl;
    std::vector<std::pair<int, double>> results;
    int numberOfTrials = 1000;
    for (int i = 1; i < 100; i++)
    {
        double t = TimeOctree(reader->GetOutput(), i, numberOfTrials, rng);
        std::pair<int, double> result(i, t);
        results.push_back(result);
    }

    // Create a table with some points in it
    vtkNew<vtkTable> table;

    vtkNew<vtkFloatArray> maxPointsPerRegion;
    maxPointsPerRegion->SetName("MaxPointsPerRegion");
    table->AddColumn(maxPointsPerRegion);

    vtkNew<vtkFloatArray> runtime;
    runtime->SetName("Run time");
    table->AddColumn(runtime);

    // Fill in the table with some example values
    size_t numPoints = results.size();
    table->SetNumberOfRows(static_cast<vtkIdType>(numPoints));
    for (size_t i = 0; i < numPoints; ++i)
    {
        table->SetValue(static_cast<vtkIdType>(i), 0, results[i].first);
        table->SetValue(static_cast<vtkIdType>(i), 1, results[i].second);
        std::cout << "Put " << results[i].first << " " << results[i].second << " in the table." << std::endl;
    }

    // Set up the view
    vtkNew<vtkContextView> view;
    view->GetRenderer()->SetBackground(colors->GetColor3d("White").GetData());
    view->GetRenderWindow()->SetWindowName("OctreeTimingDemo");

    // Add multiple line plots, setting the colors etc
    vtkNew<vtkChartXY> chart;
    view->GetScene()->AddItem(chart);
    vtkPlot* line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    auto lineColor = colors->HTMLColorToRGBA("Lime").GetData();
    line->SetColor(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);
    line->SetWidth(3.0);
    line->GetXAxis()->SetTitle("Max Points Per Region");
    line->GetYAxis()->SetTitle("Run time");

    // Set up an interactor and start
    view->Render();
    view->GetInteractor()->SetRenderWindow(view->GetRenderWindow());
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return EXIT_SUCCESS;
}

namespace {
void RandomPointInBounds(vtkPolyData* polydata, double p[3], vtkMinimalStandardRandomSequence* rng)
{
    double bounds[6];
    polydata->GetBounds(bounds);

    for (auto i = 0; i < 3; ++i)
    {
        p[i] = bounds[i * 2] + (bounds[i * 2 + 1] - bounds[i * 2]) * rng->GetRangeValue(0.0, 1.0);
        rng->Next();
    }
}

double TimeOctree(vtkPolyData* polydata, int maxPoints, int numberOfTrials, vtkMinimalStandardRandomSequence* rng)
{
    vtkNew<vtkTimerLog> timer;
    timer->StartTimer();

    // Create the tree
    vtkNew<vtkOctreePointLocator> octree;
    octree->SetDataSet(polydata);
    octree->SetMaximumPointsPerRegion(maxPoints);
    octree->BuildLocator();

    std::cout << "With maxPoints = " << maxPoints << " there are " << octree->GetNumberOfLeafNodes() << " leaf nodes." << std::endl;

    for (int i = 0; i < numberOfTrials; i++)
    {
        double p[3];
        RandomPointInBounds(polydata, p, rng);
        // vtkIdType iD = octree->FindClosestPoint(p);
        octree->FindClosestPoint(p);
    }

    timer->StopTimer();

    std::cout << "Octree took " << timer->GetElapsedTime() << std::endl;

    return timer->GetElapsedTime();
}

} // namespace

#endif // TEST44

#ifdef TEST45

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

#define Two

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, i, 0);
    }

    vtkSmartPointer<vtkCellArray> cell_line = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 9; ++i)
    {
        cell_line->InsertNextCell({ i, i + 1 });
    }

    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    for (int i = 0; i < 10; ++i)
    {
        scalars->InsertNextValue(float(i));
    }

    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->SetPoints(points);
    poly->SetLines(cell_line);
    poly->GetPointData()->SetScalars(scalars);

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(
        0.67, 0.0); // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->SetRampToSQRT();
    hueLut->Build();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    mapper->SetScalarRange(0, 9); // 设置标量值的范围
    mapper->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetLineWidth(5);
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

#ifdef Two

    // 左右两个窗口用来对比
    double leftViewport[4]  = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer);
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(colors->GetColor3d("Peru").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer);
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(colors->GetColor3d("CornflowerBlue").GetData());
    leftRenderer->AddActor(actor);

    // 重新设置scalar
    vtkSmartPointer<vtkFloatArray> scalars2 = vtkSmartPointer<vtkFloatArray>::New();
    for (int i = 9; i > -1; --i)
    {
        scalars2->InsertNextValue(float(i));
    }

    auto poly1 = actor->GetMapper()->GetInput();
    // vtkSmartPointer<vtkDataSet> poly2 = vtkSmartPointer<vtkDataSet>::New();
    vtkSmartPointer<vtkPolyData> poly2 = vtkSmartPointer<vtkPolyData>::New();
    poly2->DeepCopy(poly1);

    poly2->GetPointData()->SetScalars(scalars2);

    // vtkSmartPointer<vtkDataSetMapper> mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputData(poly2);
    mapper2->SetScalarRange(0, 9); // 设置标量值的范围
    mapper2->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->GetProperty()->SetLineWidth(5);
    actor2->SetMapper(mapper2);

    rightRenderer->AddActor(actor2);
#else
    renderer->AddActor(actor);
#endif

    // vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    // auto p = dynamic_cast<vtkGlyph3D*>(algorithm.GetPointer());

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->SetSize(1200, 800);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST45

#ifdef TEST51

#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <array>
#include <vtkProperty.h>

#include "vtkLineSource.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkSmartPointer.h"

#include "vtkInteractorStyleRubberBand2D.h"

#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageRectilinearWipe.h>
#include <vtkJPEGReader.h>
#include <vtkRectilinearWipeRepresentation.h>
#include <vtkRectilinearWipeWidget.h>
#include <vtkSmartPointer.h>

#include <vtkBMPReader.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkObjectFactory.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkResliceImageViewer.h>

class MyRubberBand : public vtkInteractorStyleRubberBand3D
{
public:
    static MyRubberBand* New();
    vtkTypeMacro(MyRubberBand, vtkInteractorStyleRubberBand3D);

    virtual void OnLeftButtonDown()
    {
        vtkInteractorStyleRubberBand3D::OnLeftButtonDown(); // 必须有这一句
        // if (mExistActor)
        //{
        //     return;
        // }
        startPosition[0] = this->GetInteractor()->GetEventPosition()[0];
        startPosition[1] = this->GetInteractor()->GetEventPosition()[1];
        startPosition[2] = 0;

        std::cout << "startPosition: " << startPosition[0] << "  " << startPosition[1] << std::endl;
    }

    virtual void OnLeftButtonUp()
    {
        vtkInteractorStyleRubberBand3D::OnLeftButtonUp(); // 必须有这一句

        // if (mExistActor)
        //{
        // return;
        //}

        endPosition[0] = this->GetInteractor()->GetEventPosition()[0];
        endPosition[1] = this->GetInteractor()->GetEventPosition()[1];
        endPosition[2] = 0;
        std::cout << "endPosition: " << endPosition[0] << "  " << endPosition[1] << std::endl;

        this->viewer->GetRenderer()->SetDisplayPoint(startPosition);
        this->viewer->GetRenderer()->DisplayToView();
        this->viewer->GetRenderer()->ViewToWorld();

        double start[4];
        this->viewer->GetRenderer()->GetWorldPoint(start);

        this->viewer->GetRenderer()->SetDisplayPoint(endPosition);
        this->viewer->GetRenderer()->DisplayToView();
        this->viewer->GetRenderer()->ViewToWorld();

        double end[4];
        this->viewer->GetRenderer()->GetWorldPoint(end);

        double point1[3];
        double point2[3];
        double point3[3];
        double point4[3];

        double left[2];
        double right[2];

        left[0] = start[0] <= end[0] ? start[0] : end[0];
        left[1] = start[1] <= end[1] ? start[1] : end[1];

        right[0] = start[0] > end[0] ? start[0] : end[0];
        right[1] = start[1] > end[1] ? start[1] : end[1];

        point1[0] = left[0];
        point1[1] = left[1];
        point1[2] = 0;
        point2[0] = left[0];
        point2[1] = right[1];
        point2[2] = 0;
        point3[0] = right[0];
        point3[1] = right[1];
        point3[2] = 0;
        point4[0] = right[0];
        point4[1] = left[1];
        point4[2] = 0;

        this->SetLine(point1, point2);
        this->SetLine(point2, point3);
        this->SetLine(point3, point4);
        this->SetLine(point4, point1);
    }

    void SetLine(double point1[], double point2[])
    {
        vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
        lineSource->SetPoint1(point1);
        lineSource->SetPoint2(point2);
        lineSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(lineSource->GetOutputPort());
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetLineWidth(2);
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        this->viewer->GetRenderer()->AddActor2D(actor);
        this->viewer->Render();
        mExistActor = true;
    }

    vtkSmartPointer<vtkImageViewer2> viewer;

    // vtkSmartPointer<vtkResliceImageViewer>  viewer;
    void SetRender(vtkImageViewer2* render)
    {
        viewer = render;
    }

protected:
    MyRubberBand()
    {
        // viewer = vtkSmartPointer<vtkImageViewer2>::New();
        // viewer->SetupInteractor(this->GetInteractor());
        mExistActor = false;
    }

    //
private:
    double startPosition[3];
    double endPosition[3];

    bool mExistActor;
    // vtkImageViewer2 * mRender;
};

vtkStandardNewMacro(MyRubberBand);

int main(int, char*[])
{
    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

    // Set the background color.
    std::array<unsigned char, 4> bkg { { 26, 51, 77, 255 } };
    colors->SetColor("BkgColor", bkg.data());

    // Create a renderer, render window and interactor
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    // renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkBMPReader> reader1 = vtkSmartPointer<vtkBMPReader>::New();
    reader1->SetFileName("./32.bmp");
    reader1->Update();

    // Add the actors to the scene

    renderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
    vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();

    viewer->SetInputConnection(reader1->GetOutputPort());

    viewer->SetRenderer(renderer);
    viewer->SetSize(640, 480);
    viewer->SetColorLevel(500);
    viewer->SetColorWindow(2000);
    viewer->SetSlice(40);
    viewer->SetSliceOrientationToXY();
    viewer->Render();
    viewer->GetRenderer()->SetBackground(1, 1, 1);
    viewer->GetRenderWindow()->SetWindowName("ImageViewer2D");
    vtkSmartPointer<vtkRenderWindowInteractor> rwi = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    viewer->SetupInteractor(rwi);
    vtkSmartPointer<MyRubberBand> g_style = vtkSmartPointer<MyRubberBand>::New();

    g_style->SetInteractor(rwi);

    rwi->SetInteractorStyle(g_style);

    g_style->SetRender(viewer);

    // 设置交互属性

    rwi->Start();
    return EXIT_SUCCESS;
}

#endif // TEST51

#ifdef TEST58

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <iostream>

class CallBack : public vtkCommand
{
public:
    static CallBack* New()
    {
        return new CallBack;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        vtkWindow* window = static_cast<vtkWindow*>(caller);
        window->Render();

        std::cout << "resize\n";
    }
};

int main(int, char*[])
{
    vtkNew<vtkCubeSource> cube;
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置相机焦点

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小
    auto cb = CallBack::New();
    renWin->AddObserver(vtkCommand::WindowResizeEvent, cb);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST58

#ifdef TEST59

#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkLineRepresentation.h>
#include <vtkLineWidget2.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

// This does the actual work.
// Callback for the interaction
class vtkLineCallback : public vtkCommand
{
public:
    static vtkLineCallback* New()
    {
        return new vtkLineCallback;
    }

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {

        vtkLineWidget2* lineWidget = reinterpret_cast<vtkLineWidget2*>(caller);

        // Get the actual box coordinates of the line
        vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
        static_cast<vtkLineRepresentation*>(lineWidget->GetRepresentation())->GetPolyData(polydata);

        // Display one of the points, just so we know it's working
        double p[3];
        polydata->GetPoint(0, p);
        std::cout << "P: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }

    vtkLineCallback()
    {
    }
};

int main(int, char*[])
{
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->Update();

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // A renderer and render window
    vtkSmartPointer<vtkRenderer> renderer         = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    // renderer->AddActor(actor);

    // An interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkLineWidget2> lineWidget = vtkSmartPointer<vtkLineWidget2>::New();
    lineWidget->SetInteractor(renderWindowInteractor);
    lineWidget->CreateDefaultRepresentation();

    // You could do this if you want to set properties at this point:
    // vtkSmartPointer<vtkLineRepresentation> lineRepresentation =
    // vtkSmartPointer<vtkLineRepresentation>::New();
    // lineWidget->SetRepresentation(lineRepresentation);

    vtkSmartPointer<vtkLineCallback> lineCallback = vtkSmartPointer<vtkLineCallback>::New();

    lineWidget->AddObserver(vtkCommand::InteractionEvent, lineCallback);

    // Render
    renderWindow->Render();

    renderWindowInteractor->Initialize();
    renderWindow->Render();
    lineWidget->On();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST59

#ifdef TEST60

#include "vtkNew.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkTextRenderer.h"
#include "vtkUnicodeString.h"
#include <iostream>
#include <string>
#include <windows.h>

int main(int argc, char* argv[])
{
    // if (argc < 2)
    //{
    //     cerr << "Missing font filename." << endl;
    //     return EXIT_FAILURE;
    // }

    // std::string uncodeFontFile(argv[1]);

    vtkSmartPointer<vtkTextRenderer> tren = vtkSmartPointer<vtkTextRenderer>::New();
    if (tren == nullptr)
    {
        std::cerr << "Object factory cannot find vtkTextRenderer override.\n";
        return EXIT_FAILURE;
    }

    if (strcmp(tren->GetClassName(), "vtkMathTextFreeTypeTextRenderer") != 0)
    {
        std::cerr << "Object factory returning unrecognized vtkTextRenderer "
                     "override: "
                  << tren->GetClassName() << std::endl;
        return EXIT_FAILURE;
    }

    std::string str                      = "Sample multiline\ntext rendered\nusing FreeTypeTools.";
    vtkSmartPointer<vtkTextActor> actor1 = vtkSmartPointer<vtkTextActor>::New();
    actor1->GetTextProperty()->SetFontSize(20);
    actor1->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
    actor1->GetTextProperty()->SetJustificationToLeft();
    actor1->GetTextProperty()->SetVerticalJustificationToTop();
    actor1->GetTextProperty()->SetFontFamilyToTimes();
    actor1->SetInput(str.c_str());
    actor1->SetPosition(10, 590);

    vtkSmartPointer<vtkTextActor> actor2 = vtkSmartPointer<vtkTextActor>::New();
    actor2->GetTextProperty()->SetFontSize(20);
    actor2->GetTextProperty()->SetColor(0.0, 1.0, 0.0);
    actor2->GetTextProperty()->SetJustificationToRight();
    actor2->GetTextProperty()->SetVerticalJustificationToTop();
    actor2->GetTextProperty()->SetFontFamilyToCourier();
    actor2->SetInput(str.c_str());
    actor2->SetPosition(590, 590);

    vtkSmartPointer<vtkTextActor> actor3 = vtkSmartPointer<vtkTextActor>::New();
    actor3->GetTextProperty()->SetFontSize(20);
    actor3->GetTextProperty()->SetColor(0.0, 0.0, 1.0);
    actor3->GetTextProperty()->SetJustificationToLeft();
    actor3->GetTextProperty()->SetVerticalJustificationToBottom();
    actor3->GetTextProperty()->SetItalic(1);
    actor3->SetInput(str.c_str());
    actor3->SetPosition(10, 10);

    vtkSmartPointer<vtkTextActor> actor4 = vtkSmartPointer<vtkTextActor>::New();
    actor4->GetTextProperty()->SetFontSize(20);
    actor4->GetTextProperty()->SetColor(0.3, 0.4, 0.5);
    actor4->GetTextProperty()->SetJustificationToRight();
    actor4->GetTextProperty()->SetVerticalJustificationToBottom();
    actor4->GetTextProperty()->SetBold(1);
    actor4->GetTextProperty()->SetShadow(1);
    actor4->GetTextProperty()->SetShadowOffset(-3, 2);
    actor4->SetInput(str.c_str());
    actor4->SetPosition(590, 10);

    vtkSmartPointer<vtkTextActor> actor5 = vtkSmartPointer<vtkTextActor>::New();
    actor5->GetTextProperty()->SetFontSize(20);
    actor5->GetTextProperty()->SetColor(1.0, 1.0, 0.0);
    actor5->GetTextProperty()->SetJustificationToCentered();
    actor5->GetTextProperty()->SetVerticalJustificationToCentered();
    actor5->GetTextProperty()->SetBold(1);
    actor5->GetTextProperty()->SetItalic(1);
    actor5->GetTextProperty()->SetShadow(1);
    actor5->GetTextProperty()->SetShadowOffset(5, -8);
    actor5->SetInput(str.c_str());
    actor5->SetPosition(300, 300);

    vtkSmartPointer<vtkTextActor> actor6 = vtkSmartPointer<vtkTextActor>::New();
    actor6->GetTextProperty()->SetFontSize(16);
    actor6->GetTextProperty()->SetColor(1.0, 0.5, 0.2);
    actor6->GetTextProperty()->SetJustificationToCentered();
    actor6->GetTextProperty()->SetVerticalJustificationToCentered();
    actor6->GetTextProperty()->SetOrientation(45);
    actor6->SetInput(str.c_str());
    actor6->SetPosition(300, 450);

    vtkSmartPointer<vtkTextActor> actor7 = vtkSmartPointer<vtkTextActor>::New();
    actor7->GetTextProperty()->SetFontSize(16);
    actor7->GetTextProperty()->SetColor(0.5, 0.2, 1.0);
    actor7->GetTextProperty()->SetJustificationToLeft();
    actor7->GetTextProperty()->SetVerticalJustificationToCentered();
    actor7->GetTextProperty()->SetOrientation(45);
    actor7->SetInput(str.c_str());
    actor7->SetPosition(100, 156);

    vtkSmartPointer<vtkTextActor> actor8 = vtkSmartPointer<vtkTextActor>::New();
    actor8->GetTextProperty()->SetFontSize(16);
    actor8->GetTextProperty()->SetColor(0.8, 1.0, 0.3);
    actor8->GetTextProperty()->SetJustificationToRight();
    actor8->GetTextProperty()->SetVerticalJustificationToCentered();
    actor8->GetTextProperty()->SetOrientation(45);
    actor8->SetInput(str.c_str());
    actor8->SetPosition(500, 249);

    // Mathtext tests

    // Test that escaped "$" are passed through to freetype:
    vtkSmartPointer<vtkTextActor> actor9 = vtkSmartPointer<vtkTextActor>::New();
    actor9->GetTextProperty()->SetFontSize(12);
    actor9->GetTextProperty()->SetColor(0.2, 0.5, 1.0);
    actor9->SetInput("Escaped dollar signs:\n\\$10, \\$20");
    actor9->SetPosition(100, 450);

    vtkSmartPointer<vtkTextActor> actor10 = vtkSmartPointer<vtkTextActor>::New();
    actor10->GetTextProperty()->SetFontSize(16);
    actor10->GetTextProperty()->SetColor(0.5, 0.2, 1.0);
    actor10->GetTextProperty()->SetJustificationToRight();
    actor10->GetTextProperty()->SetOrientation(45);
    actor10->SetInput("Test MathText $\\int_0^\\infty\\frac{2\\pi}"
                      "{x - \\frac{z}{4}}\\,dx$");
    actor10->SetPosition(588, 433);

    // Invalid latex markup -- should fallback to freetype.
    vtkSmartPointer<vtkTextActor> actor11 = vtkSmartPointer<vtkTextActor>::New();
    actor11->GetTextProperty()->SetFontSize(15);
    actor11->GetTextProperty()->SetColor(1.0, 0.5, 0.2);
    actor11->SetInput("Test FreeType fallback:\n$\\asdf$");
    actor11->SetPosition(10, 350);

    // Both $...$ and \\$
    vtkSmartPointer<vtkTextActor> actor12 = vtkSmartPointer<vtkTextActor>::New();
    actor12->GetTextProperty()->SetFontSize(18);
    actor12->GetTextProperty()->SetColor(0.0, 1.0, 0.7);
    actor12->SetInput("Test MathText '\\$' $\\$\\sqrt[3]{8}$");
    actor12->SetPosition(10, 300);

    // $...$ without other text.
    vtkSmartPointer<vtkTextActor> actor13 = vtkSmartPointer<vtkTextActor>::New();
    actor13->GetTextProperty()->SetFontSize(18);
    actor13->GetTextProperty()->SetColor(0.2, 1.0, 1.0);
    actor13->SetInput("$A = \\pi r^2$");
    actor13->SetPosition(10, 250);

    // Numbers, using courier, Text that gets 'cut off'
    vtkSmartPointer<vtkTextActor> actor14 = vtkSmartPointer<vtkTextActor>::New();
    actor14->GetTextProperty()->SetFontSize(21);
    actor14->GetTextProperty()->SetColor(1.0, 0.0, 0.0);
    actor14->GetTextProperty()->SetJustificationToCentered();
    actor14->GetTextProperty()->SetVerticalJustificationToCentered();
    actor14->GetTextProperty()->SetBold(1);
    actor14->GetTextProperty()->SetItalic(1);
    actor14->GetTextProperty()->SetFontFamilyToCourier();
    actor14->SetInput("4.0");
    actor14->SetPosition(500, 400);

    // UTF-8 freetype handling:
    vtkSmartPointer<vtkTextActor> actor15 = vtkSmartPointer<vtkTextActor>::New();
    actor15->GetTextProperty()->SetFontFamily(VTK_FONT_FILE);
    // actor15->GetTextProperty()->SetFontFile(uncodeFontFile.c_str());
    actor15->GetTextProperty()->SetJustificationToCentered();
    actor15->GetTextProperty()->SetVerticalJustificationToCentered();
    actor15->GetTextProperty()->SetFontSize(18);
    actor15->GetTextProperty()->SetColor(0.0, 1.0, 0.7);
    actor15->SetInput(u8"UTF-8 FreeType: 显示中文\n换行显示");
    actor15->SetPosition(300, 110);

    // Boring rendering setup....

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren->SetBackground(0.1, 0.1, 0.1);
    vtkSmartPointer<vtkRenderWindow> win = vtkSmartPointer<vtkRenderWindow>::New();
    win->SetSize(600, 600);
    win->AddRenderer(ren);
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(win);

    ren->AddActor(actor1);
    ren->AddActor(actor2);
    ren->AddActor(actor3);
    ren->AddActor(actor4);
    ren->AddActor(actor5);
    ren->AddActor(actor6);
    ren->AddActor(actor7);
    ren->AddActor(actor8);
    ren->AddActor(actor9);
    ren->AddActor(actor10);
    ren->AddActor(actor11);
    ren->AddActor(actor12);
    ren->AddActor(actor13);
    ren->AddActor(actor14);
    ren->AddActor(actor15);

    win->SetMultiSamples(0);
    win->Render();
    win->GetInteractor()->Initialize();
    win->GetInteractor()->Start();

    return EXIT_SUCCESS;
}
#endif // TEST60

#ifdef TEST61

// 回调函数传数据 vtkCallbackCommand::SetClientData() https://www.freesion.com/article/97861261334/
// 给交互器style设置observer https://kitware.github.io/vtk-examples/site/Cxx/Interaction/RubberBand2DObserver/
// void* callData不需要主动传数据

#define STYLE

#ifdef STYLE

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

void SelectionChangedCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

}

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RubberBand2DObserver");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderWindow->Render();

    vtkNew<vtkCallbackCommand> selectionChangedCallback;
    selectionChangedCallback->SetCallback(SelectionChangedCallbackFunction);

    vtkNew<vtkInteractorStyleRubberBand2D> style;
    style->AddObserver(vtkCommand::SelectionChangedEvent, selectionChangedCallback);
    // style->AddObserver(vtkCommand::SelectionChangedEvent, [](vtkObject* caller, unsigned long, void*, void*) {});

    renderWindowInteractor->SetInteractorStyle(style);

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {

void SelectionChangedCallbackFunction(
    vtkObject* vtkNotUsed(caller), long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* callData)
{
    std::cout << "SelectionChanged callback" << std::endl;

    unsigned int* rect = reinterpret_cast<unsigned int*>(callData);
    unsigned int pos1X = rect[0];
    unsigned int pos1Y = rect[1];
    unsigned int pos2X = rect[2];
    unsigned int pos2Y = rect[3];

    std::cout << "Start x: " << pos1X << " Start y: " << pos1Y << " End x: " << pos2X << " End y: " << pos2Y << std::endl;
}

} // namespace

#else

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <vtkActor.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

class MyRubberBand : public vtkInteractorStyleRubberBand2D
{
public:
    static MyRubberBand* New();
    vtkTypeMacro(MyRubberBand, vtkInteractorStyleRubberBand2D);

    virtual void OnLeftButtonUp() override
    {
        if (this->Interaction == SELECTING)
        {
            this->Interaction = NONE;

            // Clear the rubber band
            const int* size = this->Interactor->GetRenderWindow()->GetSize();
            // unsigned char* pixels = this->PixelArray->GetPointer(0);
            unsigned char* pixels = nullptr;
            // this->Interactor->GetRenderWindow()->SetRGBACharPixelData(
            //     0, 0, 300, 300, pixels, 0);
            this->Interactor->GetRenderWindow()->Frame();

            unsigned int rect[5];
            rect[0] = this->StartPosition[0];
            rect[1] = this->StartPosition[1];
            rect[2] = this->EndPosition[0];
            rect[3] = this->EndPosition[1];
            if (this->Interactor->GetShiftKey())
            {
                rect[4] = SELECT_UNION;
            }
            else
            {
                rect[4] = SELECT_NORMAL;
            }
            this->InvokeEvent(vtkCommand::SelectionChangedEvent, reinterpret_cast<void*>(rect));
            this->InvokeEvent(vtkCommand::EndInteractionEvent);
        }
        else if (this->Interaction == PANNING)
        {
            this->Interaction = NONE;
            this->InvokeEvent(vtkCommand::EndInteractionEvent);
        }

        std::cout << "Start position: " << this->StartPosition[0] << " " << this->StartPosition[1] << std::endl;
        std::cout << "End position: " << this->EndPosition[0] << " " << this->EndPosition[1] << std::endl;
    }

    virtual void OnLeftButtonDown() override
    {
        Superclass::OnLeftButtonDown();
    }

    virtual void OnMouseMove() override
    {
        Superclass::OnMouseMove();
    }

    virtual void OnChar() override
    {
        auto ret = this->Interactor->GetKeyCode();
        Superclass::OnChar();
    }
};

vtkStandardNewMacro(MyRubberBand);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkSphereSource> sphereSource;

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RubberBand2D");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    // Render
    renderWindow->SetSize(800, 600);
    renderWindow->Render();

    vtkNew<MyRubberBand> style;
    renderWindowInteractor->SetInteractorStyle(style);

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // STYLE

#endif // TEST61

#ifdef TEST64

#include "vtkActor.h"
#include "vtkAutoInit.h"
#include "vtkAxesActor.h"
#include "vtkBiDimensionalWidget.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToColors.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageReader.h"
#include "vtkImageReslice.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkOutlineFilter.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkResliceCursor.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkSmartPointer.h"
#include "vtkTextActor.h"
#include "vtkTransform.h"

class vtkResliceCursorCallback : public vtkCommand
{
public:
    static vtkResliceCursorCallback* New()
    {
        return new vtkResliceCursorCallback;
    }

    void Execute(vtkObject* caller, unsigned long, void* callData) override
    {
        vtkImagePlaneWidget* ipw = dynamic_cast<vtkImagePlaneWidget*>(caller);
        if (ipw)
        {
            double* wl = static_cast<double*>(callData);

            if (ipw == this->IPW[0])
            {
                this->IPW[1]->SetWindowLevel(wl[0], wl[1], 1);
                this->IPW[2]->SetWindowLevel(wl[0], wl[1], 1);
            }
            else if (ipw == this->IPW[1])
            {
                this->IPW[0]->SetWindowLevel(wl[0], wl[1], 1);
                this->IPW[2]->SetWindowLevel(wl[0], wl[1], 1);
            }
            else if (ipw == this->IPW[2])
            {
                this->IPW[0]->SetWindowLevel(wl[0], wl[1], 1);
                this->IPW[1]->SetWindowLevel(wl[0], wl[1], 1);
            }
        }
        vtkResliceCursorWidget* rcw = dynamic_cast<vtkResliceCursorWidget*>(caller);
        if (rcw)
        {
            vtkResliceCursorLineRepresentation* rep = dynamic_cast<vtkResliceCursorLineRepresentation*>(rcw->GetRepresentation());
            vtkResliceCursor* rc                    = rep->GetResliceCursorActor()->GetCursorAlgorithm()->GetResliceCursor();
            for (int i = 0; i < 3; i++)
            {
                vtkPlaneSource* ps = static_cast<vtkPlaneSource*>(this->IPW[i]->GetPolyDataAlgorithm());
                ps->SetNormal(rc->GetPlane(i)->GetNormal());
                ps->SetCenter(rc->GetPlane(i)->GetOrigin());
                this->IPW[i]->UpdatePlacement();
            }
        }
        this->RCW[0]->Render();
    }

    vtkResliceCursorCallback()
    {
    }

    vtkImagePlaneWidget* IPW[3];
    vtkResliceCursorWidget* RCW[3];
};

int main()
{
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName("../resources/dicom");
    reader->Update();

    /*vtkSmartPointer<vtkOutlineFilter> outline = vtkSmartPointer<vtkOutlineFilter>::New();
    outline->SetInputConnection(reader->GetOutputPort());*/

    vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    outlineMapper->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkActor> outlineActor = vtkSmartPointer<vtkActor>::New();
    outlineActor->SetMapper(outlineMapper);

    vtkSmartPointer<vtkRenderer> ren[4];
    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->SetMultiSamples(0);

    for (int i = 0; i < 4; i++)
    {
        ren[i] = vtkSmartPointer<vtkRenderer>::New();
        renWin->AddRenderer(ren[i]);
    }

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);

    vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

    vtkSmartPointer<vtkImagePlaneWidget> planeWidget[3];
    int imageDims[3];
    reader->GetOutput()->GetDimensions(imageDims);

    for (int i = 0; i < 3; i++)
    {
        planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
        planeWidget[i]->SetInteractor(iren);
        planeWidget[i]->SetPicker(picker);
        planeWidget[i]->RestrictPlaneToVolumeOn();
        double color[3] = { 0, 0, 0 };
        color[i]        = 1;
        planeWidget[i]->GetPlaneProperty()->SetColor(color);
        planeWidget[i]->SetTexturePlaneProperty(ipwProp);
        planeWidget[i]->TextureInterpolateOff();
        planeWidget[i]->SetResliceInterpolateToLinear();
        planeWidget[i]->SetInputConnection(reader->GetOutputPort());
        planeWidget[i]->SetPlaneOrientation(i);
        planeWidget[i]->SetSliceIndex(imageDims[i] / 2);
        planeWidget[i]->DisplayTextOn();
        planeWidget[i]->SetDefaultRenderer(ren[3]);
        planeWidget[i]->SetWindowLevel(1358, -27);
        planeWidget[i]->On();
        planeWidget[i]->InteractionOn();
    }

    planeWidget[1]->SetLookupTable(planeWidget[0]->GetLookupTable());
    planeWidget[2]->SetLookupTable(planeWidget[0]->GetLookupTable());

    vtkSmartPointer<vtkResliceCursorCallback> cbk   = vtkSmartPointer<vtkResliceCursorCallback>::New();
    vtkSmartPointer<vtkResliceCursor> resliceCursor = vtkSmartPointer<vtkResliceCursor>::New();
    resliceCursor->SetCenter(reader->GetOutput()->GetCenter());
    resliceCursor->SetThickMode(0);
    resliceCursor->SetThickness(10, 10, 10);
    resliceCursor->SetImage(reader->GetOutput());

    vtkSmartPointer<vtkResliceCursorWidget> resliceCursorWidget[3];
    vtkSmartPointer<vtkResliceCursorThickLineRepresentation> resliceCursorRep[3];

    double viewUp[3][3] = { { 0, 0, -1 }, { 0, 0, 1 }, { 0, 1, 0 } };
    for (int i = 0; i < 3; i++)
    {
        resliceCursorWidget[i] = vtkSmartPointer<vtkResliceCursorWidget>::New();
        resliceCursorWidget[i]->SetInteractor(iren);

        resliceCursorRep[i] = vtkSmartPointer<vtkResliceCursorThickLineRepresentation>::New();
        resliceCursorWidget[i]->SetRepresentation(resliceCursorRep[i]);
        resliceCursorRep[i]->GetResliceCursorActor()->GetCursorAlgorithm()->SetResliceCursor(resliceCursor);
        resliceCursorRep[i]->GetResliceCursorActor()->GetCursorAlgorithm()->SetReslicePlaneNormal(i);

        const double minVal = reader->GetOutput()->GetScalarRange()[0];
        if (vtkImageReslice* reslice = vtkImageReslice::SafeDownCast(resliceCursorRep[i]->GetReslice()))
        {
            reslice->SetBackgroundColor(minVal, minVal, minVal, minVal);
        }

        resliceCursorWidget[i]->SetDefaultRenderer(ren[i]);
        resliceCursorWidget[i]->SetEnabled(1);

        ren[i]->GetActiveCamera()->SetFocalPoint(0, 0, 0);
        double camPos[3] = { 0, 0, 0 };
        camPos[i]        = 1;
        ren[i]->GetActiveCamera()->SetPosition(camPos);
        ren[i]->GetActiveCamera()->ParallelProjectionOn();
        ren[i]->GetActiveCamera()->SetViewUp(viewUp[i][0], viewUp[i][1], viewUp[i][2]);
        ren[i]->ResetCamera();
        cbk->IPW[i] = planeWidget[i];
        cbk->RCW[i] = resliceCursorWidget[i];
        resliceCursorWidget[i]->AddObserver(vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk);
        double range[2];
        reader->GetOutput()->GetScalarRange(range);
        resliceCursorRep[i]->SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
        planeWidget[i]->SetWindowLevel(range[1] - range[0], (range[0] + range[1]) / 2.0);
        resliceCursorRep[i]->SetLookupTable(resliceCursorRep[0]->GetLookupTable());
        planeWidget[i]->GetColorMap()->SetLookupTable(resliceCursorRep[0]->GetLookupTable());
    }

    /*vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("横断面");
    textActor->GetProperty()->SetColor(0.0,1.0,0.0);*/

    ren[0]->SetBackground(0.3, 0.1, 0.1);
    ren[1]->SetBackground(0.1, 0.3, 0.1);
    ren[2]->SetBackground(0.1, 0.1, 0.3);
    // ren[3]->AddActor(outlineActor);
    ren[3]->SetBackground(0.1, 0.1, 0.1);
    renWin->SetSize(600, 600);

    ren[0]->SetViewport(0, 0, 0.5, 0.5);
    ren[1]->SetViewport(0.5, 0, 1, 0.5);
    ren[2]->SetViewport(0, 0.5, 0.5, 1);
    ren[3]->SetViewport(0.5, 0.5, 1, 1);
    renWin->Render();

    ren[3]->GetActiveCamera()->Elevation(110);
    ren[3]->GetActiveCamera()->SetViewUp(0, 0, -1);
    ren[3]->GetActiveCamera()->Azimuth(45);
    ren[3]->GetActiveCamera()->Dolly(1.15);
    ren[3]->ResetCameraClippingRange();

    vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
    iren->SetInteractorStyle(style);
    iren->Initialize();
    iren->Start();
    return EXIT_SUCCESS;
}
#endif // TEST64


#ifdef TEST66

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <array>
#include <iostream>

namespace {
std::array<float, 4 * 3> vertices { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0 };

class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        std::cout << "command callback\n";

        if (m_actor)
        {
            m_actor->GetProperty()->SetColor(1, 0, 0);
        }
    }

    void SetActor(const vtkSmartPointer<vtkActor>& actor)
    {
        m_actor = actor;
    }

private:
    vtkSmartPointer<vtkActor> m_actor { nullptr };
};

vtkStandardNewMacro(MyCommand);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    cells->InsertNextCell({ 0, 1, 2, 3 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(polyData);

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    // renderer
    vtkNew<vtkRenderer> renderer1;
    renderer1->AddActor(cubeActor);
    renderer1->ResetCamera();

    vtkNew<vtkRenderer> renderer2;
    renderer2->AddActor(cubeActor);
    renderer2->ResetCamera();

    // window
    //----------------------------------------------------------------
    vtkNew<vtkRenderWindow> renWin1;
    renWin1->AddRenderer(renderer1);
    renWin1->SetSize(600, 600);

    // 可以在不同的window中显示同一个actor，并且可以同步更新
    // 但是前提是不同window使用不同renderer，不能是同一个renderer

    vtkNew<vtkRenderWindow> renWin2;
    renWin2->SetSharedRenderWindow(renWin1);
    renWin2->AddRenderer(renderer2);
    renWin2->SetSize(600, 600);
    //----------------------------------------------------------------

    // interactor
    vtkNew<vtkRenderWindowInteractor> iren1;
    iren1->SetRenderWindow(renWin1);
    vtkNew<MyCommand> cb;
    cb->SetActor(cubeActor);
    iren1->AddObserver(vtkCommand::LeftButtonPressEvent, cb);

    vtkNew<vtkRenderWindowInteractor> iren2;
    iren2->SetRenderWindow(renWin2);

    // start
    renWin1->Render();
    renWin2->Render();
    iren1->Start();
    iren2->Start();

    return 0;
}

#endif // TEST66


