
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
1.鼠标左键弹起后修改actor颜色
2.绘制点，GetCell()函数崩溃的问题
3.（顶点 + 索引）绘制立方体（和opengl ebo方式类似），每个顶点添加一个颜色值
4.简单的平面，使用顶点属性数据设置颜色 vtkDataArray
5.通过定义三个指定x-y-z方向坐标的阵列来创建直线栅格。
6.批量生成一大堆线条
7.点构成线条，使用索引
8.修改矢量图箭头颜色 TEST17
9.绘制点，线，面，类似Opengl 顶点+索引的绘制方式
10.2D网格绘制
11.拟合样条曲线生成柱状体
12.vtkAppendPolyData合并多个polydata
13.多窗口使用相同的数据同一个相机（即多个窗口显示完全一样的东西，交互也是同步的）
14.vtkGlyph3D + vtkProbeFilter https://kitware.github.io/vtk-examples/site/Cxx/Visualization/RandomProbe/
15.箭头，可以控制箭头的方向
16.云图 stl文件
17.矢量图 vtkGlyph3D，从vtkGlyph3D获取源数据
18.带流场参数的立方体
19.vtkDistancePolyDataFilter 计算两个vtkPolyData的距离
20.可以随意拖动的坐标轴
21.官方例子，球面法向量  矢量图
22.将点（向量）投影到平面
23.鼠标双击事件 DoubleClick
24.云图，渐变线，点和单元设置流场参数，颜色过渡方式，颜色映射表，色卡
25.多边形三角化，减少三角形的数量，图像和原来保持不变
26.纹理，光照
27.框选拾取 vtk拾取相关的类 https://blog.csdn.net/lovelanjuan/article/details/7301773
28.actor拾取 vtkPropPicker 基于硬件拾取
29.vtk拾取汇总，框选拾取，范围拾取，点拾取，单元拾取
30.范围拾取 vtkAreaPicker 点到actor的最短距离 vtkCellLocator
31.比例尺(vtkLegendScaleActor)
32.vtkColorLegend  https://vtk.org/doc/nightly/html/classvtkColorLegend.html
33.vtkContourFilter  等值面
34.vtkprobefilter    探针
35.vtkStreamTracer 流线图 展示流体流动的轨迹和流动方向 有点像线框式的云图 vtkOpenFOAMReader
36.探针
37.探针 https://blog.csdn.net/liushao1031177/article/details/122860254
38.plot https://kitware.github.io/vtk-examples/site/Cxx/Plotting/LinePlot/
39.以单元形式设置标量数据
40.vtkCellDataToPointData 单元标量数据转顶点数据，等值线vtkContourFilter
41.vtkPointDataToCellData 顶点标量数据转单元数据
42.vtkCellCenters 获取单元中心即格心，并用球体标注格心(矢量图）
43.reverse access ,从经过算法(vtkAlgorithm,vtkPolyDataAlgorithm)变换的数据中获取源数据（vtkPolyData,vtkDataSet）
44.vtkChart https://kitware.github.io/vtk-examples/site/Cxx/DataStructures/OctreeTimingDemo/
45.从Actor中获取polyData，并修改polyData的属性会改变原来Actor的状态，深拷贝可以不改变源Actor状态
46.vtkCellLocator 寻找最近点 一个点到网格上最近距离的点 https://blog.csdn.net/minmindianzi/article/details/103474941
47.保存图片 (位图，矢量图）  qt框选截图 https://blog.csdn.net/GoForwardToStep/article/details/54237689
48.矢量图svg保存
49.vtkImageView  https://blog.csdn.net/liushao1031177/article/details/115799500
50.保存矢量图svg文件
51.利用vtk橡皮筋交互模式，实现鼠标绘制矩形
52.左右两个窗口，一个拖动选择框，一个显示框选的内容（官方例子）vtkBorderWidget   坐标转换  截图
53.vtkImageClip 裁剪图片
54.vtkImageBlend 合成图片，利用多个图片的z值，将多张图片放在一起
55.logowidget  vtkImageCanvasSource2D
56.vtkImageBlend vtkImageCanvasSource2D 将多张图片叠加在一起，可以实现截图框选中为原图颜色，没选中的加阴影
57.图片铺满整个窗口 图片占满(充满整个屏幕) https://kitware.github.io/vtk-examples/site/Cxx/Images/FillWindow/
58.窗口大小改变回调函数 resizeWindowEvent
59.微信群里文件
60.文字显示，汉字
61.2D橡皮筋交互模式，重载鼠标事件  vtkCallbackCommand::SetCallback  使用回调函数替代vtkCommand::Execute()
62.vtkInteractorStyleRubberBandPick 框选拾取
63.vtkImageData转QImage
64.DICOM  MPR 医学ct图四象限
65.vtkBorderWidget事件
66.shared actor 多个vtkRenderWindow共享actor
67.vtkBoxWidget 一个立方体框线盒子，可以将物体包裹起来

*/

#define TEST8

//在cmake加上vtk_module_autoinit就不需要在此处再初始化vtk模块
//#include <vtkAutoInit.h>
//VTK_MODULE_INIT(vtkRenderingOpenGL2);
//VTK_MODULE_INIT(vtkInteractionStyle);
//VTK_MODULE_INIT(vtkRenderingFreeType);
//VTK_MODULE_INIT(vtkRenderingContextOpenGL2);

#ifdef TEST1

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
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

int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;

    // 如果要建立管道连接，请使用SetInputConnection
    // 如果要处理独立数据集，请使用SetInputData
    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    //cubeActor->SetTexture()

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小

    auto style = InteractorStyle::New();
    style->SetActor(cubeActor);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;

}

#endif // TSET1

#ifdef TEST2

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>

#include <array>
#include <iostream>

namespace
{
    std::array<float, 3> vertices{
        10,10,10,
        //1,0,0,
        //1,1,0,
        //0,1,0
    };

    std::array<long long, 1> indices{
        0,
        //1,
        //2,
        //3,
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
    for (size_t i = 0; i < indices.size(); i++)
    {
        cells->InsertNextCell({ indices[i] });  // 这种插入方式有的时候会有问题，导致 GetCell(0)函数崩溃
        cells->InsertNextCell(1, 0);
    }

    polyData->SetPoints(points);
    polyData->SetVerts(cells);

    //mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    //actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetPointSize(5);

    actor->Print(std::cout);
    actor->GetMapper()->GetInput()->Print(std::cout);
    double bounds[6]{ 0 };
    actor->GetMapper()->GetInput()->GetCellBounds(0, bounds);
    auto cell = actor->GetMapper()->GetInput()->GetCell(0);
    cell->GetBounds(bounds);

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //数据交互
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

    std::array<std::array<double, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } },
      { { 0, 1, 0 } }, { { 0, 0, 1 } }, { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } },
      { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };

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
    vtkNew<vtkDoubleArray> pcoords; //用来设置顶点的属性数据
    pcoords->SetNumberOfComponents(3); // 设置元组的个数为3，默认为1
    pcoords->SetNumberOfTuples(4);// 设置总的元组个数为10
    // Assign each tuple. There are 5 specialized versions of SetTuple:
    // SetTuple1 SetTuple2 SetTuple3 SetTuple4 SetTuple9
    // These take 1, 2, 3, 4 and 9 components respectively.
    std::array<std::array<double, 3>, 4> pts = { { { { 0.0, 0.0, 0.0 } }, { { 0.0, 1.0, 0.0 } },
      { { 1.0, 0.0, 0.0 } }, { { 1.0, 1.0, 0.0 } } } };
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

    std::array<double, 47> x = { { -1.22396, -1.17188, -1.11979, -1.06771, -1.01562, -0.963542,
      -0.911458, -0.859375, -0.807292, -0.755208, -0.703125, -0.651042, -0.598958, -0.546875,
      -0.494792, -0.442708, -0.390625, -0.338542, -0.286458, -0.234375, -0.182292, -0.130209,
      -0.078125, -0.026042, 0.0260415, 0.078125, 0.130208, 0.182291, 0.234375, 0.286458, 0.338542,
      0.390625, 0.442708, 0.494792, 0.546875, 0.598958, 0.651042, 0.703125, 0.755208, 0.807292,
      0.859375, 0.911458, 0.963542, 1.01562, 1.06771, 1.11979, 1.17188 } };
    std::array<double, 33> y = { { -1.25, -1.17188, -1.09375, -1.01562, -0.9375, -0.859375, -0.78125,
      -0.703125, -0.625, -0.546875, -0.46875, -0.390625, -0.3125, -0.234375, -0.15625, -0.078125, 0,
      0.078125, 0.15625, 0.234375, 0.3125, 0.390625, 0.46875, 0.546875, 0.625, 0.703125, 0.78125,
      0.859375, 0.9375, 1.01562, 1.09375, 1.17188, 1.25 } };
    std::array<double, 44> z = { { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.9, 1, 1.1, 1.2,
      1.3, 1.4, 1.5, 1.6, 1.7, 1.75, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.75, 2.8, 2.9,
      3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.75, 3.8, 3.9 } };

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
    //std::array<int, 3> dims = { { 2,3,4 } };

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

    deltaZ = 2.0 / (dims[2] - 1);
    deltaRad = (rMax - rMin) / (dims[1] - 1);
    float x[3], v[3];
    v[2] = 0.0;
    for (auto k = 0; k < dims[2]; k++)
    {
        x[2] = -1.0 + k * deltaZ;
        int kOffset = k * dims[0] * dims[1];
        for (auto j = 0; j < dims[1]; j++)
        {
            float radius = rMin + j * deltaRad;
            int jOffset = j * dims[0];
            for (auto i = 0; i < dims[0]; i++)
            {
                float theta = i * vtkMath::RadiansFromDegrees(15.0);
                x[0] = radius * cos(theta);
                x[1] = radius * sin(theta);
                v[0] = -x[1];
                v[1] = x[0];
                int offset = i + jOffset + kOffset;
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

#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <vtkCellType.h>

int main(int argc, char* argv[])
{
    /*这种方式每一条线都要创建指针，建议使用例9方式填充数据*/

    //创建三个坐标点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(1.0, 0.0, 0.0); //返回第一个点的ID：0
    points->InsertNextPoint(0.0, 0.0, 1.0); //返回第二个点的ID：1
    points->InsertNextPoint(0.0, 0.0, 0.0); //返回第三个点的ID：2

    //每两个坐标点之间分别创建一条线
    //SetId()的第一个参数是线段的端点ID，第二个参数是连接的点的ID
    vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();
    line0->GetPointIds()->SetId(0, 0);
    line0->GetPointIds()->SetId(1, 1);

    vtkSmartPointer<vtkLine>line1 = vtkSmartPointer<vtkLine>::New();
    line1->GetPointIds()->SetId(0, 1);
    line1->GetPointIds()->SetId(1, 2);

    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
    line2->GetPointIds()->SetId(0, 2);
    line2->GetPointIds()->SetId(1, 0);

    //创建单元数组，用于存储以上创建的线段
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);

    //将点和线加入到数据集中，前者定义数据集的几何结构（顶点），后者定义拓扑结构（索引）
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

#include <vtkArrowSource.h>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkVector.h>
#include <vtkCleanPolyData.h>
#include <vtkAlgorithmOutput.h>
#include <vtkInteractorStyleRubberBand3D.h>

class CustomStyle :
    public vtkInteractorStyleRubberBand3D
{
public:
    static CustomStyle* New();
    vtkTypeMacro(CustomStyle, vtkInteractorStyleRubberBand3D);

protected:

    void OnLeftButtonDown() override
    {

        Superclass::OnLeftButtonDown();
    }

    void OnMiddleButtonUp() override
    {

        Superclass::OnMiddleButtonUp();
    }

};

vtkStandardNewMacro(CustomStyle);

int main(int, char* [])
{

    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);


    // Set point normals
    vtkNew<vtkDoubleArray> pointNormalsArray;
    auto numOfComponents = pointNormalsArray->GetNumberOfComponents(); // 默认为1，向量的维度
    pointNormalsArray->SetNumberOfComponents(3); // 3d normals (ie x,y,z)
    pointNormalsArray->SetNumberOfTuples(polydata->GetNumberOfPoints());

    // 法线只能控制箭头方向
    double pN1[3] = { 1.0, 0.0, 0.0 };
    double pN2[3] = { 0.0, 1.0, 0.0 };
    double pN3[3] = { 1.0, 1.0, 1.0 };
    double pN4[3] = { 0.0, 0.0, 0.0 };

    // Add the data to the normals array
    pointNormalsArray->SetTuple(0, pN1);  //SetTuple4(id,v0,v1,v2,v3)
    pointNormalsArray->SetTuple(1, pN2);
    pointNormalsArray->SetTuple(2, pN3);
    pointNormalsArray->SetTuple(3, pN4);

    // Add the normals to the points in the polydata
    //polydata->GetPointData()->SetNormals(pointNormalsArray);
    polydata->GetPointData()->SetVectors(pointNormalsArray); //用向量控制箭头的颜色，方向，大小

    // add scalar for colors
    // 标量可以控制箭头的大小以及颜色
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->InsertTuple1(0, 1);
    scalars->InsertTuple1(1, 2);
    scalars->InsertTuple1(2, 3);
    scalars->InsertTuple1(3, 4);

    //polydata->GetPointData()->SetScalars(scalars);

    // color map
    vtkNew<vtkLookupTable> pColorTable;
    pColorTable->SetNumberOfColors(10);
    //pColorTable->SetHueRange(.0, .67);
    pColorTable->Build();

    vtkNew<vtkNamedColors> colors;

    // Create anything you want here, we will use a cube/arrow for the demo.
    vtkNew<vtkCubeSource> cubeSource;
    vtkNew<vtkArrowSource> arrowSource;
    arrowSource->SetShaftRadius(0.01);
    arrowSource->SetTipRadius(0.05);
    arrowSource->SetTipLength(0.2);

    vtkNew<vtkGlyph3D> glyph3D;
    //glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3D->SetSourceConnection(arrowSource->GetOutputPort());

    auto mode = glyph3D->GetColorModeAsString();
    //glyph3D->SetColorModeToColorByScale();    // 颜色使用标量控制（类似云图颜色映射）
    glyph3D->SetColorModeToColorByVector();    // 使用向量映射颜色（不再需要设置scalerRange）
    glyph3D->SetScaleModeToDataScalingOff();   // 关闭由于数据（标量或向量）导致的缩放
    //glyph3D->SetScaleModeToScaleByVectorComponents();
    //glyph3D->SetInputArrayToProcess()

    glyph3D->SetVectorModeToUseVector();  // 使用向量设置方向
    //glyph3D->SetVectorModeToUseNormal();// 使用法线设置方向
    glyph3D->SetInputData(polydata);
    //glyph3D->SetInputConnection()
    auto scale = glyph3D->GetScaleFactor();
    //glyph3D->SetScaleFactor(2.0);  //设置缩放比例，默认为1
    glyph3D->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph3D->GetOutputPort());
    // 使用可视化管线
    //auto connect = mapper->GetInputConnection(0, 0);
    //if (glyph3D->GetOutputPort() == connect)
    //    std::cout << "same";

    //mapper->SetInputData(glyph3D->GetOutput());
    //mapper->SetScalarRange(1, 4);

    //mapper->SetColorModeToDirectScalars();
    //mapper->SetColorModeToDefault();
    mapper->SetLookupTable(pColorTable);

    // 如果添加的是向量，获取的范围就是向量的【模长】最大最小值
    auto range = mapper->GetInput()->GetScalarRange();
    mapper->SetScalarRange(range);



    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    mapper->ScalarVisibilityOff();
    actor->GetProperty()->SetColor(1, 0, 0);
    mapper->ScalarVisibilityOn();

    // 从经过vtkAlgorithm变换后的数据获取源数据（注意使用的是独立数据集还是管道连接方式）
    vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    auto reference = dynamic_cast<vtkGlyph2D*>(algorithm.GetPointer()); // 如果没有经过algorithm变换，reference为nullptr
    //auto originSource = vtkPolyData::SafeDownCast(reference->GetInput());
    //auto originPointNum = originSource->GetNumberOfPoints(); // 源数据共有四个顶点


    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->SetWindowName("Glyph3D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST8

#ifdef TEST9

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
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkInteractorStyleUser.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackballActor.h>

#include <array>

#define _LINE

int main()
{
#ifdef _CUBE
    //立方体
    std::array<std::array<double, 3>, 8> pts = { { { { 0, 0, 0 } }, { { 1, 0, 0 } }, { { 1, 1, 0 } },
      { { 0, 1, 0 } }, { { 0, 0, 1 } }, { { 1, 0, 1 } }, { { 1, 1, 1 } }, { { 0, 1, 1 } } } };
    // The ordering of the corner points on each face.
    std::array<std::array<vtkIdType, 4>, 6> ordering = { { { { 0, 1, 2, 3 } }, { { 4, 5, 6, 7 } },
      { { 0, 1, 5, 4 } }, { { 1, 2, 6, 5 } }, { { 2, 3, 7, 6 } }, { { 3, 0, 4, 7 } } } };
#endif // _CUBE

#ifdef _LINE
    //三条线段
    std::array<std::array<double, 3>, 4> pts = { { {0,0,0},{0,1,0},{1,1,0},{1,0,0}} };
    std::array<std::array<vtkIdType, 2>, 3> ordering = { {{0,1},{1,2},{2,3} } };
#endif // _LINE

#ifdef _TRIANGLE
    //两个三角形构成一个长方形
    std::array<std::array<double, 3>, 4> pts = { {{0,0,0} ,{0,1,0}, {1,1,0},{1,0,0} } }; //左下，左上，右上，右下
    std::array<std::array<vtkIdType, 3>, 2> ordering = { {{0,1,2},{2,3,0}} };
#endif // _TRIANGLE

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPolyData> cube;  // 可以理解为opengl的vao
    vtkNew<vtkPoints> points;  // 顶点
    vtkNew<vtkCellArray> polys;// 索引
    vtkNew<vtkFloatArray> scalars; // vtkDataArray 属性数据

    // 顶点，可以理解为opengl的vertices
    for (auto i = 0ul; i < pts.size(); ++i)
    {
        points->InsertPoint(i, pts[i].data());
        scalars->InsertTuple1(i, i);  //第一个参数为顶点索引，第二个为实际值
    }
    // 索引，可以理解为opengl的indices
    for (auto&& i : ordering)
    {
        polys->InsertNextCell(vtkIdType(i.size()), i.data());
    }

    // 设置几何结构
    cube->SetPoints(points);

    // 设置拓扑结构，即设置图元类型，类似的函数还有SetVerts（点）等
    //cube->SetPolys(polys);  //多边形，面
    cube->SetLines(polys);  //_LINE:三条线段（即首尾不相连），_TRIANGLE:四条线段（即首尾相连）
    //cube->SetStrips(polys); //三角形带，面
    //cube->SetVerts(polys);  //点

    // 设置每个顶点的参数，可以理解为云图中数据的w分量
    //cube->GetPointData()->SetScalars(scalars);

    // Mapper用来将输入的数据转换为几何图元（点、线、多边形）进行渲染
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(cube);

    // 属性数据设置方法：https://blog.csdn.net/zy2317878/article/details/78744825
    // 获取流场参数的最大最小值，返回double数组
    auto scalar = cube->GetScalarRange();
    double myScalarRange[]{ 0,2 };  // 可以理解为流场数据范围，最小值0为红色，最大值2为蓝色，那么1就为绿色
    cubeMapper->SetScalarRange(cube->GetScalarRange());
    //cubeMapper->SetScalarRange(myScalarRange);

    //设置图元颜色
    vtkNew<vtkProperty> cubeProperty;
    cubeProperty->SetColor(0, 255, 0);

    // Actor负责渲染场景中数据的可视化表达，二维空间中的数据通过vtkActor2D表达
    // Actor依赖于Mapper对象和vtkPropety对象，Mapper负责存放数据和渲染信息等；vtkProperty负责控制颜色，不透明度等属性
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    //cubeActor->SetTexture(); //设置纹理，具体用法可以看QWidgetVTK代码
    cubeActor->SetProperty(cubeProperty);

    // 相机 常用函数Dolly()，Roll()，Azimuth()，Yaw()，Elevation()，Pitch()和Zoom()
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1); // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); //设置焦点位置

    vtkNew<vtkRenderer> renderer;   //负责管理场景的渲染过程，组成对象包括Actor，照相机，光照
    vtkNew<vtkRenderWindow> renWin; //窗口，一个窗口可以有多个vtkRenderer，不同的render可以在不同的视口
    renWin->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren; //提供平台独立的响应鼠标，键盘，时钟时间的交互机制
    iren->SetRenderWindow(renWin); //设置渲染窗口

    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();
    iren->SetInteractorStyle(style); //定义交互器样式，默认为vtkInteractorStyleSwitch

    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //背景颜色，可以设置为渐变色
    renderer->SetBackground(colors->GetColor3d("Black").GetData());

    renWin->SetSize(600, 600); //窗口大小

    // interact with data
    renWin->Render();
    iren->Start(); //程序进入事件响应循环，交互器处于等待状态

    return EXIT_SUCCESS;
}

#endif // TEST9

#ifdef TEST10

#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPolyDataMapper.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include <vtkInteractorStyleUnicam.h>
#include <vtkInteractorStyleUser.h>

using namespace std;

int main(int argc, char* argv[])
{
    size_t vercount = 4;
    size_t indcount = 4;
    //size_t indcount = 6;
    float vertices[] = { 0.,0.,1.,0.,1.,1.,2.,1. };  // 共四个2D顶点
    unsigned int indices[] = { 0,1,2,3 };
    //unsigned int indices[] = { 0,1,1,2,2,3 };


    vtkPoints* points = vtkPoints::New();     //顶点
    vtkCellArray* cells = vtkCellArray::New();//索引

    for (size_t i = 0; i < vercount; i++)
    {
        points->InsertNextPoint(vertices[i * 2 + 0], vertices[i * 2 + 1], 0);
    }

    for (size_t i = 0; i < indcount; i += 2)
    {
        cells->InsertNextCell({ indices[i],indices[i + 1] });
    }

    // 渲染机制未知，需要同时设置点坐标与点坐标对应的verts
    // verts中的id必须与点坐标对应
    vtkPolyData* polyData = vtkPolyData::New();
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    //下面为正常的可视化流程，可设置的点云颜色、大小等
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputData(polyData);

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    //设置颜色与点大小
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    //actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetLineWidth(5);


    vtkRenderer* renderer = vtkRenderer::New();
    renderer->AddActor(actor);
    // 设置背景颜色
    renderer->SetBackground(1, 1, 1);

    vtkRenderWindow* renderWindow = vtkRenderWindow::New();
    renderWindow->AddRenderer(renderer);

    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renderWindow);

    vtkInteractorStyleTrackballCamera* style = vtkInteractorStyleTrackballCamera::New();  //左键旋转，滚轮缩放，滚轮拖动，右键缩放


    //style->set

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

//cells->SetData();
//cells->ExportLegacyFormat();
//cells->ImportLegacyFormat();
//cells->AppendLegacyFormat();
//cells->InsertNextCell({ 2,1,2 });  // 第一个参数是单元顶点的个数

#endif // TEST10

#ifdef TEST11

#include <vtkSmartPointer.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGlyph3DMapper.h>
#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkAutoInit.h> 
#include <vtkTubeFilter.h> 
int main(int, char* [])
{
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

    // Create three points. We will join (Origin and P0) with a red line and (Origin and P1) with a green line
    double origin[3] = { 0.0, 0.0, 0.0 };
    double p0[3] = { 0,3,2 };
    double p1[3] = { 5,4.4,3.6 };
    double p2[3] = { 10,4.4,4.2 };
    double p3[3] = { 15,4.4,4.8 };


    // Create a vtkPoints object and store the points in it
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(origin);
    points->InsertNextPoint(p0);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);

    vtkSmartPointer<vtkParametricSpline> spline =
        vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetPoints(points);
    vtkSmartPointer<vtkParametricFunctionSource> functionSource =
        vtkSmartPointer<vtkParametricFunctionSource>::New();
    functionSource->SetParametricFunction(spline);
    functionSource->Update();

    vtkSmartPointer<vtkTubeFilter> Tube =
        vtkSmartPointer<vtkTubeFilter>::New();
    Tube->SetInputConnection(functionSource->GetOutputPort());
    Tube->SetRadius(0.5);
    Tube->SetNumberOfSides(20);
    Tube->Update();
    // Setup actor and mapper
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(Tube->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    // Setup render window, renderer, and interactor
    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);

    renderer->SetBackground(colors->GetColor3d("Silver").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST11

#ifdef TEST12

//https://blog.csdn.net/qq_41023026/article/details/119776151

#include "vtkActor.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkCutter.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkStripper.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <vtkCubeSource.h>
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"
#include "vtkCleanPolyData.h"


int main()
{
    vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
    coneSource->SetCenter(0, 0, 0);
    coneSource->SetRadius(2);
    coneSource->SetHeight(10);
    coneSource->SetResolution(50);
    coneSource->Update();

    // second 
    vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();
    line->SetPoint1(1.0, 0, 0);
    line->SetPoint2(0, 1.0, 20);

    vtkSmartPointer<vtkTubeFilter> tube = vtkSmartPointer<vtkTubeFilter>::New();
    tube->SetInputConnection(line->GetOutputPort());
    tube->SetRadius(2);
    tube->SetNumberOfSides(50);
    tube->SetCapping(1);
    tube->Update();

    vtkNew< vtkCubeSource> cube;
    cube->SetXLength(10);
    cube->SetYLength(10);
    cube->SetZLength(10);
    cube->Update();

    // combine two poly data
    vtkSmartPointer<vtkAppendPolyData> appendFilter =
        vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter->AddInputData(tube->GetOutput());
    appendFilter->AddInputData(coneSource->GetOutput());
    appendFilter->AddInputData(cube->GetOutput());
    appendFilter->Update();

    // Remove any duplicate points.
    vtkSmartPointer<vtkCleanPolyData> cleanFilter =
        vtkSmartPointer<vtkCleanPolyData>::New();
    cleanFilter->SetInputConnection(appendFilter->GetOutputPort());
    cleanFilter->Update();

    //Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> appendmapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    appendmapper->SetInputConnection(cleanFilter->GetOutputPort());

    vtkSmartPointer<vtkActor> appendactor =
        vtkSmartPointer<vtkActor>::New();
    appendactor->SetMapper(appendmapper);
    appendactor->GetProperty()->SetColor(1., 0., 0.);

    //Create a renderer, render window, and interactor
    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    //Add the actors to the scene
    renderer->AddActor(appendactor);
    renderer->SetBackground(.3, .2, .1); // Background color dark red

    //Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();


    return EXIT_SUCCESS;
}

#endif // TEST12

#ifdef TEST13

//https://blog.csdn.net/calmreason/article/details/88712248

#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkSmartPointer.h"
#include "vtkInteractorStyleSwitch.h"
#include "vtkContourWidget.h"
#include "vtkContourRepresentation.h"

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

        //遍历所有renderer，都设置一遍最新的相机
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
    int numPts = 5;
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

    //渲染时使用同样的数据
    contourWidget1->SetInteractor(windowInteractor01);
    contourWidget2->SetInteractor(windowInteractor02);
    contourWidget1->On();
    contourWidget2->On();
    contourWidget1->Initialize(polydata);
    contourWidget2->Initialize(polydata);

    //形状使用同样的数据来保持同步
    vtkSmartPointer< vtkMyContourCallback> contourCallBack1 = vtkSmartPointer< vtkMyContourCallback>::New();
    contourCallBack1->m_contourWidget = contourWidget1;
    vtkSmartPointer< vtkMyContourCallback> contourCallBack2 = vtkSmartPointer< vtkMyContourCallback>::New();
    contourCallBack2->m_contourWidget = contourWidget2;
    contourWidget1->AddObserver(vtkCommand::EndInteractionEvent, contourCallBack1);//拖动控制点完成
    contourWidget2->AddObserver(vtkCommand::EndInteractionEvent, contourCallBack2);//拖动控制点完成
    contourWidget1->AddObserver(vtkCommand::KeyReleaseEvent, contourCallBack1);//删除一个控制点
    contourWidget2->AddObserver(vtkCommand::KeyReleaseEvent, contourCallBack2);//删除一个拖动控制点
    contourWidget1->AddObserver(vtkCommand::InteractionEvent, contourCallBack1);//拖动一个控制点
    contourWidget2->AddObserver(vtkCommand::InteractionEvent, contourCallBack2);//拖动一个拖动控制点


    //相机同步
    vtkSmartPointer<vtkMyCameraCallback> cameraCallback1 = vtkSmartPointer<vtkMyCameraCallback>::New();
    cameraCallback1->m_renderer = renderer01;
    vtkSmartPointer<vtkMyCameraCallback> cameraCallback2 = vtkSmartPointer<vtkMyCameraCallback>::New();
    cameraCallback2->m_renderer = renderer02;
    renderer01->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraCallback1);
    renderer02->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, cameraCallback2);
    renderer01->ResetCamera();
    renderer02->ResetCamera();

    //程序启动
    windowInteractor01->Initialize();
    windowInteractor02->Initialize();
    windowInteractor01->Start();
    windowInteractor02->Start();

    return 0;

}

#endif // TEST13

#ifdef TEST14

#include <vtkActor.h>
#include <vtkCone.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3D.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkThreshold.h>
#include <vtkThresholdPoints.h>
#include <vtkUnsignedCharArray.h>

int main(int argc, char* argv[])
{
    int resolution = 50;
    if (argc > 1)
    {
        resolution = atoi(argv[1]);
    }

    vtkNew<vtkNamedColors> colors;

    // Create a sampled cone
    vtkNew<vtkCone> implicitCone;
    implicitCone->SetAngle(30.0);

    double radius = 1.;
    vtkNew<vtkSampleFunction> sampledCone;
    sampledCone->SetSampleDimensions(resolution, resolution, resolution);
    double xMin = -radius * 2.0;
    double xMax = radius * 2.0;
    sampledCone->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledCone->SetImplicitFunction(implicitCone);

    vtkNew<vtkThreshold> thresholdCone;
    thresholdCone->SetInputConnection(sampledCone->GetOutputPort());
    thresholdCone->SetLowerThreshold(0);
    thresholdCone->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);

    vtkNew<vtkPointSource> randomPoints;
    randomPoints->SetCenter(0.0, 0.0, 0.0);
    randomPoints->SetNumberOfPoints(10000);
    randomPoints->SetDistributionToUniform();
    randomPoints->SetRadius(xMax);

    // Probe the cone dataset with random points
    vtkNew<vtkProbeFilter> randomProbe;
    randomProbe->SetInputConnection(0, randomPoints->GetOutputPort());
    randomProbe->SetInputConnection(1, thresholdCone->GetOutputPort());
    randomProbe->Update();
    randomProbe->GetOutput()->GetPointData()->SetActiveScalars(
        "vtkValidPointMask");

    vtkNew<vtkThresholdPoints> selectPoints;
    selectPoints->SetInputConnection(randomProbe->GetOutputPort());
    selectPoints->ThresholdByUpper(1.0);

    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(0.025);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetSourceConnection(sphere->GetOutputPort());
    glyph->SetInputConnection(selectPoints->GetOutputPort());

    // Create a mapper and actor
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort());
    mapper->ScalarVisibilityOff();
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RandomProbe");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}



#endif // TEST14

#ifdef TEST15

#include <vtkArrowSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkMinimalStandardRandomSequence.h>

#include <array>
#include <iostream>

// 设置箭头位置
vtkSmartPointer<vtkDataSetMapper> createArrow(double* start, double* end)
{
    auto _arrowSource = vtkSmartPointer<vtkArrowSource>::New();

    // Compute a basis
    double normalizedX[3];
    double normalizedY[3];
    double normalizedZ[3];

    // The X axis is a vector from start to end
    vtkMath::Subtract(end, start, normalizedX);
    double length = vtkMath::Norm(normalizedX);
    vtkMath::Normalize(normalizedX);

    // The Z axis is an arbitrary vector cross X
    vtkSmartPointer<vtkMinimalStandardRandomSequence> rng = vtkSmartPointer<vtkMinimalStandardRandomSequence>::New();

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
    vtkSmartPointer<vtkMatrix4x4> matrix =
        vtkSmartPointer<vtkMatrix4x4>::New();

    // Create the direction cosine matrix
    matrix->Identity();
    for (auto i = 0; i < 3; i++)
    {
        matrix->SetElement(i, 0, normalizedX[i]);
        matrix->SetElement(i, 1, normalizedY[i]);
        matrix->SetElement(i, 2, normalizedZ[i]);
    }

    // Apply the transforms
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(start);
    transform->Concatenate(matrix);
    transform->Scale(length, length, length);

    // Transform the polydata
    vtkSmartPointer<vtkTransformPolyDataFilter> transformPD = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformPD->SetTransform(transform);
    transformPD->SetInputConnection(_arrowSource->GetOutputPort());

    vtkNew<vtkDataSetMapper> arrowMapper;
    arrowMapper->SetInputConnection(transformPD->GetOutputPort());

    return arrowMapper;
}

int main()
{
    /*
    SetShaftRadius        设置轴线半径，轴的粗细
    SetShaftResolution    设置轴线分辨率，轴截面的圆由多少个线段构成
    SetShaftLength        设置轴线长度
    SetTipRadius          设置端点圆锥体半径，圆锥体底面圆的半径
    SetTipResolution      设置端点圆锥体分辨率，圆锥体侧面由多少个三角构成，
    SetTipLength          设置端点圆锥体长度，圆锥体的高度
    */

    {
        vtkNew<vtkArrowSource> arrowSource;

        auto num1 = arrowSource->GetShaftResolution();  //6
        auto num2 = arrowSource->GetTipResolution();    //6

        arrowSource->SetShaftRadius(.01);
        arrowSource->SetTipLength(.2);
        arrowSource->SetTipRadius(.05);
        arrowSource->Update();

        std::cout << num1 << ',' << num2 << '\n';

        auto polyData = arrowSource->GetOutput();
        auto numPoints = polyData->GetNumberOfPoints();
        auto numCells = polyData->GetNumberOfCells();
        auto numPolys = polyData->GetNumberOfPolys();

        std::cout << "numPoints: " << numPoints << '\t' << "numCells: " << numCells << '\t' << "numPolys: " << numPolys << '\n';
    }


    vtkNew<vtkActor> actor;

    if (1)
    {
        // 默认位置
        vtkNew<vtkArrowSource> arrowSource;

        arrowSource->SetShaftResolution(10);
        arrowSource->SetShaftRadius(0.05);
        arrowSource->SetTipRadius(0.1);
        arrowSource->SetTipLength(0.3);
        arrowSource->SetTipResolution(10);
        arrowSource->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(arrowSource->GetOutputPort());
        actor->SetMapper(mapper);
    }
    else
    {
        // 自定义位置
        double start[] = { 1,0,10 };
        double end[] = { 20,-10,10 };
        actor->SetMapper(createArrow(start, end));
    }

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST15

#ifdef TEST16

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>


int main(int, char* [])
{
    // 加载一个STL模型
    vtkSmartPointer<vtkSTLReader> source = vtkSmartPointer<vtkSTLReader>::New();
    source->SetFileName("test1.stl");
    source->Update();

    int numPts = source->GetOutput()->GetPoints()->GetNumberOfPoints();                    // 获取模型的顶点数量
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();        // 创建存储顶点属性的float数组
    scalars->SetNumberOfValues(numPts);
    for (int i = 0; i < numPts; ++i)        // 为属性数组中的每个元素设置标量值（这个标量值可以当作颜色值）
        scalars->SetValue(i, i);

    vtkSmartPointer<vtkPolyData> poly = vtkSmartPointer<vtkPolyData>::New();
    poly->DeepCopy(source->GetOutput());
    poly->GetPointData()->SetScalars(scalars);

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetNumberOfColors(numPts);        // 指定颜色查找表中有多少种颜色
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    mapper->SetScalarRange(0, numPts);            // 设置标量值的范围
    mapper->ScalarVisibilityOn();
    //mapper->SetColorModeToMapScalars();        // 无论变量数据是何种类型，该方法都通过查询表对标量数据进行映射
    mapper->SetColorModeToDefault();            // 默认的映射器行为，即把unsigned char类型的标量属性数据当作颜色值，不执行隐式。对于其他类型的标量数据，将通过查询表映射。
    mapper->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->GradientBackgroundOn();
    renderer->SetBackground(1, 1, 1);
    renderer->SetBackground2(0, 0, 0);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST16

#ifdef TEST17
//https://kitware.github.io/vtk-examples/site/Cxx/Filtering/Glyph3D/
#include <vtkArrowSource.h>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph2D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkVector.h>
#include <vtkCleanPolyData.h>
#include <vtkAlgorithmOutput.h>

int main(int, char* [])
{
    // Create 3 points
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    // Add the points to a polydata
    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);

    vtkVector2f vecF(1, 2);
    vecF.Set(1, 2);
    auto ret = vecF.Norm();

    vtkNew<vtkFloatArray> arr;

    // Set point normals
    vtkNew<vtkDoubleArray> pointNormalsArray;
    auto com = pointNormalsArray->GetNumberOfComponents(); // 默认为1，向量的维度
    pointNormalsArray->SetNumberOfComponents(3); // 3d normals (ie x,y,z)
    pointNormalsArray->SetNumberOfTuples(polydata->GetNumberOfPoints());

    // 法线只能控制箭头方向
    double pN1[3] = { 1.0, 0.0, 0.0 };
    double pN2[3] = { 0.0, 1.0, 0.0 };
    double pN3[3] = { 1.0, 1.0, 1.0 };
    double pN4[3] = { 0.0, 0.0, 0.0 };

    // Add the data to the normals array
    pointNormalsArray->SetTuple(0, pN1);  //SetTuple4(id,v0,v1,v2,v3)
    pointNormalsArray->SetTuple(1, pN2);
    pointNormalsArray->SetTuple(2, pN3);
    pointNormalsArray->SetTuple(3, pN4);

    // Add the normals to the points in the polydata
    //polydata->GetPointData()->SetNormals(pointNormalsArray);
    polydata->GetPointData()->SetVectors(pointNormalsArray); //用向量控制箭头的颜色，方向，大小

    // add scalar for colors
    // 标量可以控制箭头的大小以及颜色
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->InsertTuple1(0, 1);
    scalars->InsertTuple1(1, 2);
    scalars->InsertTuple1(2, 3);
    scalars->InsertTuple1(3, 4);

    //polydata->GetPointData()->SetScalars(scalars);

    // color map
    vtkNew<vtkLookupTable> pColorTable;
    pColorTable->SetNumberOfColors(10);
    //pColorTable->SetHueRange(.0, .67);
    pColorTable->Build();

    vtkNew<vtkNamedColors> colors;

    // Create anything you want here, we will use a cube/arrow for the demo.
    vtkNew<vtkCubeSource> cubeSource;
    vtkNew<vtkArrowSource> arrowSource;
    arrowSource->SetShaftRadius(0.01);
    arrowSource->SetTipRadius(0.05);
    arrowSource->SetTipLength(0.2);

    //arrowSource->SetTipLength(0.02);
    //arrowSource->SetShaftRadius(0.1);
    //arrowSource->SetTipLength(1);
    //SetShaftRadius        设置轴线半径，轴的粗细
    //    SetShaftResolution    设置轴线分辨率，轴截面的圆由多少个线段构成
    //    SetTipRadius        设置端点圆锥体半径，圆锥体底面圆的半径
    //    SetTipResolution    设置端点圆锥体分辨率，圆锥体侧面由多少个三角构成，
    //    SetTipLength        设置端点圆锥体长度，圆锥体的高度

    vtkNew<vtkGlyph2D> glyph3D;
    //glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3D->SetSourceConnection(arrowSource->GetOutputPort());

    auto mode = glyph3D->GetColorModeAsString();
    //glyph3D->SetColorModeToColorByScale();    // 颜色使用标量控制（类似云图颜色映射）
    glyph3D->SetColorModeToColorByVector();    // 使用向量映射颜色（不再需要设置scalerRange）
    glyph3D->SetScaleModeToDataScalingOff();   // 关闭由于数据（标量或向量）导致的缩放
    //glyph3D->SetScaleModeToScaleByVectorComponents();
    //glyph3D->SetInputArrayToProcess()

    glyph3D->SetVectorModeToUseVector();  // 使用向量设置方向
    //glyph3D->SetVectorModeToUseNormal();// 使用法线设置方向
    glyph3D->SetInputData(polydata);
    //glyph3D->SetInputConnection()
    auto scale = glyph3D->GetScaleFactor();
    //glyph3D->SetScaleFactor(2.0);  //设置缩放比例，默认为1
    glyph3D->Update();

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(glyph3D->GetOutputPort());
    // 使用可视化管线
    //auto connect = mapper->GetInputConnection(0, 0);
    //if (glyph3D->GetOutputPort() == connect)
    //    std::cout << "same";

    //mapper->SetInputData(glyph3D->GetOutput());
    //mapper->SetScalarRange(1, 4);

    //mapper->SetColorModeToDirectScalars();
    //mapper->SetColorModeToDefault();
    mapper->SetLookupTable(pColorTable);

    // 如果添加的是向量，获取的范围就是向量的【模长】最大最小值
    auto range = mapper->GetInput()->GetScalarRange();
    mapper->SetScalarRange(range);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // 从经过vtkAlgorithm变换后的数据获取源数据（注意使用的是独立数据集还是管道连接方式）
    vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    auto reference = dynamic_cast<vtkGlyph2D*>(algorithm.GetPointer()); // 如果没有经过algorithm变换，reference为nullptr
    //auto originSource = vtkPolyData::SafeDownCast(reference->GetInput());
    //auto originPointNum = originSource->GetNumberOfPoints(); // 源数据共有四个顶点


    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->SetWindowName("Glyph3D");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST17

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
#include <vtkLookupTable.h>
#include "vtkSmartPointer.h"

int main()
{
    int i;
    //定义立方体的顶点坐标
    static float x[8][3] = { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, { 0, 1, 0 },
    { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } };
    //定义单元，每4个顶点建立一个四边形单元，共计6个单元
    static vtkIdType pts[6][4] = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 0, 1, 5, 4 },
    { 1, 2, 6, 5 }, { 2, 3, 7, 6 }, { 3, 0, 4, 7 } };
    //创建对象
    vtkSmartPointer<vtkPolyData> cube = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    //存储顶点
    for (i = 0; i < 8; i++)
        points->InsertPoint(i, x[i]);
    //设定单元
    for (i = 0; i < 6; i++)
        polys->InsertNextCell(4, pts[i]);

    //存储标量值
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    //设定每个顶点的标量值
    for (i = 0; i < 8; i++)
        scalars->InsertTuple1(i, i * 4);
    //创建多边形数据
    cube->SetPoints(points);
    //设定单元类型为多边形
    cube->SetPolys(polys);
    //设定每个顶点的标量值
    cube->GetPointData()->SetScalars(scalars);
    //定义颜色映射表
    vtkSmartPointer<vtkLookupTable> pColorTable = vtkSmartPointer<vtkLookupTable>::New();
    //设置颜色表中的颜色
    pColorTable->SetNumberOfColors(256);
    pColorTable->SetHueRange(0.67, 0.0);        //色调范围从红色到蓝色
    pColorTable->Build();
    //数据映射
    vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    cubeMapper->SetInputData(cube);
    cubeMapper->SetScalarRange(0, 7);
    cubeMapper->SetLookupTable(pColorTable);
    vtkSmartPointer<vtkActor> cubeActor = vtkSmartPointer<vtkActor>::New();
    cubeActor->SetMapper(cubeMapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
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

int main(int, char* [])
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

    vtkSmartPointer<vtkDistancePolyDataFilter> distanceFilter =
        vtkSmartPointer<vtkDistancePolyDataFilter>::New();

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
    //scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetLookupTable(mapper2->GetLookupTable());
    scalarBar->SetTitle("Distance");
    scalarBar->SetNumberOfLabels(5);
    scalarBar->SetTextPad(4);
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renWinInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
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

#ifdef TEST20

#include <vtkAnnotatedCubeActor.h>
#include <vtkAxesActor.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkCellData.h>
#include <vtkColorSeries.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkElevationFilter.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <array>

namespace {

    /**
     * Make an axes actor.
     *
     * @param scale: 轴的比例和方向
     * @param xyzLabels: 轴的文字标签
     * @return The axes actor.
     */
    vtkSmartPointer<vtkAxesActor>
        MakeAxesActor(std::array<double, 3>& scale,
            std::array<std::string, 3>& xyzLabels);

} // namespace

int main(int, char* [])
{
    // Basic stuff setup
    // Set up the renderer, window, and interactor
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    //vtkNew< vtkGenericOpenGLRenderWindow> renWin;

    renWin->AddRenderer(ren);
    renWin->SetSize(640, 480);
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);

    vtkNew< vtkInteractorStyleRubberBand3D> style;
    iRen->SetInteractorStyle(style);

    std::array<std::string, 3> xyzLabels{ {"X", "Y", "Z"} };
    std::array<double, 3> scale{ {1.0, 1.0, 1.0} };
    auto axes = MakeAxesActor(scale, xyzLabels);

    // 2D的窗口小部件，可以用鼠标拖来拖去，小部件上可以绘制其他图元
    vtkNew<vtkOrientationMarkerWidget> om2;
    om2->SetOrientationMarker(axes);
    // Position lower right in the viewport.
    om2->SetViewport(0.5, 0, 1.0, 0.5);
    //om2->SetShouldConstrainSize(true);       // 开启最大最小尺寸限制
    //om2->SetSizeConstraintDimensionSizes();  // 设置最大和最小尺寸

    om2->SetInteractor(iRen);
    om2->EnabledOn();
    om2->InteractiveOn();

    //ren->AddActor(axes);

    // 设置窗口的背景颜色（渐变色）
    //ren->SetBackground2(colors->GetColor3d("RoyalBlue").GetData());
    ren->SetBackground(.1, .2, .3);
    //ren->SetBackground(colors->GetColor3d("MistyRose").GetData());
    //ren->GradientBackgroundOn();

    // 设置摄像机方向
    ren->GetActiveCamera()->Azimuth(45);
    ren->GetActiveCamera()->Pitch(-22.5);
    ren->ResetCamera();

    renWin->SetSize(600, 1000);
    renWin->Render();
    renWin->SetWindowName("ColoredAnnotatedCube");
    renWin->Render();
    iRen->Start();

    return EXIT_SUCCESS;
}

namespace {

    vtkSmartPointer<vtkAxesActor>
        MakeAxesActor(std::array<double, 3>& scale,
            std::array<std::string, 3>& xyzLabels)
    {
        vtkNew<vtkAxesActor> axes;
        //axes->SetScale(scale[0], scale[1], scale[2]);
        //axes->SetShaftTypeToCylinder();
        //axes->SetXAxisLabelText(xyzLabels[0].c_str());
        //axes->SetYAxisLabelText(xyzLabels[1].c_str());
        //axes->SetZAxisLabelText(xyzLabels[2].c_str());

        //axes->SetCylinderRadius(0.5 * axes->GetCylinderRadius());  //轴的粗细
        //axes->SetConeRadius(1.025 * axes->GetConeRadius());        //圆锥的大小
        //axes->SetSphereRadius(0.1 * axes->GetSphereRadius());


        vtkTextProperty* tprop = axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
        tprop->ItalicOn();  //启用文本斜体
        tprop->ShadowOn();  //启用文本阴影
        tprop->SetFontFamilyToTimes();  //字体
        tprop->SetColor(1, 1, 1);  //标签文字颜色

        // Use the same text properties on the other two axes.
        axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
        axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->ShallowCopy(tprop);
        return axes;
    }

} // namespace


#endif // TEST20

#ifdef TEST21

// https://kitware.github.io/vtk-examples/site/Cxx/Points/NormalEstimation/

#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCANormalEstimation.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>

#include <vtkPolyDataNormals.h>

namespace {
    void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);
}

int main(int, char* [])
{
    double radius = 1.0;
    vtkNew<vtkPointSource> points;
    points->SetNumberOfPoints(1000);
    points->SetRadius(radius);
    points->SetCenter(0.0, 0.0, 0.0);
    points->SetDistributionToShell();

    double p0[3] = { 1.0, 0.0, 0.0 };
    double p1[3] = { 0.0, 1.0, 0.0 };

    vtkNew<vtkLineSource> lineSource;
    lineSource->SetPoint1(p0);
    lineSource->SetPoint2(p1);

    vtkNew<vtkPolyDataNormals> nor;

    int sampleSize = 10;
    vtkNew<vtkPCANormalEstimation> normals;
    normals->SetInputConnection(points->GetOutputPort());
    normals->SetSampleSize(sampleSize);
    normals->SetNormalOrientationToGraphTraversal();
    normals->Update();

    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkGlyph3D> glyph3D;
    MakeGlyphs(normals->GetOutput(), radius * 0.2, glyph3D.GetPointer());

    vtkNew<vtkPolyDataMapper> glyph3DMapper;
    glyph3DMapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkNew<vtkActor> glyph3DActor;
    glyph3DActor->SetMapper(glyph3DMapper);
    glyph3DActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Banana").GetData());

    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(1.0);
    sphere->SetThetaResolution(41);
    sphere->SetPhiResolution(21);

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("Tomato").GetData());

    // Create graphics stuff
    //
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("NormalEstimation");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    // Add the actors to the renderer, set the background and size
    //
    renderer->AddActor(glyph3DActor);
    renderer->AddActor(sphereActor);

    // Generate an interesting view
    //
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(0);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->GetActiveCamera()->Dolly(1.0);
    renderer->ResetCameraClippingRange();

    renderWindow->Render();
    interactor->Initialize();
    interactor->Start();

    return EXIT_SUCCESS;
}
namespace {
    void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph)
    {
        // Source for the glyph filter
        vtkNew<vtkArrowSource> arrow;
        arrow->SetTipResolution(16);
        arrow->SetTipLength(0.3);
        arrow->SetTipRadius(0.1);

        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetInputData(src);
        glyph->SetVectorModeToUseNormal();
        glyph->SetScaleModeToScaleByVector();
        glyph->SetScaleFactor(size);
        glyph->OrientOn();
        glyph->Update();
    }
} // namespace

#endif // TEST21

#ifdef TEST22

 // https://blog.csdn.net/minmindianzi/article/details/87071213

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkPlane.h>
#include <vtkSphereSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkGlyph3DMapper.h>
#include <vtkAxesActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

int main(int, char* [])
{
    vtkSmartPointer<vtkPlane> plane =
        vtkSmartPointer<vtkPlane>::New();
    double origin[] = { 0, 0, 0 };
    double normal[] = { 0, 0, 1 };
    plane->SetOrigin(origin);
    plane->SetNormal(normal);

    double p[] = { 11.0, 8.3, 5.7 };
    double projected[3];
    plane->ProjectPoint(p, origin, normal, projected);

    char outputMsg[256] = { 0 };
    //sprintf_s(outputMsg, sizeof(outputMsg), "p(%f, %f, %f)", p[0], p[1], p[2]);
    sprintf_s(outputMsg, sizeof(outputMsg), "p(%f, %f, %f)\nprojected(%f, %f, %f)", p[0], p[1], p[2],
        projected[0], projected[1], projected[2]);

    vtkSmartPointer<vtkTextActor> textActor =
        vtkSmartPointer<vtkTextActor>::New();
    textActor->SetPosition2(100, 40);
    textActor->GetTextProperty()->SetFontSize(24);
    textActor->GetTextProperty()->SetColor(0, 1, 0);
    textActor->SetInput(outputMsg);

    vtkSmartPointer<vtkSphereSource> pointsSphere =
        vtkSmartPointer<vtkSphereSource>::New();
    pointsSphere->SetPhiResolution(21);
    pointsSphere->SetThetaResolution(21);
    pointsSphere->SetRadius(.5);

    vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(p);
    points->InsertNextPoint(projected);

    vtkSmartPointer<vtkPolyData> pointsData =
        vtkSmartPointer<vtkPolyData>::New();
    pointsData->SetPoints(points);

    vtkSmartPointer<vtkGlyph3DMapper> pointsMapper =
        vtkSmartPointer<vtkGlyph3DMapper>::New();
    pointsMapper->SetInputData(pointsData);
    pointsMapper->SetSourceConnection(pointsSphere->GetOutputPort());

    vtkSmartPointer<vtkActor> pointsActor =
        vtkSmartPointer<vtkActor>::New();
    pointsActor->SetMapper(pointsMapper);
    pointsActor->GetProperty()->SetColor(1, 0, 0);

#define LINE_LEN 10.
    vtkSmartPointer<vtkAxesActor> axesActor =
        vtkSmartPointer<vtkAxesActor>::New();
    axesActor->SetPosition(0, 0, 0);
    axesActor->SetTotalLength(LINE_LEN, LINE_LEN, LINE_LEN);
    axesActor->SetShaftType(0);
    axesActor->SetAxisLabels(0);
    axesActor->SetCylinderRadius(0.02);

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(600, 600);
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
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
            //Superclass::OnLeftButtonUp();
        }

        void OnLeftButtonDown() override
        {
            std::cout << "left button down\n";
            //Superclass::OnLeftButtonDown();
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

int main(int, char* [])
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

#ifdef TEST24

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkScalarBarActor.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkViewport.h>
#include <vtkCommand.h>
#include <vtkScalarBarWidget.h>

class Callback :
    public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }
    virtual void Execute(vtkObject* caller, unsigned long x, void* y)
    {
        if (auto interactor = vtkRenderWindowInteractor::SafeDownCast(caller))
        {
            auto size = interactor->GetRenderWindow()->GetSize();
            std::cout << "Window size : " << size[0] << ',' << size[1] << '\t';

            std::cout << "Mouse pos : " << interactor->GetEventPosition()[0] << ',' << interactor->GetEventPosition()[1] << '\n';
        }

        if (m_bar && m_renderer)
        {
            std::cout << "----------------------\n";

            // 色卡的色带部分在渲染窗口中的像素位置
            int rect[4]{ 0 };
            m_bar->GetScalarBarRect(rect, m_renderer);
            std::cout << "ScalarBarRect : " << rect[0] << '\t' << rect[1] << '\t' << rect[2] << '\t' << rect[3] << '\n';

            auto range = m_bar->GetLookupTable()->GetRange();
            std::cout << "Range: " << range[0] << '\t' << range[1] << '\n';

            // 获取指定scalar在颜色映射表中的索引
            if (auto lut = vtkLookupTable::SafeDownCast(m_bar->GetLookupTable()))
            {
                double scalar = 10.0;
                auto index = lut->GetIndex(scalar);
                auto numColors = lut->GetNumberOfColors();
                auto indexLu = lut->GetIndexedLookup();
                std::cout << "indexedLookup: " << indexLu << "\tcolor num: " << numColors << "\tscalar: " << scalar << "\tindex: " << index << '\n';
            }
        }
    }

    void SetScalarBar(vtkSmartPointer<vtkScalarBarActor> bar)
    {
        m_bar = bar;
    }

    void SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
    {
        m_renderer = renderer;
    }

private:
    vtkSmartPointer<vtkScalarBarActor> m_bar{ nullptr };
    vtkSmartPointer<vtkRenderer> m_renderer{ nullptr };
};

#define line_fill  // 上下左右四视图，格点格心数据，线框面显示模式的区别
//#define scalar_bar    // 色卡颜色查找表的使用


int main(int, char* [])
{

#ifdef line_fill

    // 顶点，共有40个顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i + 0, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i + 0, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    // 多边形拓扑，共有27个四边形
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i + 0,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 线段拓扑，共有66条线段
    vtkSmartPointer<vtkCellArray> cell_line = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 37; i += 3)
    {
        cell_line->InsertNextCell({ i + 0, i + 1 });
        cell_line->InsertNextCell({ i + 1, i + 2 });
        cell_line->InsertNextCell({ i + 2, i + 3 });
    }
    for (long long i = 0; i < 33; i += 4)
    {
        cell_line->InsertNextCell({ i + 0,i + 4 });
        cell_line->InsertNextCell({ i + 1,i + 5 });
        cell_line->InsertNextCell({ i + 2,i + 6 });
        cell_line->InsertNextCell({ i + 3,i + 7 });
    }

    const int numPoints = 40;
    const int numPolys = 27;
    const int numLines = 66;

    // 为每个顶点设置标量属性
    vtkNew<vtkFloatArray> scalar_points;
    scalar_points->SetNumberOfValues(numPoints);
    for (int i = 0; i < numPoints - 3; i += 4)
    {
        // 为属性数组中的每个元素设置标量值（这个标量值可以当作颜色值）
        scalar_points->SetValue(i + 0, i);
        scalar_points->SetValue(i + 1, i);
        scalar_points->SetValue(i + 2, i);
        scalar_points->SetValue(i + 3, i);
    }

    // 为每个多边形设置标量属性
    vtkNew<vtkFloatArray> scalar_cell_poly;
    scalar_cell_poly->SetNumberOfValues(numPolys);
    for (int i = 0; i < numPolys; i += 3)
    {
        scalar_cell_poly->SetValue(i + 0, i);
        scalar_cell_poly->SetValue(i + 1, i);
        scalar_cell_poly->SetValue(i + 2, i);
    }

    // 为每条线段设置标量属性
    vtkNew<vtkFloatArray> scalar_cell_line;
    scalar_cell_line->SetNumberOfValues(numLines);
    for (long long i = 0; i < 37; i += 3)
    {
        scalar_cell_line->SetValue(i + 0, i);
        scalar_cell_line->SetValue(i + 1, i);
        scalar_cell_line->SetValue(i + 2, i);
    }
    for (long long i = 30; i < 63; i += 4)
    {
        scalar_cell_line->SetValue(i + 0, i - 30);
        scalar_cell_line->SetValue(i + 1, i - 30);
        scalar_cell_line->SetValue(i + 2, i - 30);
        scalar_cell_line->SetValue(i + 3, i - 30);
    }

    // 多边形，格点数据
    vtkNew<vtkPolyData> polyData_poly_scalarPoint;
    polyData_poly_scalarPoint->SetPoints(points);
    polyData_poly_scalarPoint->SetPolys(cell_poly);
    polyData_poly_scalarPoint->GetPointData()->SetScalars(scalar_points);

    // 线段，格点数据
    vtkNew<vtkPolyData> polyData_line_scalarPoint;
    polyData_line_scalarPoint->SetPoints(points);
    polyData_line_scalarPoint->SetLines(cell_line);
    polyData_line_scalarPoint->GetPointData()->SetScalars(scalar_points);

    // 多边形，格心数据
    vtkNew<vtkPolyData> polyData_poly_scalarCell;
    polyData_poly_scalarCell->SetPoints(points);
    polyData_poly_scalarCell->SetPolys(cell_poly);
    polyData_poly_scalarCell->GetCellData()->SetScalars(scalar_cell_poly);

    // 线段，格心数据
    vtkNew<vtkPolyData> polyData_line_scalarCell;
    polyData_line_scalarCell->SetPoints(points);
    polyData_line_scalarCell->SetLines(cell_line);
    polyData_line_scalarCell->GetPointData()->SetScalars(scalar_cell_line);

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->SetRampToSQRT();
    hueLut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapperPoly_scalarCell;
    mapperPoly_scalarCell->SetInputData(polyData_poly_scalarCell);
    mapperPoly_scalarCell->SetLookupTable(hueLut);
    mapperPoly_scalarCell->SetScalarRange(scalar_cell_poly->GetRange());

    vtkNew<vtkPolyDataMapper> mapperLine_scalarCell;
    mapperLine_scalarCell->SetInputData(polyData_line_scalarCell);
    mapperLine_scalarCell->SetLookupTable(hueLut);
    mapperLine_scalarCell->SetScalarRange(scalar_cell_line->GetRange());

    vtkNew<vtkPolyDataMapper> mapperPoly_scalarPoint;
    mapperPoly_scalarPoint->SetInputData(polyData_poly_scalarPoint);
    mapperPoly_scalarPoint->SetLookupTable(hueLut);
    mapperPoly_scalarPoint->SetScalarRange(scalar_points->GetRange());

    vtkNew<vtkPolyDataMapper> mapperLine_scalarPoint;
    mapperLine_scalarPoint->SetInputData(polyData_line_scalarPoint);
    mapperLine_scalarPoint->SetLookupTable(hueLut);
    mapperLine_scalarPoint->SetScalarRange(scalar_points->GetRange());

    vtkNew<vtkActor> actorPolyPoint;
    vtkNew<vtkActor> actorPolyCell;
    vtkNew<vtkActor> actorLinePoint;
    vtkNew<vtkActor> actorLineCell;
    actorPolyPoint->SetMapper(mapperPoly_scalarPoint);
    actorPolyCell->SetMapper(mapperPoly_scalarCell);
    actorLinePoint->SetMapper(mapperLine_scalarPoint);
    actorLineCell->SetMapper(mapperLine_scalarCell);

    // 以点，线框，面方式显示
    actorPolyPoint->GetProperty()->SetRepresentationToWireframe();
    //actorPolyPoint->GetProperty()->SetRepresentationToSurface();
    //actorPolyPoint->GetProperty()->SetRepresentationToPoints();
    //actorPolyCell->GetProperty()->SetRepresentationToWireframe();
    actorPolyPoint->GetProperty()->EdgeVisibilityOn();
    actorPolyPoint->GetProperty()->SetEdgeColor(1, 1, 1);

    // 设置云图的颜色
    mapperPoly_scalarCell->ScalarVisibilityOff();
    actorPolyCell->GetProperty()->SetColor(1, 1, 0);
    mapperPoly_scalarCell->ScalarVisibilityOn();

    vtkNew<vtkRenderer> topLeftRenderer;
    vtkNew<vtkRenderer> topRightRenderer;
    vtkNew<vtkRenderer> bottomLeftRenderer;
    vtkNew<vtkRenderer> bottomRightRenderer;
    topLeftRenderer->AddActor(actorPolyPoint);
    topRightRenderer->AddActor(actorPolyCell);
    bottomLeftRenderer->AddActor(actorLinePoint);
    bottomRightRenderer->AddActor(actorLineCell);

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(topLeftRenderer);
    topLeftRenderer->SetViewport(0, 0.5, 0.5, 1);
    window->AddRenderer(topRightRenderer);
    topRightRenderer->SetViewport(0.5, 0.5, 1, 1);
    window->AddRenderer(bottomLeftRenderer);
    bottomLeftRenderer->SetViewport(0, 0, 0.5, 0.5);
    window->AddRenderer(bottomRightRenderer);
    bottomRightRenderer->SetViewport(0.5, 0, 1, 0.5);


    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(window);
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    window->SetSize(800, 600);
    window->Render();
    renderWindowInteractor->Start();

#endif // line_fill

#ifdef scalar_bar

    std::vector<float> vertices{
        0.,0.,
        0.,1.,
        0.,2.,
        0.,3.,

        1.,3.,
        1.,2.,
        1.,1.,
        1.,0.,

        2.,0.,
        2.,1.,
        2.,2.,
        2.,3.,

        3.,3.,
        3.,2.,
        3.,1.,
        3.,0.
    };

    std::vector<int> indicesPoly{
        0,1,6,7,
        1,2,5,6,
        2,3,4,5,

        4,5,10,11,
        5,6,9,10,
        6,7,8,9,

        8,9,14,15,
        9,10,13,14,
        10,11,12,13,
    };

    std::vector<float> fields{
        1.0f,
        1.0f,
        1.0f,
        1.0f,

        2.0f,
        2.0f,
        2.0f,
        2.0f,

        3.0f,
        3.0f,
        3.0f,
        3.0f,

        4.0f,
        4.0f,
        4.0f,
        4.0f,
    };

    auto GenPolyData = [vertices, indicesPoly, fields]()
    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cellsLine;
        vtkNew<vtkCellArray> cellsPoly;

        for (size_t i = 0; i < vertices.size(); i += 2)
        {
            points->InsertNextPoint(vertices[i], vertices[i + 1], 0.0);
        }

        for (size_t i = 0; i < indicesPoly.size(); i += 4)
        {
            cellsPoly->InsertNextCell({ indicesPoly[i],indicesPoly[i + 1], indicesPoly[i + 2],indicesPoly[i + 3] });
        }
        polyData->SetPoints(points);
        polyData->SetPolys(cellsPoly);

        vtkNew<vtkFloatArray> scalars;
        for (size_t i = 0; i < 16; ++i)
        {
            scalars->InsertNextValue(fields[i]);
        }

        polyData->GetPointData()->SetScalars(scalars);

        return polyData;
    };

    //-------------------------------------------------------------------

    // 创建颜色查找表
    vtkNew<vtkLookupTable> hueLut;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfColors\t" << hueLut->GetNumberOfColors() << '\n';
    std::cout << "GetNumberOfTableValues\t" << hueLut->GetNumberOfTableValues() << '\n';
    std::cout << "GetNumberOfColorsMinValue\t" << hueLut->GetNumberOfColorsMinValue() << '\n';
    std::cout << "GetNumberOfColorsMaxValue\t" << hueLut->GetNumberOfColorsMaxValue() << '\n';
    std::cout << "GetNumberOfAnnotatedValues\t" << hueLut->GetNumberOfAnnotatedValues() << '\n';
    std::cout << "GetNumberOfAvailableColors\t" << hueLut->GetNumberOfAvailableColors() << '\n';

    hueLut->SetNumberOfColors(10);          // 指定颜色查找表中有多少种颜色，默认256
    hueLut->SetHueRange(0.67, 0.0);         // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    //hueLut->SetRampToSQRT();
    //hueLut->UseAboveRangeColorOn();
    //hueLut->SetAboveRangeColor(0.0, 1.0, 0.0, 0.0);
    //hueLut->SetBelowRangeColor(1.0, 1.0, 0.0, 0.0);
    //hueLut->SetNumberOfTableValues(20);
    //hueLut->SetAlphaRange(1.0, 1.0);
    //hueLut->SetValueRange(1, 1);
    //hueLut->SetSaturationRange(1, 1);
    hueLut->SetRange(GenPolyData()->GetPointData()->GetScalars()->GetRange());
    hueLut->Build();

    //---------------------------------------------------------

    vtkNew<vtkPolyDataMapper> mapper;
    std::cout << "------------------------------------------\n";
    std::cout << "GetScalarVisibility : " << mapper->GetScalarVisibility() << '\n';
    std::cout << "GetColorModeAsString : " << mapper->GetColorModeAsString() << '\n';
    std::cout << "GetScalarModeAsString" << mapper->GetScalarModeAsString() << '\n';

    //mapper->SeamlessUOn();
    //mapper->SeamlessVOn();
    mapper->SetInputData(GenPolyData());
    mapper->SetScalarRange(mapper->GetInput()->GetPointData()->GetScalars()->GetRange());            // 设置标量值的范围
    mapper->ScalarVisibilityOn(); // 默认开启标量属性显示

    mapper->SetLookupTable(hueLut);
    //mapper->InterpolateScalarsBeforeMappingOn();  // 设置颜色映射模式为梯度平滑，类似纹理贴图
    mapper->InterpolateScalarsBeforeMappingOff();   // 颜色平滑过度

    //mapper->SetColorMode(1);
    //mapper->SetColorModeToMapScalars();        // 无论变量数据是何种类型，该方法都通过查询表对标量数据进行映射
    //mapper->SetColorModeToDefault();           // 默认的映射器行为，即把unsigned char类型的标量属性数据当作颜色值，不执行隐式。对于其他类型的标量数据，将通过查询表映射。
    //mapper->SetColorModeToDirectScalars();

    //mapper->SetScalarModeToUseCellData();
    //mapper->SetScalarModeToUseCellFieldData();
    //mapper->SetScalarModeToUsePointFieldData();
    //mapper->SetScalarModeToUseFieldData();
    //mapper->SetScalarModeToUsePointData();

    //---------------------------------------------------------

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    std::cout << "------------------------------------------\n";
    std::cout << "Interpolation\t" << actor->GetProperty()->GetInterpolationAsString() << '\n';
    actor->SetMapper(mapper);

    // 设置着色方法
    //actor->GetProperty()->SetInterpolation(0);
    //actor->GetProperty()->SetInterpolationToFlat();
    //actor->GetProperty()->SetInterpolationToPBR();
    //actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetInterpolationToPhong();

    //---------------------------------------------------------
    vtkNew<vtkScalarBarActor> scalarBar;
    std::cout << "------------------------------------------\n";
    std::cout << "GetNumberOfLabels \t" << scalarBar->GetNumberOfLabels() << '\n';
    std::cout << "GetNumberOfConsumers\t" << scalarBar->GetNumberOfConsumers() << '\n';
    std::cout << "GetNumberOfLabelsMinValue\t" << scalarBar->GetNumberOfLabelsMinValue() << '\n';
    std::cout << "GetNumberOfLabelsMaxValue\t" << scalarBar->GetNumberOfLabelsMaxValue() << '\n';
    std::cout << "GetNumberOfPaths\t" << scalarBar->GetNumberOfPaths() << '\n';
    std::cout << "GetNumberOfColors\t" << vtkLookupTable::SafeDownCast(mapper->GetLookupTable())->GetNumberOfColors() << '\n';
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';

    scalarBar->SetLookupTable(mapper->GetLookupTable());
    scalarBar->SetTextPad(5);           //设置文本框周围的填充量，就是标签文本距离颜色映射表的距离
    scalarBar->SetPosition(.2, .2);     //设置位置，左下角
    scalarBar->SetPosition2(.8, .8);    //右上角，是相对最下角的位置，不是实际位置，还包括Title的大小
    std::cout << "GetPostion\t" << scalarBar->GetPosition()[0] << '\t' << scalarBar->GetPosition()[1] << '\n';
    std::cout << "GetPostion2\t" << scalarBar->GetPosition2()[0] << '\t' << scalarBar->GetPosition2()[1] << '\n';
    std::cout << "GetBarRatio\t" << scalarBar->GetBarRatio() << '\n'; // 获取颜色条相对于小部件框架的厚度。

    std::cout << "GetPositionCoordinate\t" << scalarBar->GetPositionCoordinate()->GetValue()[0] << '\t' << scalarBar->GetPositionCoordinate()->GetValue()[1] << '\n';
    std::cout << "GetPosition2Coordinate\t" << scalarBar->GetPosition2Coordinate()->GetValue()[0] << '\t' << scalarBar->GetPosition2Coordinate()->GetValue()[1] << '\n';

    std::cout << "GetHeight\t" << scalarBar->GetHeight() << '\n';
    std::cout << "GetWidth\t" << scalarBar->GetWidth() << '\n';

    //scalarBar->SetUseOpacity(1);          //设置不透明度
    //scalarBar->UseOpacityOn();
    //scalarBar->SetLayerNumber(0);
    //scalarBar->SetTextureGridWidth(10);
    //scalarBar->SetDrawAnnotations(0);

    // 色卡标题
    scalarBar->SetTitle("U (m/s)");
    scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
    scalarBar->GetTitleTextProperty()->SetFontFamilyToArial();
    scalarBar->GetTitleTextProperty()->SetFontSize(20);

    std::cout << "------------------------------------------\n";
    std::cout << "GetTitleRatio\t" << scalarBar->GetTitleRatio() << '\n';
    std::cout << "GetTitleRatioMaxValue\t" << scalarBar->GetTitleRatioMaxValue() << '\n';
    std::cout << "GetTitleRatioMinValue\t" << scalarBar->GetTitleRatioMinValue() << '\n';

    // 色卡标签
    scalarBar->UnconstrainedFontSizeOn(); // 设置标题和标签的字体大小是否不受限制。默认设置为禁用。
    scalarBar->SetNumberOfLabels(5);      // 标签个数，和颜色个数没关系，颜色个数由SetLookupTable的NumberOfColors决定
    scalarBar->SetLabelFormat("%5.3f");
    //scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    //scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    //scalarBar->GetLabelTextProperty()->SetFontFamilyToCourier();
    //scalarBar->GetLabelTextProperty()->SetFontSize(20);
    std::cout << "------------------------------------------\n";


    //设置色卡最大颜色分段个数，如果LookupTable大于该数，也只能显示MaximumNumberOfColors个颜色。太小会导致两头的颜色不显示，默认64
    scalarBar->SetMaximumNumberOfColors(10);

    scalarBar->SetMaximumWidthInPixels(80);
    scalarBar->SetMaximumHeightInPixels(900);


    vtkNew<vtkTextProperty> textProperty;
    textProperty->SetColor(0, 0, 0);
    textProperty->SetFontFamilyToTimes();
    scalarBar->SetLabelTextProperty(textProperty);

    //scalarBar->VisibilityOff(); // 不显示色卡


    //---------------------------------------------------------

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 设置回调函数，用来返回色卡的位置
    auto callback = Callback::New();
    callback->SetScalarBar(scalarBar);
    callback->SetRenderer(renderer);
    renderWindowInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, callback);

    // 添加色卡小部件，可以使用鼠标拖动色卡，缩放色卡
    vtkNew<vtkScalarBarWidget> scalarBarWidget;
    scalarBarWidget->SetInteractor(renderWindowInteractor);
    scalarBarWidget->SetScalarBarActor(scalarBar);
    scalarBarWidget->On();

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    //renderer->AddActor2D(scalarBar);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

#endif scalar_bar

    return 0;
}

#endif // TEST24

#ifdef TEST25

#include <vtkCamera.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkQuadricDecimation.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>

#include <array>
#include <iostream>

// 四条线段组成一个正方形
namespace
{
    std::array<float, 16 * 3> vertices{
        0,0,0,
        0,1,0,
        0,2,0,
        0,3,0,

        1,0,0,
        1,1,0,
        1,2,0,
        1,3,0,

        2,0,0,
        2,1,0,
        2,2,0,
        2,3,0,

        3,0,0,
        3,1,0,
        3,2,0,
        3,3,0,
    };

    std::array<long long, 9 * 4> indices1{
        0,1,5,4,
        1,2,6,5,
        2,3,7,6,

        4,5,9,8,
        5,6,10,9,
        6,7,11,10,

        8,9,13,12,
        9,10,14,13,
        10,11,15,14
    };

    std::array<long long, 18 * 3> indices2{
        0,1,5,
        0,5,4,

        1,2,6,
        1,6,5,

        2,3,7,
        2,7,6,

        4,5,9,
        4,9,8,

        5,6,10,
        5,10,9,

        6,7,11,
        6,11,10,

        8,9,13,
        8,13,12,

        9,10,14,
        9,14,13,

        10,11,15,
        10,15,14
    };
}

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
            cells->InsertNextCell({ indices2[i],indices2[i + 1] ,indices2[i + 2] });
        }
    }
    else if (type == 4)
    {
        for (size_t i = 0; i < indices1.size(); i += 4)
        {
            cells->InsertNextCell({ indices1[i],indices1[i + 1] ,indices1[i + 2],indices1[i + 3] });
        }
    }


    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    return polyData;
}

int main()
{
    auto trianglePoly = GenPolyData(3);
    auto quad = GenPolyData(4);

    // 将多边形转为三角形
    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputData(quad);
    triangleFilter->Update();

    // 减少网格中的三角形
    // 例如原来网格有十个三角形，经过处理之后只有五个三角形，但是绘制的图像没有变化
    // 输入必须是三角形，如果不是可以用vtkTriangleFilter进行三角化
    vtkNew<vtkQuadricDecimation> decimate;
    decimate->SetInputData(triangleFilter->GetOutput());
    //decimate->SetInputData(trianglePoly);
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

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(decimated);

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);
    cubeActor->GetProperty()->SetRepresentationToWireframe();

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST25

#ifdef TEST26

#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkTexture.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

int main()
{
    // 加载图片
    //vtkSmartPointer<vtkBMPReader>bmpReader = vtkSmartPointer<vtkBMPReader>::New();
    //bmpReader->SetFileName("test.bmp");
    vtkSmartPointer<vtkJPEGReader>jpgReader = vtkSmartPointer<vtkJPEGReader>::New();
    jpgReader->SetFileName("resource/test1.jpg");
    // 生成纹理
    vtkSmartPointer<vtkTexture>texture = vtkSmartPointer<vtkTexture>::New();
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
    //cylinderActor->GetProperty()->SetColor(1.0, 0.0, 0.0);

    // render
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(cylinderActor);
    renderer->SetBackground(.1, .2, .3);
    //renderer->SetBackground2(1, 0, 0);

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
    vtkSmartPointer<vtkCamera>myCamera = vtkSmartPointer<vtkCamera>::New();
    myCamera->SetClippingRange(0.0475, 2.3786); //前后裁剪平面
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

#ifdef TEST27

// https://kitware.github.io/vtk-examples/site/Cxx/Picking/HighlightSelectedPoints/

#include <vtkActor.h>
#include <vtkAreaPicker.h>
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


#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

namespace {
    // Define interaction style
    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

        InteractorStyle()
        {
            this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            this->SelectedActor = vtkSmartPointer<vtkActor>::New();
            this->SelectedActor->SetMapper(SelectedMapper);
        }

        virtual void OnLeftButtonUp() override
        {
            vtkNew<vtkNamedColors> colors;

            // Forward events
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

            vtkPlanes* frustum =
                static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
                ->GetFrustum();

            vtkNew<vtkExtractGeometry> extractGeometry;
            extractGeometry->SetImplicitFunction(frustum);
            extractGeometry->SetInputData(this->Points);
            extractGeometry->Update();

            vtkNew<vtkVertexGlyphFilter> glyphFilter;
            glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
            glyphFilter->Update();

            vtkPolyData* selected = glyphFilter->GetOutput();
            std::cout << "Selected " << selected->GetNumberOfPoints() << " points."
                << std::endl;
            std::cout << "Selected " << selected->GetNumberOfCells() << " cells."
                << std::endl;
            this->SelectedMapper->SetInputData(selected);
            this->SelectedMapper->ScalarVisibilityOff();

            vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
                selected->GetPointData()->GetArray("OriginalIds"));
            for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
            {
                std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
            }

            this->SelectedActor->GetProperty()->SetColor(
                colors->GetColor3d("Red").GetData());
            this->SelectedActor->GetProperty()->SetPointSize(5);

            this->CurrentRenderer->AddActor(SelectedActor);
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }

        void SetPoints(vtkSmartPointer<vtkPolyData> points)
        {
            this->Points = points;
        }

    private:
        vtkSmartPointer<vtkPolyData> Points;
        vtkSmartPointer<vtkActor> SelectedActor;
        vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
    };

    vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(20);
    pointSource->Update();

    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputConnection(pointSource->GetOutputPort());
#if VTK890
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
#else
    idFilter->SetIdsArrayName("OriginalIds");
#endif
    idFilter->Update();

    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    vtkPolyData* input = surfaceFilter->GetOutput();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(input);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetColor(colors->GetColor3d("Green").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightSelectedPoints");

    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    renderWindow->Render();

    vtkNew<InteractorStyle> style;
    style->SetPoints(input);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST27

#ifdef TEST28

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <map>

namespace {
    std::map<vtkSmartPointer<vtkActor>, int> theActors;

    // Handle mouse events
    class MouseInteractorHighLightActor : public vtkInteractorStyleTrackballCamera
    {
    public:
        static MouseInteractorHighLightActor* New();
        vtkTypeMacro(MouseInteractorHighLightActor,
            vtkInteractorStyleTrackballCamera);

        MouseInteractorHighLightActor()
        {
            LastPickedActor = NULL;
            LastPickedProperty = vtkProperty::New();
        }
        virtual ~MouseInteractorHighLightActor()
        {
            LastPickedProperty->Delete();
        }
        virtual void OnLeftButtonDown() override
        {
            std::cout << "start picking\t";
            vtkNew<vtkNamedColors> colors;

            int* clickPos = this->GetInteractor()->GetEventPosition();

            // Pick from this location.
            vtkNew<vtkPropPicker> picker;
            picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

            // If we picked something before, reset its property
            if (this->LastPickedActor)
            {
                // 用来将上一次拾取的图元恢复颜色
                //std::cout << "restore color\t";
                this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);
            }
            this->LastPickedActor = picker->GetActor();
            if (this->LastPickedActor)
            {
                std::cout << "picked actor:\t" << theActors[this->LastPickedActor] << '\t';

                //std::cout << "set color\t";
                // Save the property of the picked actor so that we can

                // 将上一次拾取的图元恢复颜色
                // restore it next time 
                this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());

                // 将这一次拾取的图元设置为红色
                // Highlight the picked actor by changing its properties
                this->LastPickedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

                //std::cout << LastPickedActor->GetClassName();


                //this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
                //this->LastPickedActor->GetProperty()->SetSpecular(0.0);
                //this->LastPickedActor->GetProperty()->EdgeVisibilityOn();
            }

            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

            std::cout << "end picking\n";
        }

    private:
        vtkActor* LastPickedActor;
        vtkProperty* LastPickedProperty;
    };

    vtkStandardNewMacro(MouseInteractorHighLightActor);
} // namespace

// Execute application.
int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    int numberOfSpheres = 10;

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(640, 480);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightPickedActor");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Set the custom type to use for interaction.
    vtkNew<MouseInteractorHighLightActor> style;
    style->SetDefaultRenderer(renderer);

    renderWindowInteractor->SetInteractorStyle(style);

    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);
    for (int i = 0; i < 10; ++i)
    {
        vtkNew<vtkSphereSource> source;
        double x, y, z, radius;
        // random position and radius
        x = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        y = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        z = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        radius = randomSequence->GetRangeValue(0.5, 1.0);
        randomSequence->Next();
        source->SetRadius(radius);
        source->SetCenter(x, y, z);
        source->SetPhiResolution(11);
        source->SetThetaResolution(21);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(source->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        double r, g, b;
        r = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        g = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        b = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        actor->GetProperty()->SetDiffuseColor(r, g, b);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetSpecular(0.5);
        actor->GetProperty()->SetSpecularColor(
            colors->GetColor3d("White").GetData());
        actor->GetProperty()->SetSpecularPower(30.0);

        theActors.emplace(actor, i);
        renderer->AddActor(actor);
    }

    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST28

#ifdef TEST29

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkSphereSource.h>

#include <vtkAreaPicker.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>
#include <vtkPropPicker.h>
#include <vtkPicker.h>
#include <vtkWorldPointPicker.h>

#include <vtkAssembly.h>

#include <map>

namespace {

    std::map<vtkSmartPointer<vtkActor>, int> theActors;

    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

        virtual void OnLeftButtonUp() override
        {
            std::cout << "------------------------------------\n";

            if (this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];
                std::cout << "X : " << posX << "\tY : " << posY << '\n';

                // https://blog.csdn.net/q610098308/article/details/125433189 vtk拾取器汇总

                // vtkAreaPicker
                if (0)
                {
                    vtkNew<vtkAreaPicker> areaPicker;
                    this->Interactor->SetPicker(areaPicker);
                    // 拾取鼠标弹起位置的+-20矩形区域
                    double bound = 20;
                    areaPicker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);

                    auto pickPosition = areaPicker->GetPickPosition();
                    auto selectionPoint = areaPicker->GetSelectionPoint();

                    std::cout << "pickPosition\t" << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';
                    std::cout << "selectionPoint\t" << selectionPoint[0] << '\t' << selectionPoint[1] << '\t' << selectionPoint[2] << '\n';

                }

                // vtkAreaPicker + vtkAssembly
                if (0)
                {
                    vtkNew<vtkAreaPicker> areaPicker;
                    this->Interactor->SetPicker(areaPicker);
                    double bound = 20;
                    areaPicker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);
                    auto props = areaPicker->GetProp3Ds();
                    props->InitTraversal();
                    auto num = props->GetNumberOfItems();
                    std::cout << "actor number : " << num << '\n';
                    if (num == 1)
                    {
                        auto prop = props->GetNextProp3D();
                        auto assembly = vtkAssembly::SafeDownCast(prop);
                        auto parts = assembly->GetParts();
                        auto num2 = parts->GetNumberOfItems();

                        for (size_t i = 0; i < num2; ++i)
                        {
                            auto actor = vtkActor::SafeDownCast(parts->GetNextProp3D());

                            for (auto [actor_, id_] : theActors)
                            {
                                if (actor == actor_)
                                {
                                    std::cout << "pick id : " << id_ << '\t';
                                }
                            }
                        }
                        std::cout << '\n';
                    }
                }

                // vtkPicker vtkAbstractPicker
                if (0)
                {
                    vtkAbstractPicker* abstractPicker = this->Interactor->GetPicker();
                    abstractPicker->Pick(posX, posY, 0, this->CurrentRenderer);

                    auto pickPosition = abstractPicker->GetPickPosition();     // 模型坐标，不是世界坐标，如果多个actor叠加，返回前面的actor坐标
                    auto selectionPoint = abstractPicker->GetSelectionPoint(); // 窗口坐标

                    std::cout << "pickPosition\t" << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';
                    std::cout << "selectionPoint\t" << selectionPoint[0] << '\t' << selectionPoint[1] << '\t' << selectionPoint[2] << '\n';
                }

                // vtkPropPicker
                if (1)
                {
                    vtkNew<vtkPropPicker> propPicker;
                    this->Interactor->SetPicker(propPicker);
                    vtkObject::GetGlobalWarningDisplay();

                    if (propPicker->Pick(posX, posY, 0, this->CurrentRenderer) != 0)
                    {
                        auto pickPosition = propPicker->GetPickPosition();
                        auto pickActor = propPicker->GetActor();
                        std::cout << "pickPostion : " << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';
                    }
                }

                // vtkCellPicker
                if (1)
                {
                    vtkNew<vtkCellPicker> cellPicker;
                    this->Interactor->SetPicker(cellPicker);

                    cellPicker->SetTolerance(0.01); // 相当于指定鼠标所在位置的范围
                    cellPicker->Pick(posX, posY, 0, this->CurrentRenderer);
                    cellPicker->GetPickPosition();
                    cellPicker->GetActor();
                    cellPicker->GetMapper();
                    auto cellId = cellPicker->GetCellId();
                    cellPicker->GetCellIJK();
                    cellPicker->GetDataSet();

                    if (cellId != -1)
                    {
                        std::cout << "pick cell id: " << cellId << '\n';
                    }
                }

                // vtkPointPicker
                if (1)
                {
                    vtkNew<vtkPointPicker> pointPicker;
                    this->Interactor->SetPicker(pointPicker);

                    pointPicker->SetTolerance(0.01);
                    pointPicker->Pick(posX, posY, 0, this->CurrentRenderer);
                    pointPicker->GetActor();
                    auto pointId = pointPicker->GetPointId();
                    pointPicker->GetPickPosition();
                    pointPicker->GetDataSet();

                    if (pointId != -1)
                    {
                        std::cout << "pick point id: " << pointId << '\n';
                    }
                }
            }

            Superclass::OnLeftButtonUp();
        }

    };
    vtkStandardNewMacro(InteractorStyle);
}

int main(int, char* [])
{
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

#if(1)
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);
    vtkNew<vtkAssembly> assembly;

    for (int i = 0; i < 10; ++i)
    {
        vtkNew<vtkSphereSource> source;
        double x, y, z, radius;
        x = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        y = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        z = randomSequence->GetRangeValue(-5.0, 5.0);
        randomSequence->Next();
        radius = randomSequence->GetRangeValue(0.5, 1.0);
        randomSequence->Next();
        source->SetRadius(radius);
        source->SetCenter(x, y, z);
        source->SetPhiResolution(11);
        source->SetThetaResolution(21);
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(source->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        double r, g, b;
        r = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        g = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        b = randomSequence->GetRangeValue(0.4, 1.0);
        randomSequence->Next();
        actor->GetProperty()->SetDiffuseColor(r, g, b);
        actor->GetProperty()->SetDiffuse(0.8);
        actor->GetProperty()->SetSpecular(0.5);
        actor->GetProperty()->SetSpecularColor(1, 1, 1);
        actor->GetProperty()->SetSpecularPower(30.0);

        theActors.emplace(actor, i);
        assembly->AddPart(actor);

        //renderer->AddActor(actor);
    }

    renderer->AddActor(assembly);
#else
    std::array<float, 4 * 3> vertices{
        10,10,0,
        20,10,0,
        20,20,0,
        10,20,0
    };

    std::array<long long, 4 * 2> indices{
        0,1,
        1,2,
        2,3,
        3,0
    };

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    for (size_t i = 0; i < indices.size(); i += 2)
    {
        cells->InsertNextCell({ indices[i],indices[i + 1] });
    }

    polyData->SetPoints(points);
    polyData->SetLines(cells);

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(polyData);

    //actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(cubeMapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetLineWidth(5);

    //vtkNew<vtkAssembly> assembly;
    //assembly->AddPart(actor);

    //renderer->AddActor(assembly);
    renderer->AddActor(actor);
#endif

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<InteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);


    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST29

#ifdef TEST30

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkCoordinate.h>
#include <vtkAbstractPicker.h>
#include <vtkCellLocator.h>
#include <vtkAreaPicker.h>
#include <vtkProp3DCollection.h>
#include <vtkRegularPolygonSource.h>
#include <vtkPropPicker.h>
#include <vtkPolygon.h>
#include <vtkCellPicker.h>
#include <vtkWorldPointPicker.h>

#include <vtkMatrix4x4.h>
#include <vtkMath.h>

#include <array>
#include <iostream>

// 四条线段组成一个正方形
namespace
{
    std::array<float, 4 * 3> vertices1{
        10,10,0,
        20,10,0,
        20,20,0,
        10,20,0
    };

    std::array<float, 4 * 3> vertices2{
        10,10,10,
        20,10,10,
        20,20,10,
        10,20,10
    };

    std::array<long long, 4 * 2> indices{
        0,1,
        1,2,
        2,3,
        3,0
    };

    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

        virtual void OnLeftButtonUp() override
        {
            std::cout << "------------------------------------\n";

            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                vtkNew<vtkAreaPicker> picker;
                this->Interactor->SetPicker(picker);
                double bound = 5;
                picker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);
                auto props = picker->GetProp3Ds();
                props->InitTraversal();
                std::cout << "pick number: " << props->GetNumberOfItems() << '\n';
            }

            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                vtkNew<vtkAreaPicker> picker;
                this->Interactor->SetPicker(picker);
                double bound = 5;
                picker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);
                auto props = picker->GetProp3Ds();
                props->InitTraversal();
                auto numPicked = props->GetNumberOfItems();
                std::cout << "pick number: " << numPicked << '\n';

                if (numPicked > 0)
                {
                    vtkNew<vtkCoordinate> coord;
                    coord->SetCoordinateSystemToDisplay();
                    coord->SetValue(posX, posY, 0.);
                    auto worldValue = coord->GetComputedWorldValue(this->CurrentRenderer);
                    std::cout << "world value\t" << worldValue[0] << '\t' << worldValue[1] << '\t' << worldValue[2] << '\n';

                    // 查找最近的点
                    auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
                    cellLocator->SetDataSet(m_polyData);
                    cellLocator->BuildLocator();

                    auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
                    double closestPoint[3];   //the coordinates of the closest point will be returned here
                    double closestPointDist2; //the squared distance to the closest point will be returned here
                    vtkIdType cellId;         //the cell id of the cell containing the closest point will be returned here
                    int subId;

                    cellLocator->FindClosestPoint(worldValue, closestPoint, assistCell, cellId, subId, closestPointDist2);
                    std::cout << "closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
                    std::cout << "CellId: " << cellId << std::endl;

                    vtkNew<vtkRegularPolygonSource> polygonSource;
                    polygonSource->GeneratePolygonOn();
                    polygonSource->SetNumberOfSides(30);
                    polygonSource->SetRadius(0.5);
                    polygonSource->SetCenter(worldValue);

                    vtkNew<vtkPolyDataMapper> mapper;
                    mapper->SetInputConnection(polygonSource->GetOutputPort());

                    vtkNew<vtkActor> actor;
                    actor->SetMapper(mapper);
                    actor->GetProperty()->SetColor(1, 0, 0);

                    this->CurrentRenderer->AddActor(actor);
                }
            }

            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                vtkNew<vtkPropPicker> picker;
                this->Interactor->SetPicker(picker);

                if (picker->Pick(posX, posY, 0, this->CurrentRenderer) != 0)
                {
                    auto pickPosition = picker->GetPickPosition();
                    std::cout << "pickPostion : " << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';

                    // 查找最近的点
                    auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
                    cellLocator->SetDataSet(m_polyData);
                    cellLocator->BuildLocator();

                    auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
                    double closestPoint[3];   //the coordinates of the closest point will be returned here
                    double closestPointDist2; //the squared distance to the closest point will be returned here
                    vtkIdType cellId;         //the cell id of the cell containing the closest point will be returned here
                    int subId;

                    cellLocator->FindClosestPoint(pickPosition, closestPoint, assistCell, cellId, subId, closestPointDist2);
                    std::cout << "closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
                    std::cout << "CellId: " << cellId << std::endl;

                    vtkNew<vtkRegularPolygonSource> polygonSource;
                    polygonSource->GeneratePolygonOn();
                    polygonSource->SetNumberOfSides(30);
                    polygonSource->SetRadius(0.5);
                    polygonSource->SetCenter(closestPoint);

                    vtkNew<vtkPolyDataMapper> mapper;
                    mapper->SetInputConnection(polygonSource->GetOutputPort());

                    vtkNew<vtkActor> actor;
                    actor->SetMapper(mapper);
                    actor->GetProperty()->SetColor(1, 0, 0);

                    this->CurrentRenderer->AddActor(actor);

                    vtkNew<vtkPolygon> polygon;
                }
            }

            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                std::cout << "X : " << posX << "\tY : " << posY << '\n';
                std::cout << "parallelScale: " << this->CurrentRenderer->GetActiveCamera()->GetParallelScale() << '\n';

                vtkNew<vtkCoordinate> coord;
                coord->SetCoordinateSystemToDisplay();
                //coord->SetCoordinateSystemToNormalizedDisplay();
                //coord->SetCoordinateSystemToView();
                coord->SetValue(posX, posY, 0.);
                auto displayValue = coord->GetComputedDisplayValue(this->CurrentRenderer);
                auto worldValue = coord->GetComputedWorldValue(this->CurrentRenderer);
                std::cout << "world value\t" << worldValue[0] << '\t' << worldValue[1] << '\t' << worldValue[2] << '\n';
                std::cout << "display  value\t" << displayValue[0] << '\t' << displayValue[1] << '\t' << displayValue[2] << '\n';

                //auto picker = this->Interactor->GetPicker();
                //picker->Pick(posX, posY, 0, this->CurrentRenderer);

                vtkNew<vtkAreaPicker> picker;
                this->Interactor->SetPicker(picker);
                double bound = 5;
                picker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);
                auto props = picker->GetProp3Ds();
                props->InitTraversal();
                std::cout << "pick number: " << props->GetNumberOfItems() << '\n';

                auto pickPosition = picker->GetPickPosition();     // 模型坐标系，返回的是拾取点在模型坐标系中的值
                auto selectionPoint = picker->GetSelectionPoint(); // 窗口坐标

                std::cout << "pickPosition\t" << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';
                std::cout << "selectionPoint\t" << selectionPoint[0] << '\t' << selectionPoint[1] << '\t' << selectionPoint[2] << '\n';

                auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
                cellLocator->SetDataSet(m_polyData);
                cellLocator->BuildLocator();

                //Find the closest points to TestPoint
                auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
                double closestPoint[3];   //the coordinates of the closest point will be returned here
                double closestPointDist2; //the squared distance to the closest point will be returned here
                vtkIdType cellId;         //the cell id of the cell containing the closest point will be returned here
                int subId;

                cellLocator->FindClosestPoint(worldValue, closestPoint, assistCell, cellId, subId, closestPointDist2);
                std::cout << "closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
                std::cout << "Squared distance: " << closestPointDist2 << std::endl; // 计算输入点到查找到的点之间的距离平方 worldValue <-> closestPoint
                std::cout << "CellId: " << cellId << std::endl;
                std::cout << "SubId: " << subId << '\n';

                {
                    std::cout << "***********************\n";
                    // 模型坐标转世界坐标
                    auto modelMat = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                    double modelPos[4]{ closestPoint[0],closestPoint[1], closestPoint[2], 0.0 };
                    auto worldValue = modelMat->MultiplyPoint(modelPos);

                    vtkNew<vtkMatrix4x4> matInvert;
                    vtkMatrix4x4::Invert(modelMat, matInvert);

                    vtkNew<vtkMatrix4x4> matTranspose;
                    vtkMatrix4x4::Transpose(modelMat, matTranspose);

                    auto modelPos1 = matInvert->MultiplyPoint(worldValue);
                    auto modelPos2 = matTranspose->MultiplyPoint(worldValue);

                    std::cout << modelPos[0] << ',' << modelPos[1] << ',' << modelPos[2] << '\n';
                    std::cout << modelPos1[0] << ',' << modelPos1[1] << ',' << modelPos1[2] << '\n';
                    std::cout << modelPos2[0] << ',' << modelPos2[1] << ',' << modelPos2[2] << '\n';
                    std::cout << "******************\n";
                }

                // 模型坐标转世界坐标
                //double ssss[4]{ closestPoint[0],closestPoint[1],closestPoint[2],0.0 };
                double ssss[4]{ 10,10,0.0,0.0 };
                auto mat = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                auto xxxx = mat->MultiplyPoint(ssss);

                // 世界坐标转屏幕坐标
                vtkNew<vtkCoordinate> coord1;
                coord1->SetCoordinateSystemToWorld();
                coord1->SetValue(xxxx);
                auto displayValue1 = coord1->GetComputedDisplayValue(this->CurrentRenderer);
                std::cout << "display  value\t" << displayValue1[0] << '\t' << displayValue1[1] << '\t' << displayValue1[2] << '\n';
                std::cout << std::hypot(displayValue1[0] - posX, displayValue1[1] - posY) << '\n'; // 鼠标点击的位置与获取到的距离最近的点之间的距离（屏幕坐标）

                vtkNew<vtkRegularPolygonSource> polygonSource;
                polygonSource->GeneratePolygonOff();
                polygonSource->SetNumberOfSides(30);
                polygonSource->SetRadius(0.5);
                polygonSource->SetCenter(closestPoint);

                vtkNew<vtkPolyDataMapper> mapper;
                mapper->SetInputConnection(polygonSource->GetOutputPort());

                vtkNew<vtkActor> actor;
                actor->SetMapper(mapper);
                actor->GetProperty()->SetColor(1, 0, 0);

                this->CurrentRenderer->AddActor(actor);
            }

            // vtkPropPicker
            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                vtkNew<vtkPropPicker> picker;
                this->Interactor->SetPicker(picker);

                if (picker->Pick(posX, posY, 0, this->CurrentRenderer) != 0)
                {
                    auto pickPosition = picker->GetPickPosition();
                    auto actor = picker->GetActor();

                    std::cout << actor << '\n';
                    std::cout << "pickPostion : " << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';

                    vtkNew<vtkRegularPolygonSource> polygonSource;
                    polygonSource->GeneratePolygonOff();
                    polygonSource->SetNumberOfSides(30);
                    polygonSource->SetRadius(0.5);
                    polygonSource->SetCenter(pickPosition);

                    vtkNew<vtkPolyDataMapper> mapper;
                    mapper->SetInputConnection(polygonSource->GetOutputPort());

                    //vtkNew<vtkActor> actor;
                    //actor->SetMapper(mapper);
                    //actor->GetProperty()->SetColor(1, 0, 0);

                    //this->CurrentRenderer->AddActor2D(actor);
                }

            }

            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                //std::cout << "X : " << posX << "\tY : " << posY << '\n';

                vtkNew<vtkCoordinate> coord;
                coord->SetCoordinateSystemToDisplay();
                coord->SetValue(posX, posY, 0.);
                auto worldValue = coord->GetComputedWorldValue(this->CurrentRenderer);
                //std::cout << "world value\t" << worldValue[0] << '\t' << worldValue[1] << '\t' << worldValue[2] << '\n';

                auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
                cellLocator->SetDataSet(m_polyData);
                cellLocator->BuildLocator();

                //Find the closest points to TestPoint
                auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
                double closestPoint[3];   //the coordinates of the closest point will be returned here
                double closestPointDist2; //the squared distance to the closest point will be returned here
                vtkIdType cellId;         //the cell id of the cell containing the closest point will be returned here
                int subId;

                cellLocator->FindClosestPoint(worldValue, closestPoint, assistCell, cellId, subId, closestPointDist2);
                //std::cout << "closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;

                auto modelMat = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
                auto modelViewMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
                auto viewTransMat = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();
                auto eyeTransMat = this->CurrentRenderer->GetActiveCamera()->GetEyeTransformMatrix();

                auto cameraPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                std::cout << "camera\t" << cameraPos[0] << ',' << cameraPos[1] << ',' << cameraPos[2] << '\n';

                //std::cout << "model mat: ";
                //for (size_t i = 0; i < 16; ++i)
                //{
                //    std::cout << modelMat->GetData()[i] << ',';
                //}
                //std::cout << "\nmodel view mat: ";
                //for (size_t i = 0; i < 16; ++i)
                //{
                //    std::cout << modelViewMat->GetData()[i] << ',';
                //}
                //std::cout << "\nview trans mat: ";
                //for (size_t i = 0; i < 16; ++i)
                //{
                //    std::cout << viewTransMat->GetData()[i] << ',';
                //}
                //std::cout << "\neye trans mat: ";
                //for (size_t i = 0; i < 16; ++i)
                //{
                //    std::cout << eyeTransMat->GetData()[i] << ',';
                //}
                //std::cout << '\n';

                double modelPos[4]{ closestPoint[0],closestPoint[1], closestPoint[2], 0.0 };
                auto worldValue1 = modelMat->MultiplyPoint(modelPos);

                vtkNew<vtkMatrix4x4> matInvert;
                vtkMatrix4x4::Invert(modelMat, matInvert);

                vtkNew<vtkMatrix4x4> matTranspose;
                vtkMatrix4x4::Transpose(modelMat, matTranspose);

                auto modelPos1 = matInvert->MultiplyPoint(worldValue1);
                auto modelPos2 = matTranspose->MultiplyPoint(worldValue1);

                //std::cout << modelPos[0] << ',' << modelPos[1] << ',' << modelPos[2] << '\n';
                //std::cout << modelPos1[0] << ',' << modelPos1[1] << ',' << modelPos1[2] << '\n';
                //std::cout << modelPos2[0] << ',' << modelPos2[1] << ',' << modelPos2[2] << '\n';
            }


            // 在鼠标点击处绘制一个直径始终为10个像素的圆
            if (!this->Interactor)
            {
                auto posX = this->Interactor->GetEventPosition()[0];
                auto posY = this->Interactor->GetEventPosition()[1];

                vtkNew<vtkCoordinate> coord;
                coord->SetCoordinateSystemToDisplay();
                coord->SetValue(posX, posY, 0.);
                auto worldCoord = coord->GetComputedWorldValue(this->CurrentRenderer);

                auto cameraPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
                std::cout << "camera\t" << cameraPos[0] << ',' << cameraPos[1] << ',' << cameraPos[2] << '\n';

                std::cout << "ParallelScale switch\t" << this->CurrentRenderer->GetActiveCamera()->GetParallelProjection() << '\n';
                std::cout << "ParallelScale\t" << this->CurrentRenderer->GetActiveCamera()->GetParallelScale() << '\n';

                vtkNew<vtkCoordinate> coord1;
                coord1->SetCoordinateSystemToDisplay();
                coord1->SetValue(0., 0., 0.);
                auto worldCoord1 = coord1->GetComputedWorldValue(this->CurrentRenderer);

                vtkNew<vtkCoordinate> coord2;
                coord2->SetCoordinateSystemToDisplay();
                coord2->SetValue(10., 0., 0.);
                auto worldCoord2 = coord2->GetComputedWorldValue(this->CurrentRenderer);

                auto distance = std::hypot(worldCoord1[0] - worldCoord2[0], worldCoord1[1] - worldCoord2[1], worldCoord1[2] - worldCoord2[2]);

                std::cout << "world 1: " << worldCoord1[0] << ',' << worldCoord1[1] << ',' << worldCoord1[2] << '\n';
                std::cout << "world 2: " << worldCoord2[0] << ',' << worldCoord2[1] << ',' << worldCoord2[2] << '\n';
                std::cout << "distance: " << distance << '\n';

                vtkNew<vtkRegularPolygonSource> polygonSource;
                polygonSource->GeneratePolygonOff();
                polygonSource->SetNumberOfSides(30);
                polygonSource->SetRadius(distance);
                polygonSource->SetCenter(worldCoord);

                vtkNew<vtkPolyDataMapper> mapper;
                mapper->SetInputConnection(polygonSource->GetOutputPort());

                vtkNew<vtkActor> actor;
                actor->SetMapper(mapper);
                actor->GetProperty()->SetColor(1, 0, 0);

                this->CurrentRenderer->AddActor(actor);
            }

            Superclass::OnLeftButtonUp();
        }
        void SetPolyData(vtkSmartPointer<vtkPolyData> polyData)
        {
            m_polyData = polyData;
        }
    private:
        vtkSmartPointer<vtkPolyData> m_polyData{ nullptr };
    };

    vtkStandardNewMacro(InteractorStyle);
}

int main()
{
    vtkNew<vtkRenderer> renderer;

    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < vertices1.size(); i += 3)
        {
            points->InsertNextPoint(vertices1[i], vertices1[i + 1], vertices1[i + 2]);
        }
        cells->InsertNextCell({ 0,1,2,3 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        //mapper
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputData(polyData);

        //actor
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(0, 1, 0);

        renderer->AddActor(cubeActor);
    }

    {
        vtkNew<vtkPolyData> polyData;
        vtkNew<vtkPoints> points;
        vtkNew<vtkCellArray> cells;

        for (size_t i = 0; i < vertices2.size(); i += 3)
        {
            points->InsertNextPoint(vertices2[i], vertices2[i + 1], vertices2[i + 2]);
        }

        cells->InsertNextCell({ 0,1,2,3 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        //mapper
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputData(polyData);

        //actor
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(1, 1, 0);

        renderer->AddActor(cubeActor);
    }

    // 从actor获取顶点数据
    //auto dataset = cubeActor->GetMapper()->GetInput();
    //vtkNew<vtkIdList> pts;
    //dataset->GetCellPoints(1, pts);
    //auto num = pts->GetNumberOfIds();
    //auto id1 = pts->GetId(0);
    //auto id2 = pts->GetId(1);
    //double pt[3]{ 0. };
    //dataset->GetPoint(id1, pt);
    //dataset->GetPoint(id2, pt);

    //renderer
    renderer->GetActiveCamera()->ParallelProjectionOff();
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<InteractorStyle> style;
    //style->SetPolyData(polyData);
    iren->SetInteractorStyle(style);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST30

#ifdef TEST31

#include <vtkActor.h>
#include <vtkLegendScaleActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricEnneper.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShaderProperty.h>
#include <vtkTextProperty.h>

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // The next source will be a parametric function
    vtkNew<vtkParametricEnneper> src;
    vtkNew<vtkParametricFunctionSource> fnSrc;
    fnSrc->SetParametricFunction(src);

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(fnSrc->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("LegendScaleActor");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkLegendScaleActor> legendScaleActor;
    // 位置可以通过继承vtkLegendScaleActor并重新实现BuildRepresentation方法并修改LabelActors的坐标来修改
    //legendScaleActor->LegendVisibilityOff();  //主比例尺不显示
    //legendScaleActor->VisibilityOff();        // 所有都不显示
    //legendScaleActor->BottomAxisVisibilityOff();//下面的比例尺不显示
    //legendScaleActor->LeftAxisVisibilityOff();  //左边比例尺不显示
    //legendScaleActor->RightAxisVisibilityOff();
    //legendScaleActor->TopAxisVisibilityOff();

    //legendScaleActor->SetLabelMode(); //设置比例尺标签模式
    //legendScaleActor->SetLabelModeToXYCoordinates();
    //legendScaleActor->SetLabelModeToDistance();

    //legendScaleActor->UseBoundsOff();
    //legendScaleActor->SetAllocatedRenderTime();
    //legendScaleActor->SetCornerOffsetFactor(0);//设置四个次比例尺相交距离
    //legendScaleActor->SetTopBorderOffset(0); //设置上比例尺到窗口边缘的距离
    //legendScaleActor->SetDragable(false);  //和拖动有关，默认为true
    //legendScaleActor->SetEstimatedRenderTime();
    //legendScaleActor->SetMemkindDirectory("sss");
    //vtkNew<vtkShaderProperty> shaderProperty;
    //legendScaleActor->SetShaderProperty(shaderProperty); //设置着色器属性
    //legendScaleActor->SetRenderTimeMultiplier();
    //legendScaleActor->SetPropertyKeys();

    //legendScaleActor->GetLegendLabelProperty()->SetColor(1, 0, 0); //标签文本属性
    //legendScaleActor->GetLegendTitleProperty()->SetColor(0, 1, 0); //标题文本属性
    //legendScaleActor->GetLegendTitleProperty()->SetFontSize(legendScaleActor->GetLegendLabelProperty()->GetFontSize() * 10);
    //legendScaleActor->GetLegendLabelProperty()->SetFontSize(legendScaleActor->GetLegendLabelProperty()->GetFontSize() * 2);
    //legendScaleActor->GetLegendTitleProperty()->SetLineOffset(100);
    //legendScaleActor->GetLegendLabelProperty()->SetLineOffset(-250);

    //vtkNew<vtkPropCollection> propCollection;
    //legendScaleActor->GetActors(propCollection);
    //auto bound = legendScaleActor->GetBounds();

    // Add the actor to the scene
    renderer->AddActor(actor);
    renderer->AddActor(legendScaleActor);

    renderer->SetBackground(colors->GetColor3d("RoyalBlue").GetData());

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST31

#ifdef TEST32

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkCamera.h>
#include <vtkCleanPolyData.h>
#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkElevationFilter.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkMaskPoints.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricRandomHills.h>
#include <vtkParametricTorus.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkScalarBarActor.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkSuperquadricSource.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkVariant.h>
#include <vtkVariantArray.h>
#include <vtkVersion.h>

#if VTK_VERSION_NUMBER >= 90020210809ULL
#define HAS_COW
#include <vtkCameraOrientationWidget.h>
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {
    //! Generate elevations over the surface.
    /*!
    @param src - the vtkPolyData source.
    @return elev - the elevations.
    */
    vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src);

    vtkSmartPointer<vtkPolyData> GetHills();
    vtkSmartPointer<vtkPolyData> GetParametricHills();
    vtkSmartPointer<vtkPolyData> GetParametricTorus();
    vtkSmartPointer<vtkPolyData> GetPlane();
    vtkSmartPointer<vtkPolyData> GetSphere();
    vtkSmartPointer<vtkPolyData> GetTorus();
    vtkSmartPointer<vtkPolyData> GetSource(std::string const& source);

    vtkSmartPointer<vtkColorSeries> GetColorSeries();

    vtkSmartPointer<vtkLookupTable> GetCategoricalLUT();

    vtkSmartPointer<vtkLookupTable> GetOrdinaLUT();

    vtkSmartPointer<vtkLookupTable> GetDivergingLUT();

    vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut);

    //!  Glyph the normals on the surface.
    /*!
    @param src - the vtkPolyData source.
    #param scaleFactor - the scale factor for the glyphs.
    @param reverseNormals - if True the normals on the surface are reversed.
    @return The glyphs.
    */
    vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor = 1.0,
        bool const& reverseNormals = false);

    std::map<int, std::vector<double>> GetBands(double const dR[2],
        int const& numberOfBands,
        int const& precision = 2,
        bool const& nearestInteger = false);

    //! Divide a range into custom bands
    /*!
    You need to specify each band as an array [r1, r2] where r1 < r2 and
    append these to a vector.
    The vector should ultimately look
    like this: [[r1, r2], [r2, r3], [r3, r4]...]

    @param dR - [min, max] the range that is to be covered by the bands.
    @param numberOfBands - the number of bands, a positive integer.
    @param myBands - the bands.
    @return  A map consisting of the band inxex and [min, midpoint, max] for each
    band.
    */
    std::map<int, std::vector<double>>
        GetCustomBands(double const dR[2], int const& numberOfBands,
            std::vector<std::array<double, 2>> const& myBands);

    //! Divide a range into integral bands
    /*!
    Divide a range into bands
    @param dR - [min, max] the range that is to be covered by the bands.
    @returnA map consisting of the band inxex and [min, midpoint, max] for each
    band.
    */
    std::map<int, std::vector<double>> GetIntegralBands(double const dR[2]);

    //! Count the number of scalars in each band.
    /*
     * The scalars used are the active scalars in the polydata.
     *
     * @param bands - the bands.
     * @param src - the vtkPolyData source.
     * @return The frequencies of the scalars in each band.
     */
    std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
        vtkPolyData* src);
    //!
    /*
     * The bands and frequencies are adjusted so that the first and last
     *  frequencies in the range are non-zero.
     * @param bands: The bands.
     * @param freq: The frequencies.
     */
    void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
        std::map<int, int>& freq);

    void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
        std::map<int, int>& freq, int const& precision = 2);

} // namespace

int main(int, char* [])
{
    // Get the surface
    std::string desiredSurface{ "RandomHills" };
    // desiredSurface = "Hills";
    // desiredSurface = "ParametricTorus";
    // desiredSurface = "Plane";
    // desiredSurface = "RandomHills";
    // desiredSurface = "Sphere";
    // desiredSurface = "Torus";
    auto source = GetSource(desiredSurface);

    // The length of the normal arrow glyphs.
    auto scaleFactor = 1.0;
    if (desiredSurface == "Hills")
    {
        scaleFactor = 0.5;
    }
    if (desiredSurface == "Sphere")
    {
        scaleFactor = 2.0;
    }
    std::cout << desiredSurface << std::endl;

    source->GetPointData()->SetActiveScalars("Elevation");
    auto scalarRange =
        source->GetPointData()->GetScalars("Elevation")->GetRange();

    auto lut = GetCategoricalLUT();
    auto lut1 = GetOrdinaLUT();
    lut->SetTableRange(scalarRange);
    lut1->SetTableRange(scalarRange);
    vtkIdType numberOfBands = lut->GetNumberOfTableValues();
    auto precision = 10;
    auto bands = GetBands(scalarRange, numberOfBands, precision, false);

    if (desiredSurface == "RandomHills")
    {
        // These are my custom bands.
        // Generated by first running:
        // bands = GetBands(scalarRange, numberOfBands, precision, false);
        // then:
        //  std::vector<int> freq = Frequencies(bands, source);
        //  PrintBandsFrequencies(bands, freq);
        // Finally using the output to create this table:
        std::vector<std::array<double, 2>> myBands = {
            {0, 1.0},   {1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0},
            {4.0, 5.0}, {5.0, 6.0}, {6.0, 7.0}, {7.0, 8.0} };
        // Comment this out if you want to see how allocating
        // equally spaced bands works.
        bands = GetCustomBands(scalarRange, numberOfBands, myBands);
        // bands = GetBands(scalarRange, numberOfBands, precision, false);
        // Adjust the number of table values
    }
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));
    lut1->SetNumberOfTableValues(static_cast<vtkIdType>(bands.size()));

    // Let's do a frequency table.
    auto freq = GetFrequencies(bands, source);
    AdjustFrequencyRanges(bands, freq);
    PrintBandsFrequencies(bands, freq);

    scalarRange[0] = bands.begin()->second[0];
    scalarRange[1] = std::prev(bands.end())->second[2];
    lut->SetTableRange(scalarRange);
    lut->SetNumberOfTableValues(bands.size());

    // We will use the midpoint of the band as the label.
    std::vector<std::string> labels;
    for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
        p != bands.end(); ++p)
    {
        std::ostringstream os;
        os << std::fixed << std::setw(6) << std::setprecision(2) << p->second[1];
        labels.push_back(os.str());
    }

    // Annotate
    vtkNew<vtkVariantArray> values;
    for (size_t i = 0; i < labels.size(); ++i)
    {
        values->InsertNextValue(vtkVariant(labels[i]));
    }
    for (vtkIdType i = 0; i < values->GetNumberOfTuples(); ++i)
    {
        lut->SetAnnotation(i, values->GetValue(i).ToString());
    }

    // Create a lookup table with the colors reversed.
    auto lutr = ReverseLUT(lut);

    // Create the contour bands.
    vtkNew<vtkBandedPolyDataContourFilter> bcf;
    bcf->SetInputData(source);
    // Use either the minimum or maximum value for each band.
    int i = 0;
    for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
        p != bands.end(); ++p)
    {
        bcf->SetValue(i, p->second[2]);
        ++i;
    }
    // We will use an indexed lookup table.
    bcf->SetScalarModeToIndex();
    bcf->GenerateContourEdgesOn();

    // Generate the glyphs on the original surface.
    auto glyph = GetGlyphs(source, scaleFactor, false);

    // ------------------------------------------------------------
    // Create the mappers and actors
    // ------------------------------------------------------------
    vtkNew<vtkNamedColors> colors;

    // Set the background color.
    colors->SetColor("BkgColor",
        std::array<unsigned char, 4>{179, 204, 255, 255}.data());
    colors->SetColor("ParaViewBkg",
        std::array<unsigned char, 4>{82, 87, 110, 255}.data());

    vtkNew<vtkPolyDataMapper> srcMapper;
    srcMapper->SetInputConnection(bcf->GetOutputPort());
    srcMapper->SetScalarRange(scalarRange);
    srcMapper->SetLookupTable(lut);
    srcMapper->SetScalarModeToUseCellData();

    vtkNew<vtkActor> srcActor;
    srcActor->SetMapper(srcMapper);

    // Create contour edges
    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputData(bcf->GetContourEdgesOutput());
    edgeMapper->SetResolveCoincidentTopologyToPolygonOffset();

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());

    vtkNew<vtkPolyDataMapper> glyphMapper;
    glyphMapper->SetInputConnection(glyph->GetOutputPort());
    glyphMapper->SetScalarModeToUsePointFieldData();
    glyphMapper->SetColorModeToMapScalars();
    glyphMapper->ScalarVisibilityOn();
    glyphMapper->SelectColorArray("Elevation");
    // Colour by scalars.
    glyphMapper->SetLookupTable(lut1);
    glyphMapper->SetScalarRange(scalarRange);

    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);

    auto windowWidth = 800;
    auto windowHeight = 800;

    // Add scalar bars.
    vtkNew<vtkScalarBarActor> scalarBar;
    // This LUT puts the lowest value at the top of the scalar bar.
    // scalarBar->SetLookupTable(lut);
    // Use this LUT if you want the highest value at the top.
    scalarBar->SetLookupTable(lutr);
    scalarBar->SetTitle("Elevation");
    scalarBar->GetTitleTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->GetLabelTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->GetAnnotationTextProperty()->SetColor(
        colors->GetColor3d("AliceBlue").GetData());
    scalarBar->UnconstrainedFontSizeOn();
    scalarBar->SetMaximumWidthInPixels(windowWidth / 8);
    scalarBar->SetMaximumHeightInPixels(windowHeight / 3);
    scalarBar->SetPosition(0.85, 0.05);

    // ------------------------------------------------------------
    // Create the RenderWindow, Renderer and Interactor
    // ------------------------------------------------------------
    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    renWin->AddRenderer(ren);
    // Important: The interactor must be set prior to enabling the widget.
    iren->SetRenderWindow(renWin);
#ifdef HAS_COW
    vtkNew<vtkCameraOrientationWidget> camOrientManipulator;
    camOrientManipulator->SetParentRenderer(ren);
    // Enable the widget.
    camOrientManipulator->On();
#endif

    // add actors
    ren->AddViewProp(srcActor);
    ren->AddViewProp(edgeActor);
    ren->AddViewProp(glyphActor);
    ren->AddActor2D(scalarBar);

    ren->SetBackground(colors->GetColor3d("ParaViewBkg").GetData());
    renWin->SetSize(windowWidth, windowHeight);
    renWin->SetWindowName("ElevationBandsWithGlyphs");

    if (desiredSurface == "RandomHills")
    {
        auto camera = ren->GetActiveCamera();
        camera->SetPosition(10.9299, 59.1505, 24.9823);
        camera->SetFocalPoint(2.21692, 7.97545, 7.75135);
        camera->SetViewUp(-0.230136, 0.345504, -0.909761);
        camera->SetDistance(54.6966);
        camera->SetClippingRange(36.3006, 77.9852);
        renWin->Render();
    }

    iren->Start();

    return EXIT_SUCCESS;
}

namespace {
    vtkSmartPointer<vtkPolyData> GetElevations(vtkPolyData* src)
    {
        double bounds[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        src->GetBounds(bounds);
        if (std::abs(bounds[2]) < 1.0e-8 && std::abs(bounds[3]) < 1.0e-8)
        {
            bounds[3] = bounds[2] + 1;
        }
        vtkNew<vtkElevationFilter> elevFilter;
        elevFilter->SetInputData(src);
        elevFilter->SetLowPoint(0, bounds[2], 0);
        elevFilter->SetHighPoint(0, bounds[3], 0);
        elevFilter->SetScalarRange(bounds[2], bounds[3]);
        elevFilter->Update();

        return elevFilter->GetPolyDataOutput();
    }

    vtkSmartPointer<vtkPolyData> GetHills()
    {
        // Create four hills on a plane.
        // This will have regions of negative, zero and positive Gsaussian curvatures.

        auto xRes = 50;
        auto yRes = 50;
        auto xMin = -5.0;
        auto xMax = 5.0;
        auto dx = (xMax - xMin) / (xRes - 1.0);
        auto yMin = -5.0;
        auto yMax = 5.0;
        auto dy = (yMax - yMin) / (xRes - 1.0);

        // Make a grid.
        vtkNew<vtkPoints> points;
        for (auto i = 0; i < xRes; ++i)
        {
            auto x = xMin + i * dx;
            for (auto j = 0; j < yRes; ++j)
            {
                auto y = yMin + j * dy;
                points->InsertNextPoint(x, y, 0);
            }
        }

        // Add the grid points to a polydata object.
        vtkNew<vtkPolyData> plane;
        plane->SetPoints(points);

        // Triangulate the grid.
        vtkNew<vtkDelaunay2D> delaunay;
        delaunay->SetInputData(plane);
        delaunay->Update();

        auto polydata = delaunay->GetOutput();

        vtkNew<vtkDoubleArray> elevation;
        elevation->SetNumberOfTuples(points->GetNumberOfPoints());

        //  We define the parameters for the hills here.
        // [[0: x0, 1: y0, 2: x variance, 3: y variance, 4: amplitude]...]
        std::vector<std::array<double, 5>> hd{ {-2.5, -2.5, 2.5, 6.5, 3.5},
                                              {2.5, 2.5, 2.5, 2.5, 2},
                                              {5.0, -2.5, 1.5, 1.5, 2.5},
                                              {-5.0, 5, 2.5, 3.0, 3} };
        std::array<double, 2> xx{ 0.0, 0.0 };
        for (auto i = 0; i < points->GetNumberOfPoints(); ++i)
        {
            auto x = polydata->GetPoint(i);
            for (size_t j = 0; j < hd.size(); ++j)
            {
                xx[0] = std::pow(x[0] - hd[j][0] / hd[j][2], 2.0);
                xx[1] = std::pow(x[1] - hd[j][1] / hd[j][3], 2.0);
                x[2] += hd[j][4] * std::exp(-(xx[0] + xx[1]) / 2.0);
            }
            polydata->GetPoints()->SetPoint(i, x);
            elevation->SetValue(i, x[2]);
        }

        vtkNew<vtkFloatArray> textures;
        textures->SetNumberOfComponents(2);
        textures->SetNumberOfTuples(2 * polydata->GetNumberOfPoints());
        textures->SetName("Textures");

        for (auto i = 0; i < xRes; ++i)
        {
            float tc[2];
            tc[0] = i / (xRes - 1.0);
            for (auto j = 0; j < yRes; ++j)
            {
                // tc[1] = 1.0 - j / (yRes - 1.0);
                tc[1] = j / (yRes - 1.0);
                textures->SetTuple(static_cast<vtkIdType>(i) * yRes + j, tc);
            }
        }

        polydata->GetPointData()->SetScalars(elevation);
        polydata->GetPointData()->GetScalars()->SetName("Elevation");
        polydata->GetPointData()->SetTCoords(textures);

        vtkNew<vtkPolyDataNormals> normals;
        normals->SetInputData(polydata);
        normals->SetInputData(polydata);
        normals->SetFeatureAngle(30);
        normals->SplittingOff();

        vtkNew<vtkTransform> tr1;
        tr1->RotateX(-90);

        vtkNew<vtkTransformPolyDataFilter> tf1;
        tf1->SetInputConnection(normals->GetOutputPort());
        tf1->SetTransform(tr1);
        tf1->Update();

        return tf1->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetParametricHills()
    {
        vtkNew<vtkParametricRandomHills> fn;
        fn->AllowRandomGenerationOn();
        fn->SetRandomSeed(1);
        fn->SetNumberOfHills(30);

        vtkNew<vtkParametricFunctionSource> source;
        source->SetParametricFunction(fn);
        source->SetUResolution(50);
        source->SetVResolution(50);
        source->SetScalarModeToZ();
        source->Update();

        // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
        // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
        // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
        // Rename the scalars to "Elevation" since we are using the Z-scalars as
        // elevations.
        source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

        vtkNew<vtkTransform> transform;
        transform->Translate(0.0, 5.0, 15.0);
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetParametricTorus()
    {
        vtkNew<vtkParametricTorus> fn;
        fn->SetRingRadius(5);
        fn->SetCrossSectionRadius(2);

        vtkNew<vtkParametricFunctionSource> source;
        source->SetParametricFunction(fn);
        source->SetUResolution(50);
        source->SetVResolution(50);
        source->SetScalarModeToZ();
        source->Update();

        // Name the arrays (not needed in VTK 6.2+ for vtkParametricFunctionSource).
        // source->GetOutput()->GetPointData()->GetNormals()->SetName("Normals");
        // source->GetOutput()->GetPointData()->GetScalars()->SetName("Scalars");
        // Rename the scalars to "Elevation" since we are using the Z-scalars as
        // elevations.
        source->GetOutput()->GetPointData()->GetScalars()->SetName("Elevation");

        vtkNew<vtkTransform> transform;
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        return transformFilter->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetPlane()
    {
        vtkNew<vtkPlaneSource> source;
        source->SetOrigin(-10.0, -10.0, 0.0);
        source->SetPoint2(-10.0, 10.0, 0.0);
        source->SetPoint1(10.0, -10.0, 0.0);
        source->SetXResolution(20);
        source->SetYResolution(20);
        source->Update();

        vtkNew<vtkTransform> transform;
        transform->Translate(0.0, 0.0, 0.0);
        transform->RotateX(-90.0);
        vtkNew<vtkTransformPolyDataFilter> transformFilter;
        transformFilter->SetInputConnection(source->GetOutputPort());
        transformFilter->SetTransform(transform);
        transformFilter->Update();

        // We have a m x n array of quadrilaterals arranged as a regular tiling in a
        // plane. So pass it through a triangle filter since the curvature filter only
        // operates on polys.
        vtkNew<vtkTriangleFilter> tri;
        tri->SetInputConnection(transformFilter->GetOutputPort());

        // Pass it though a CleanPolyDataFilter and merge any points which
        // are coincident, or very close
        vtkNew<vtkCleanPolyData> cleaner;
        cleaner->SetInputConnection(tri->GetOutputPort());
        cleaner->SetTolerance(0.005);
        cleaner->Update();

        return cleaner->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetSphere()
    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(0.0, 0.0, 0.0);
        source->SetRadius(10.0);
        source->SetThetaResolution(32);
        source->SetPhiResolution(32);
        source->Update();

        return source->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetTorus()
    {
        vtkNew<vtkSuperquadricSource> source;
        source->SetCenter(0.0, 0.0, 0.0);
        source->SetCenter(1.0, 1.0, 1.0);
        source->SetPhiResolution(64);
        source->SetThetaResolution(64);
        source->SetThetaRoundness(1);
        source->SetThickness(0.5);
        source->SetSize(10);
        source->SetToroidal(1);
        source->Update();

        // The quadric is made of strips, so pass it through a triangle filter as
        // the curvature filter only operates on polys
        vtkNew<vtkTriangleFilter> tri;
        tri->SetInputConnection(source->GetOutputPort());

        // The quadric has nasty discontinuities from the way the edges are generated
        // so let's pass it though a CleanPolyDataFilter and merge any points which
        // are coincident, or very close
        vtkNew<vtkCleanPolyData> cleaner;
        cleaner->SetInputConnection(tri->GetOutputPort());
        cleaner->SetTolerance(0.005);
        cleaner->Update();

        return cleaner->GetOutput();
    }

    vtkSmartPointer<vtkPolyData> GetSource(std::string const& source)
    {
        std::string surface = source;
        std::transform(surface.begin(), surface.end(), surface.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::map<std::string, int> available_surfaces = {
            {"hills", 0},       {"parametrictorus", 1}, {"plane", 2},
            {"randomhills", 3}, {"sphere", 4},          {"torus", 5} };
        if (available_surfaces.find(surface) == available_surfaces.end())
        {
            std::cout << "The surface is not available." << std::endl;
            std::cout << "Using RandomHills instead." << std::endl;
            surface = "randomhills";
        }
        switch (available_surfaces[surface])
        {
        case 0:
            return GetHills();
            break;
        case 1:
            return GetParametricTorus();
            break;
        case 2:
            return GetElevations(GetPlane());
            break;
        case 3:
            return GetParametricHills();
            break;
        case 4:
            return GetElevations(GetSphere());
            break;
        case 5:
            return GetElevations(GetTorus());
            break;
        }
        return GetParametricHills();
    }

    vtkSmartPointer<vtkColorSeries> GetColorSeries()
    {

        vtkNew<vtkColorSeries> colorSeries;
        // Select a color scheme.
        int colorSeriesEnum;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_BROWN_BLUE_GREEN_9;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_10;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_SPECTRAL_3;
        // colorSeriesEnum = colorSeries->BREWER_DIVERGING_PURPLE_ORANGE_9;
        // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_PURPLE_9;
        // colorSeriesEnum = colorSeries->BREWER_SEQUENTIAL_BLUE_GREEN_9;
        colorSeriesEnum = colorSeries->BREWER_QUALITATIVE_SET3;
        // colorSeriesEnum = colorSeries->CITRUS;
        colorSeries->SetColorScheme(colorSeriesEnum);
        return colorSeries;
    }

    vtkSmartPointer<vtkLookupTable> GetCategoricalLUT()
    {
        vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
        // Make the lookup table.
        vtkNew<vtkLookupTable> lut;
        colorSeries->BuildLookupTable(lut, vtkColorSeries::CATEGORICAL);
        lut->SetNanColor(0, 0, 0, 1);

        return lut;
    }

    vtkSmartPointer<vtkLookupTable> GetOrdinaLUT()
    {
        vtkSmartPointer<vtkColorSeries> colorSeries = GetColorSeries();
        // Make the lookup table.
        vtkNew<vtkLookupTable> lut;
        colorSeries->BuildLookupTable(lut, vtkColorSeries::ORDINAL);
        lut->SetNanColor(0, 0, 0, 1);

        return lut;
    }

    // clang-format off
    /**
     * See: [Diverging Color Maps for Scientific Visualization](https://www.kennethmoreland.com/color-maps/)
     *
     *                   start point         midPoint            end point
     * cool to warm:     0.230, 0.299, 0.754 0.865, 0.865, 0.865 0.706, 0.016, 0.150
     * purple to orange: 0.436, 0.308, 0.631 0.865, 0.865, 0.865 0.759, 0.334, 0.046
     * green to purple:  0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.436, 0.308, 0.631
     * blue to brown:    0.217, 0.525, 0.910 0.865, 0.865, 0.865 0.677, 0.492, 0.093
     * green to red:     0.085, 0.532, 0.201 0.865, 0.865, 0.865 0.758, 0.214, 0.233
     *
     */
     // clang-format on
    vtkSmartPointer<vtkLookupTable> GetDivergingLUT()
    {

        vtkNew<vtkColorTransferFunction> ctf;
        ctf->SetColorSpaceToDiverging();
        ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
        ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
        ctf->AddRGBPoint(1.0, 0.758, 0.214, 0.233);

        auto tableSize = 256;
        vtkNew<vtkLookupTable> lut;
        lut->SetNumberOfTableValues(tableSize);
        lut->Build();

        for (auto i = 0; i < lut->GetNumberOfColors(); ++i)
        {
            std::array<double, 3> rgb;
            ctf->GetColor(static_cast<double>(i) / lut->GetNumberOfColors(),
                rgb.data());
            std::array<double, 4> rgba{ 0.0, 0.0, 0.0, 1.0 };
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
            lut->SetTableValue(static_cast<vtkIdType>(i), rgba.data());
        }

        return lut;
    }

    vtkSmartPointer<vtkLookupTable> ReverseLUT(vtkLookupTable* lut)
    {
        // First do a deep copy just to get the whole structure
        // and then reverse the colors and annotations.
        vtkNew<vtkLookupTable> lutr;
        lutr->DeepCopy(lut);
        vtkIdType t = lut->GetNumberOfTableValues() - 1;
        for (vtkIdType i = t; i >= 0; --i)
        {
            std::array<double, 3> rgb{ 0.0, 0.0, 0.0 };
            std::array<double, 4> rgba{ 0.0, 0.0, 0.0, 1.0 };
            lut->GetColor(i, rgb.data());
            std::copy(std::begin(rgb), std::end(rgb), std::begin(rgba));
            rgba[3] = lut->GetOpacity(i);
            lutr->SetTableValue(t - i, rgba.data());
        }
        t = lut->GetNumberOfAnnotatedValues() - 1;
        for (vtkIdType i = t; i >= 0; --i)
        {
            lutr->SetAnnotation(t - i, lut->GetAnnotation(i));
        }

        return lutr;
    }

    vtkNew<vtkGlyph3D> GetGlyphs(vtkPolyData* src, double const& scaleFactor,
        bool const& reverseNormals)
    {
        // Sometimes the contouring algorithm can create a volume whose gradient
        // vector and ordering of polygon(using the right hand rule) are
        // inconsistent. vtkReverseSense cures this problem.
        vtkNew<vtkReverseSense> reverse;
        vtkNew<vtkMaskPoints> maskPts;
        maskPts->SetOnRatio(5);
        maskPts->RandomModeOn();
        if (reverseNormals)
        {
            reverse->SetInputData(src);
            reverse->ReverseCellsOn();
            reverse->ReverseNormalsOn();
            maskPts->SetInputConnection(reverse->GetOutputPort());
        }
        else
        {
            maskPts->SetInputData(src);
        }

        // Source for the glyph filter
        vtkNew<vtkArrowSource> arrow;
        arrow->SetTipResolution(16);
        arrow->SetTipLength(0.3);
        arrow->SetTipRadius(0.1);

        vtkNew<vtkGlyph3D> glyph;
        glyph->SetSourceConnection(arrow->GetOutputPort());
        glyph->SetInputConnection(maskPts->GetOutputPort());
        glyph->SetVectorModeToUseNormal();
        glyph->SetScaleFactor(scaleFactor);
        glyph->SetColorModeToColorByVector();
        glyph->SetScaleModeToScaleByVector();
        glyph->OrientOn();
        glyph->Update();

        return glyph;
    }

    std::map<int, std::vector<double>> GetBands(double const dR[2],
        int const& numberOfBands,
        int const& precision,
        bool const& nearestInteger)
    {
        auto prec = abs(precision);
        prec = (prec > 14) ? 14 : prec;

        auto RoundOff = [&prec](const double& x) {
            auto pow_10 = std::pow(10.0, prec);
            return std::round(x * pow_10) / pow_10;
        };

        std::map<int, std::vector<double>> bands;
        if ((dR[1] < dR[0]) || (numberOfBands <= 0))
        {
            return bands;
        }
        double x[2];
        for (int i = 0; i < 2; ++i)
        {
            x[i] = dR[i];
        }
        if (nearestInteger)
        {
            x[0] = std::floor(x[0]);
            x[1] = std::ceil(x[1]);
        }
        double dx = (x[1] - x[0]) / static_cast<double>(numberOfBands);
        std::vector<double> b;
        b.push_back(x[0]);
        b.push_back(x[0] + dx / 2.0);
        b.push_back(x[0] + dx);
        for (int i = 0; i < numberOfBands; ++i)
        {
            if (i == 0)
            {
                for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
                {
                    *p = RoundOff(*p);
                }
                b[0] = x[0];
            }
            bands[i] = b;
            for (std::vector<double>::iterator p = b.begin(); p != b.end(); ++p)
            {
                *p = RoundOff(*p + dx);
            }
        }
        return bands;
    }

    std::map<int, std::vector<double>>
        GetCustomBands(double const dR[2], int const& numberOfBands,
            std::vector<std::array<double, 2>> const& myBands)
    {
        std::map<int, std::vector<double>> bands;
        if ((dR[1] < dR[0]) || (numberOfBands <= 0))
        {
            return bands;
        }

        std::vector<std::array<double, 2>> x;
        std::copy(myBands.begin(), myBands.end(), std::back_inserter(x));

        // Determine the index of the range minimum and range maximum.
        int idxMin = 0;
        for (auto idx = 0; idx < static_cast<int>(myBands.size()); ++idx)
        {
            if (dR[0] < myBands[idx][1] && dR[0] >= myBands[idx][0])
            {
                idxMin = idx;
                break;
            }
        }
        int idxMax = static_cast<int>(myBands.size()) - 1;
        for (int idx = static_cast<int>(myBands.size()) - 1; idx >= 0; --idx)
        {
            if (dR[1] < myBands[idx][1] && dR[1] >= myBands[idx][0])
            {
                idxMax = static_cast<int>(idx);
                break;
            }
        }

        // Set the minimum to match the range minimum.
        x[idxMin][0] = dR[0];
        x[idxMax][1] = dR[1];
        for (int i = idxMin; i < idxMax + 1; ++i)
        {
            std::vector<double> b(3);
            b[0] = x[i][0];
            b[1] = x[i][0] + (x[i][1] - x[i][0]) / 2.0;
            b[2] = x[i][1];
            bands[i] = b;
        }
        return bands;
    }

    std::map<int, std::vector<double>> GetIntegralBands(double const dR[2])
    {
        std::map<int, std::vector<double>> bands;
        if (dR[1] < dR[0])
        {
            return bands;
        }
        double x[2];
        for (int i = 0; i < 2; ++i)
        {
            x[i] = dR[i];
        }
        x[0] = std::floor(x[0]);
        x[1] = std::ceil(x[1]);
        int numberOfBands = static_cast<int>(std::abs(x[1]) + std::abs(x[0]));
        return GetBands(x, numberOfBands, false);
    }

    std::map<int, int> GetFrequencies(std::map<int, std::vector<double>>& bands,
        vtkPolyData* src)
    {
        std::map<int, int> freq;
        for (auto i = 0; i < static_cast<int>(bands.size()); ++i)
        {
            freq[i] = 0;
        }
        vtkIdType tuples = src->GetPointData()->GetScalars()->GetNumberOfTuples();
        for (int i = 0; i < tuples; ++i)
        {
            double* x = src->GetPointData()->GetScalars()->GetTuple(i);
            for (auto j = 0; j < static_cast<int>(bands.size()); ++j)
            {
                if (*x <= bands[j][2])
                {
                    freq[j] = freq[j] + 1;
                    break;
                }
            }
        }
        return freq;
    }

    void AdjustFrequencyRanges(std::map<int, std::vector<double>>& bands,
        std::map<int, int>& freq)
    {
        // Get the indices of the first and last non-zero elements.
        auto first = 0;
        for (auto i = 0; i < static_cast<int>(freq.size()); ++i)
        {
            if (freq[i] != 0)
            {
                first = i;
                break;
            }
        }
        std::vector<int> keys;
        for (std::map<int, int>::iterator it = freq.begin(); it != freq.end(); ++it)
        {
            keys.push_back(it->first);
        }
        std::reverse(keys.begin(), keys.end());
        auto last = keys[0];
        for (size_t i = 0; i < keys.size(); ++i)
        {
            if (freq[keys[i]] != 0)
            {
                last = keys[i];
                break;
            }
        }
        // Now adjust the ranges.
        std::map<int, int>::iterator freqItr;
        freqItr = freq.find(first);
        freq.erase(freq.begin(), freqItr);
        freqItr = ++freq.find(last);
        freq.erase(freqItr, freq.end());
        std::map<int, std::vector<double>>::iterator bandItr;
        bandItr = bands.find(first);
        bands.erase(bands.begin(), bandItr);
        bandItr = ++bands.find(last);
        bands.erase(bandItr, bands.end());
        // Reindex freq and bands.
        std::map<int, int> adjFreq;
        int idx = 0;
        for (auto p : freq)
        {
            adjFreq[idx] = p.second;
            ++idx;
        }
        std::map<int, std::vector<double>> adjBands;
        idx = 0;
        for (auto p : bands)
        {
            adjBands[idx] = p.second;
            ++idx;
        }
        bands = adjBands;
        freq = adjFreq;
    }

    void PrintBandsFrequencies(std::map<int, std::vector<double>> const& bands,
        std::map<int, int>& freq, int const& precision)
    {
        auto prec = abs(precision);
        prec = (prec > 14) ? 14 : prec;

        if (bands.size() != freq.size())
        {
            std::cout << "Bands and frequencies must be the same size." << std::endl;
            return;
        }
        std::ostringstream os;
        os << "Bands & Frequencies:\n";
        size_t idx = 0;
        auto total = 0;
        auto width = prec + 6;
        for (std::map<int, std::vector<double>>::const_iterator p = bands.begin();
            p != bands.end(); ++p)
        {
            total += freq[p->first];
            for (std::vector<double>::const_iterator q = p->second.begin();
                q != p->second.end(); ++q)
            {
                if (q == p->second.begin())
                {
                    os << std::setw(4) << idx << " [";
                }
                if (q == std::prev(p->second.end()))
                {
                    os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
                        << "]: " << std::setw(8) << freq[p->first] << "\n";
                }
                else
                {
                    os << std::fixed << std::setw(width) << std::setprecision(prec) << *q
                        << ", ";
                }
            }
            ++idx;
        }
        width = 3 * width + 13;
        os << std::left << std::setw(width) << "Total" << std::right << std::setw(8)
            << total << std::endl;
        std::cout << os.str() << endl;
    }

} // namespace

#endif // TEST32

#ifdef TEST33
//https://kitware.github.io/vtk-examples/site/Cxx/VisualizationAlgorithms/FilledContours/
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkCubeSource.h>

#include <vector>

int main(int argc, char* argv[])
{
    //if (argc < 3)
    //{
    //    std::cerr
    //        << "Usage: " << argv[0]
    //        << " InputPolyDataFile(.vtp) NumberOfContours e.g filledContours.vtp 10"
    //        << std::endl;
    //    return EXIT_FAILURE;
    //}

    vtkNew<vtkCubeSource> cube;


    vtkNew<vtkNamedColors> colors;

    // Read the file
    //vtkNew<vtkXMLPolyDataReader> reader;

    //reader->SetFileName(argv[1]);
    //reader->Update(); // Update so that we can get the scalar range

    vtkNew<vtkFloatArray> scalars;

    // Load the point, cell, and data attributes.
    for (auto i = 0ul; i < cube->GetOutput()->GetPointData()->GetNumberOfTuples(); ++i)
    {
        scalars->InsertTuple1(i, i);
    }

    cube->GetOutput()->GetPointData()->SetScalars(scalars);

    double scalarRange[2];
    //reader->GetOutput()->GetPointData()->GetScalars()->GetRange(scalarRange);
    cube->GetOutput()->GetPointData()->GetScalars()->GetRange(scalarRange);

    vtkNew<vtkAppendPolyData> appendFilledContours;

    //int numberOfContours = atoi(argv[2]);
    int numberOfContours = 5;

    double delta = (scalarRange[1] - scalarRange[0]) /
        static_cast<double>(numberOfContours - 1);

    // Keep the clippers alive
    std::vector<vtkSmartPointer<vtkClipPolyData>> clippersLo;
    std::vector<vtkSmartPointer<vtkClipPolyData>> clippersHi;

    for (int i = 0; i < numberOfContours; i++)
    {
        double valueLo = scalarRange[0] + static_cast<double>(i) * delta;
        double valueHi = scalarRange[0] + static_cast<double>(i + 1) * delta;
        clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersLo[i]->SetValue(valueLo);
        if (i == 0)
        {
            //clippersLo[i]->SetInputConnection(reader->GetOutputPort());
            clippersLo[i]->SetInputConnection(cube->GetOutputPort());
        }
        else
        {
            clippersLo[i]->SetInputConnection(clippersHi[i - 1]->GetOutputPort(1));
        }
        clippersLo[i]->InsideOutOff();
        clippersLo[i]->Update();

        clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
        clippersHi[i]->SetValue(valueHi);
        clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
        clippersHi[i]->GenerateClippedOutputOn();
        clippersHi[i]->InsideOutOn();
        clippersHi[i]->Update();
        if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0)
        {
            continue;
        }

        vtkNew<vtkFloatArray> cd;
        cd->SetNumberOfComponents(1);
        cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
        cd->FillComponent(0, valueLo);

        clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
        appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
    }

    vtkNew<vtkCleanPolyData> filledContours;
    filledContours->SetInputConnection(appendFilledContours->GetOutputPort());

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(numberOfContours + 1);
    lut->Build();
    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputConnection(filledContours->GetOutputPort());
    contourMapper->SetScalarRange(scalarRange[0], scalarRange[1]);
    contourMapper->SetScalarModeToUseCellData();
    contourMapper->SetLookupTable(lut);

    vtkNew<vtkActor> contourActor;
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkContourFilter> contours;
    contours->SetInputConnection(filledContours->GetOutputPort());
    contours->GenerateValues(numberOfContours, scalarRange[0], scalarRange[1]);

    vtkNew<vtkPolyDataMapper> contourLineMapperer;
    contourLineMapperer->SetInputConnection(contours->GetOutputPort());
    contourLineMapperer->SetScalarRange(scalarRange[0], scalarRange[1]);
    contourLineMapperer->ScalarVisibilityOff();

    vtkNew<vtkActor> contourLineActor;
    contourLineActor->SetMapper(contourLineMapperer);
    contourLineActor->GetProperty()->SetLineWidth(2);

    // The usual renderer, render window and interactor
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;

    renWin->AddRenderer(ren1);
    renWin->SetWindowName("FilledContours");

    iren->SetRenderWindow(renWin);

    // Add the actors
    ren1->AddActor(contourActor);
    ren1->AddActor(contourLineActor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    // Begin interaction
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST33

#ifdef TEST34
#include <vtkActor.h>
#include <vtkCylinder.h>
#include <vtkImageData.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVersion.h>

// vtkFlyingEdges3D was introduced in VTK >= 8.2
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 2)
#define USE_FLYING_EDGES
#else
#undef USE_FLYING_EDGES
#endif

#ifdef USE_FLYING_EDGES
#include <vtkFlyingEdges3D.h>
#else
#include <vtkMarchingCubes.h>
#endif

int main(int argc, char* argv[])
{
    int resolution = 50;
    if (argc > 1)
    {
        resolution = atoi(argv[1]);
    }

    vtkNew<vtkNamedColors> colors;

    // Create a sampled sphere
    vtkNew<vtkSphere> implicitSphere;
    double radius = 1.0;
    implicitSphere->SetRadius(radius);

    vtkNew<vtkSampleFunction> sampledSphere;
    sampledSphere->SetSampleDimensions(resolution, resolution, resolution);
    double xMin = -radius * 2.0;
    double xMax = radius * 2.0;
    sampledSphere->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledSphere->SetImplicitFunction(implicitSphere);

#ifdef USE_FLYING_EDGES
    vtkNew<vtkFlyingEdges3D> isoSphere;
#else
    vtkNew<vtkMarchingCubes> isoSphere;
#endif
    isoSphere->SetValue(0, 2.0);
    isoSphere->SetInputConnection(sampledSphere->GetOutputPort());

    // Create a sampled cylinder
    vtkNew<vtkCylinder> implicitCylinder;
    implicitCylinder->SetRadius(radius / 2.0);
    vtkNew<vtkSampleFunction> sampledCylinder;
    sampledCylinder->SetSampleDimensions(resolution, resolution, resolution);
    sampledCylinder->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledCylinder->SetImplicitFunction(implicitCylinder);

    // Probe cylinder with the sphere isosurface
    vtkNew<vtkProbeFilter> probeCylinder;
    probeCylinder->SetInputConnection(0, isoSphere->GetOutputPort());
    probeCylinder->SetInputConnection(1, sampledCylinder->GetOutputPort());
    probeCylinder->Update();

    // Restore the original normals
    probeCylinder->GetOutput()->GetPointData()->SetNormals(
        isoSphere->GetOutput()->GetPointData()->GetNormals());

    std::cout << "Scalar range: "
        << probeCylinder->GetOutput()->GetScalarRange()[0] << ", "
        << probeCylinder->GetOutput()->GetScalarRange()[1] << std::endl;

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapSphere;
    mapSphere->SetInputConnection(probeCylinder->GetOutputPort());
    mapSphere->SetScalarRange(probeCylinder->GetOutput()->GetScalarRange());

    vtkNew<vtkActor> sphere;
    sphere->SetMapper(mapSphere);

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("IsosurfaceSampling");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(sphere);
    renderer->SetBackground(colors->GetColor3d("AliceBlue").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST34

#ifdef TEST35

//https://blog.csdn.net/muye2356/article/details/115144581


#include <vtkOpenFOAMReader.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkTextProperty.h>
#include <vtkLineSource.h>
#include <vtkStreamTracer.h>
#include <vtkPolyData.h>
#include <vtkArrayCalculator.h>

int main()
{
    vtkSmartPointer<vtkOpenFOAMReader> openFOAMReader = vtkSmartPointer<vtkOpenFOAMReader>::New();
    openFOAMReader->SetFileName("04vtk\\resource\\test.foam");//设置读取文件路径
    openFOAMReader->SetCreateCellToPoint(1);
    openFOAMReader->SetSkipZeroTime(1);//开启跳过0时刻
    openFOAMReader->SetTimeValue(298.0);//设置需要读取的时刻
    openFOAMReader->Update();

    vtkUnstructuredGrid* block0 = vtkUnstructuredGrid::SafeDownCast(openFOAMReader->GetOutput()->GetBlock(0));

    //设置生成流线的位置
    vtkSmartPointer<vtkLineSource> line1 = vtkSmartPointer<vtkLineSource>::New();
    line1->SetPoint1(-0.019, 0.0254, 0.0005);
    line1->SetPoint2(-0.0206, 0, 0.0005);
    line1->SetResolution(10);
    line1->Update();

    vtkSmartPointer<vtkStreamTracer> streamline = vtkSmartPointer<vtkStreamTracer>::New();
    streamline->SetSourceConnection(line1->GetOutputPort());
    streamline->SetInputData(block0);
    streamline->SetIntegratorTypeToRungeKutta45();//设置流线的积分类型
    streamline->SetMaximumPropagation(5000);//设置流线长度
    streamline->SetIntegrationStepUnit(2);//设置流线积分步长单位
    streamline->SetInitialIntegrationStep(0.2);//设置流线积分初始步长
    streamline->SetMinimumIntegrationStep(0.01);//设置流线积分最小步长
    streamline->SetMaximumIntegrationStep(0.5);//设置流线积分最大步长
    streamline->SetMaximumNumberOfSteps(2000);//设置流线积分最大步数
    streamline->SetIntegrationDirectionToBoth();//设置流线积分方向
    streamline->SetTerminalSpeed(1e-12);//设置流线积分终止速度
    streamline->SetMaximumError(1e-6);
    streamline->Update();

    //计算速度的模
    vtkSmartPointer<vtkArrayCalculator> calc = vtkSmartPointer<vtkArrayCalculator>::New();
    calc->SetInputConnection(streamline->GetOutputPort());
    calc->AddVectorArrayName("U");
    calc->SetFunction("mag(U)");
    calc->SetResultArrayName("u_mag");
    calc->Update();


    vtkPolyData* streamlinedata = vtkPolyData::SafeDownCast(calc->GetOutput());
    streamlinedata->GetPointData()->SetActiveScalars("u_mag");

    //计算速度范围
    double scalarRange[2]{ 0.0,1.0 };
    if (auto sxalars_ = streamlinedata->GetPointData()->GetScalars())
    {
        scalarRange[0] = sxalars_->GetRange()[0];
        scalarRange[1] = sxalars_->GetRange()[1];
    }

    vtkSmartPointer<vtkScalarBarActor> scalarBar =
        vtkSmartPointer<vtkScalarBarActor>::New();
    vtkSmartPointer<vtkLookupTable> pColorTable =
        vtkSmartPointer<vtkLookupTable>::New();
    pColorTable->SetNumberOfTableValues(31);
    pColorTable->SetHueRange(0.67, 0);//标量条颜色范围，从蓝到红
    pColorTable->SetAlphaRange(1.0, 1.0);
    pColorTable->SetValueRange(1, 1);
    pColorTable->SetSaturationRange(1, 1);
    pColorTable->SetRange(scalarRange);
    pColorTable->Build();

    scalarBar->SetTitle("U (m/s)");
    scalarBar->GetTitleTextProperty()->SetColor(0, 0, 0);
    scalarBar->GetTitleTextProperty()->SetFontFamilyToArial();
    scalarBar->GetTitleTextProperty()->SetFontSize(20);
    scalarBar->GetLabelTextProperty()->SetColor(0, 0, 0);
    scalarBar->SetLabelFormat("%5.3f");
    scalarBar->GetLabelTextProperty()->SetFontFamilyToArial();
    scalarBar->GetLabelTextProperty()->SetFontSize(20);
    scalarBar->SetNumberOfLabels(7);
    scalarBar->SetUnconstrainedFontSize(1);
    scalarBar->SetMaximumWidthInPixels(80);
    scalarBar->SetMaximumHeightInPixels(900);
    scalarBar->SetLookupTable(pColorTable);


    vtkSmartPointer<vtkPolyDataMapper> streamlinemappper = vtkSmartPointer<vtkPolyDataMapper>::New();
    streamlinemappper->SetInputData(streamlinedata);
    streamlinemappper->SetLookupTable(pColorTable);
    streamlinemappper->SetScalarRange(scalarRange);

    vtkSmartPointer<vtkActor> streamlineactor = vtkSmartPointer<vtkActor>::New();
    streamlineactor->SetMapper(streamlinemappper);

    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren->AddActor(streamlineactor);
    ren->AddActor(scalarBar);
    ren->SetBackground(1, 1, 1);//设置背景色为白色
    ren->ResetCamera();

    vtkSmartPointer<vtkRenderWindow> renWin =
        vtkSmartPointer<vtkRenderWindow>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> iren =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> TrackballCamera =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    iren->SetInteractorStyle(TrackballCamera);

    renWin->AddRenderer(ren.GetPointer());
    renWin->SetSize(500, 500);
    renWin->Render();


    iren->Initialize();
    iren->Start();

    return 0;
}

#endif // TEST35

#ifdef TEST36

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLine.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWarpScalar.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>

namespace {

    // Get the camera position.
    void CameraModifiedCallback(vtkObject* caller,
        long unsigned int vtkNotUsed(eventId),
        void* vtkNotUsed(clientData),
        void* vtkNotUsed(callData));
} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Create a random set of 100 points in the xy-plane in (0,10)x(0,10).
    // If you had instead started with a set of (x,y,z) points, you must copy
    // the zvalues into a FloatArray and set it as the data set's scalars,
    // and then set the z-coordinates to zero
    vtkNew<vtkPoints> randomPoints;

    vtkNew<vtkFloatArray> zvalues;
    zvalues->SetName("ZValues");

    unsigned int gridSize = 10;
    float maxHeight = 1;
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    randomSequence->SetSeed(8775070);
    for (unsigned int i = 0; i < 100; ++i)
    {
        double x, y, z;
        // random position and radius
        x = randomSequence->GetRangeValue(0, gridSize);
        randomSequence->Next();
        y = randomSequence->GetRangeValue(0, gridSize);
        randomSequence->Next();
        z = randomSequence->GetRangeValue(0, maxHeight);
        randomSequence->Next();


        // 两种不同的点集会产生不同的结果
        // 因为这个点集是赋值给数据源，探测点还是 （x, y, 0）
        // 当数据源的 Z 轴值为非零值时，探测的结果属性值都为0

        // randomPoints->InsertNextPoint ( x, y, z);
        randomPoints->InsertNextPoint(x, y, 0);
        zvalues->InsertNextValue(z);
    }

    // Add the grid points to a polydata object
    vtkNew<vtkPolyData> randomPolyData;
    randomPolyData->SetPoints(randomPoints);
    randomPolyData->GetPointData()->SetScalars(zvalues);

    // Triangulate the grid points. If you do not have a mesh (points
    // only), the output will not be interpolated!
    vtkNew<vtkDelaunay2D> randomDelaunay;
    randomDelaunay->SetInputData(randomPolyData);
    randomDelaunay->Update();

    // Create a grid of points to interpolate over
    vtkNew<vtkPoints> gridPoints;
    for (unsigned int x = 0; x < gridSize; x++)
    {
        for (unsigned int y = 0; y < gridSize; y++)
        {
            gridPoints->InsertNextPoint(2 * x, 2 * y, 0);
        }
    }

    // Create a dataset from the grid points
    vtkNew<vtkPolyData> gridPolyData;
    gridPolyData->SetPoints(gridPoints);

    // Perform the interpolation
    vtkNew<vtkProbeFilter> probeFilter;
    probeFilter->SetSourceConnection(randomDelaunay->GetOutputPort());
    probeFilter->SetInputData(
        gridPolyData); //
    // Interpolate 'Source' at these points
    probeFilter->Update();

    // Map the output zvalues to the z-coordinates of the data so that
    // we get a surface, rather than a flat grid with interpolated
    // scalars.
    vtkNew<vtkWarpScalar> gridWarpScalar;
    gridWarpScalar->SetInputConnection(probeFilter->GetOutputPort());
    gridWarpScalar->Update();

    //////// Setup outputs ////////
    // Output random points
    // Map the output zvalues to the z-coordinates of the data
    vtkNew<vtkWarpScalar> randomWarpScalar;
    randomWarpScalar->SetInputConnection(randomDelaunay->GetOutputPort());
    randomWarpScalar->Update();

    // Mesh the output grid points
    vtkNew<vtkDelaunay2D> gridDelaunay;
    gridDelaunay->SetInputConnection(gridWarpScalar->GetOutputPort());

    ////////// Setup visualization ////////
    vtkNew<vtkDataSetMapper> randomMapper;
    randomMapper->SetInputConnection(randomWarpScalar->GetOutputPort());
    //randomMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> randomActor;
    randomActor->SetMapper(randomMapper);
    randomActor->GetProperty()->SetColor(colors->GetColor3d("Salmon").GetData());
    randomActor->GetProperty()->SetPointSize(3);

    vtkNew<vtkDataSetMapper> gridMapper;
    gridMapper->SetInputConnection(gridDelaunay->GetOutputPort());
    //gridMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> gridActor;
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->SetColor(colors->GetColor3d("SteelBlue").GetData());
    gridActor->GetProperty()->SetPointSize(3);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("InterpolateMeshOnGrid");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(randomActor);
    //renderer->AddActor(gridActor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());


    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST36

#ifdef TEST37

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSplineFilter.h>
#include <vtkWindowLevelLookupTable.h>

#include <sstream>

namespace {
    vtkSmartPointer<vtkPolyData> SweepLine(vtkPolyData* line, double direction[3],
        double distance, unsigned int cols);
}

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    // Verify arguments
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " InputVolume PolyDataInput"
            << " Resolution" << std::endl;
        std::cout << "e.g. HeadMRVolume.mhd polyline.vtk 200" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse arguments
    std::string volumeFileName = argv[1];
    std::string polyDataFileName = argv[2];
    std::stringstream ssResolution;
    ssResolution << argv[3];
    unsigned int resolution;
    ssResolution >> resolution;

    // Output arguments
    std::cout << "InputVolume: " << volumeFileName << std::endl
        << "PolyDataInput: " << polyDataFileName << std::endl
        << "Resolution: " << resolution << std::endl;

    // Read the volume data
    vtkNew<vtkImageReader2Factory> imageFactory;
    vtkSmartPointer<vtkImageReader2> imageReader;
    imageReader.TakeReference(
        imageFactory->CreateImageReader2(volumeFileName.c_str()));
    imageReader->SetFileName(volumeFileName.c_str());
    imageReader->Update();

    // Read the Polyline
    vtkNew<vtkPolyDataReader> polyLineReader;
    polyLineReader->SetFileName(polyDataFileName.c_str());
    polyLineReader->Update();

    vtkNew<vtkSplineFilter> spline;
    spline->SetInputConnection(polyLineReader->GetOutputPort());
    spline->SetSubdivideToSpecified();
    spline->SetNumberOfSubdivisions(resolution);

    // Sweep the line to form a surface
    double direction[3];
    direction[0] = 0.0;
    direction[1] = 0.0;
    direction[2] = 1.0;
    double distance = 164;
    spline->Update();
    auto surface =
        SweepLine(spline->GetOutput(), direction, distance, atoi(argv[3]));

    // Probe the volume with the extruded surface
    vtkNew<vtkProbeFilter> sampleVolume;
    sampleVolume->SetInputConnection(1, imageReader->GetOutputPort());
    sampleVolume->SetInputData(0, surface);

    // Compute a simple window/level based on scalar range
    vtkNew<vtkWindowLevelLookupTable> wlLut;
    double range = imageReader->GetOutput()->GetScalarRange()[1] - imageReader->GetOutput()->GetScalarRange()[0];
    double level = (imageReader->GetOutput()->GetScalarRange()[1] + imageReader->GetOutput()->GetScalarRange()[0]) / 2.0;
    wlLut->SetWindow(range);
    wlLut->SetLevel(level);

    // Create a mapper and actor.
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(sampleVolume->GetOutputPort());
    mapper->SetLookupTable(wlLut);
    mapper->SetScalarRange(0, 255);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // Create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CurvedReformation");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // Set the camera for viewing medical images
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer->GetActiveCamera()->SetPosition(0, 0, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 1, 0);
    renderer->ResetCamera();

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {

    vtkSmartPointer<vtkPolyData> SweepLine(vtkPolyData* line, double direction[3],
        double distance, unsigned int cols)
    {
        unsigned int rows = line->GetNumberOfPoints();
        double spacing = distance / cols;
        vtkNew<vtkPolyData> surface;

        // Generate the points
        cols++;
        unsigned int numberOfPoints = rows * cols;
        unsigned int numberOfPolys = (rows - 1) * (cols - 1);
        vtkNew<vtkPoints> points;
        points->Allocate(numberOfPoints);
        vtkNew<vtkCellArray> polys;
        polys->Allocate(numberOfPolys * 4);

        double x[3];
        unsigned int cnt = 0;
        for (unsigned int row = 0; row < rows; row++)
        {
            for (unsigned int col = 0; col < cols; col++)
            {
                double p[3];
                line->GetPoint(row, p);
                x[0] = p[0] + direction[0] * col * spacing;
                x[1] = p[1] + direction[1] * col * spacing;
                x[2] = p[2] + direction[2] * col * spacing;
                points->InsertPoint(cnt++, x);
            }
        }
        // Generate the quads
        vtkIdType pts[4];
        for (unsigned int row = 0; row < rows - 1; row++)
        {
            for (unsigned int col = 0; col < cols - 1; col++)
            {
                pts[0] = col + row * (cols);
                pts[1] = pts[0] + 1;
                pts[2] = pts[0] + cols + 1;
                pts[3] = pts[0] + cols;
                polys->InsertNextCell(4, pts);
            }
        }
        surface->SetPoints(points);
        surface->SetPolys(polys);

        return surface;
    }
} // namespace



#endif // TEST37

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

int main(int, char* [])
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
    float inc = 7.5 / (numPoints - 1);
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

#ifdef TEST39

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkProbeFilter.h>
#include <vtkSplineFilter.h>
#include <vtkPolyLine.h>
#include <vtkDelaunay2D.h>

//probeFilter->SetInputConnection(1, );
//probeFilter->SetInputData(0, poly); //需要探测的数据
    //https://blog.csdn.net/Jane_yuhui/article/details/83991316
//probeFilter->SetSourceData(source); //源数据，需要探测的数据
//probeFilter->SetInputConnection()   //重采样（点集）

int main(int, char* [])
{
    // 顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    //// 拓扑
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 流场参数
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();        // 创建存储顶点属性的float数组
    scalars->SetNumberOfValues(27);
    for (int i = 0; i < 27; i++)        // 为属性数组中的每个元素设置标量值（这个标量值可以当作颜色值）
    {
        scalars->SetValue(i, i);
    }


    vtkSmartPointer<vtkPolyData> poly1 = vtkSmartPointer<vtkPolyData>::New();
    poly1->SetPoints(points);
    poly1->SetPolys(cell_poly);

    auto num = poly1->GetNumberOfCells();
    poly1->GetCellData()->SetScalars(scalars);


    //vtkNew<vtkProbeFilter> probeFilter;
    //probeFilter->SetInputData(0, poly);
    //probeFilter->SetInputConnection(1, filter->GetOutputPort());

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();


    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly1);
    mapper->SetScalarRange(0, 26);            // 设置标量值的范围
    mapper->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);

    auto ret = actor->GetMapper()->GetInput()->GetNumberOfCells();

    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST39

#ifdef TEST40

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkContourFilter.h>

int main(int, char* [])
{
    // 顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    // 拓扑
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 标量
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetNumberOfValues(27);
    for (int i = 0; i < 27; i++)
    {
        scalars->SetValue(i, i);
    }

    vtkSmartPointer<vtkPolyData> poly1 = vtkSmartPointer<vtkPolyData>::New();
    poly1->SetPoints(points);
    poly1->SetPolys(cell_poly);
    poly1->GetCellData()->SetScalars(scalars);

    // 单元标量数据转为顶点数据
    vtkNew<vtkCellDataToPointData> cellToPoint;
    cellToPoint->SetInputData(poly1);
    cellToPoint->PassCellDataOff();
    cellToPoint->Update(); //updata不能少

    vtkNew<vtkContourFilter> contourFilter;
    contourFilter->SetInputData(cellToPoint->GetOutput());
    contourFilter->GenerateValues(50, 0, 26);
    contourFilter->Update();

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();

    vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper1->SetInputData(cellToPoint->GetOutput());
    mapper1->SetInputData(contourFilter->GetOutput());
    mapper1->SetScalarRange(0, 26);            // 设置标量值的范围
    mapper1->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper1);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);

    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST40

#ifdef TEST41

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointDataToCellData.h>
#include <vtkDataSetMapper.h>

int main(int, char* [])
{
    // 顶点
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i + 1, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i + 1, 3, 0);
    }

    // 拓扑
    vtkSmartPointer<vtkCellArray> cell_poly = vtkSmartPointer<vtkCellArray>::New();
    for (long long i = 0; i < 33; i += 4)
    {
        cell_poly->InsertNextCell({ i,i + 1,i + 5,i + 4 });
        cell_poly->InsertNextCell({ i + 1,i + 2,i + 6,i + 5 });
        cell_poly->InsertNextCell({ i + 2,i + 3,i + 7,i + 6 });
    }

    // 标量
    vtkSmartPointer<vtkFloatArray> scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetNumberOfValues(40); // 40个点，27个单元
    for (int i = 0; i < 40; i++)
    {
        scalars->SetValue(i, i);
    }

    vtkSmartPointer<vtkPolyData> poly1 = vtkSmartPointer<vtkPolyData>::New();
    poly1->SetPoints(points);
    poly1->SetPolys(cell_poly);
    poly1->GetPointData()->SetScalars(scalars);
    //poly1->GetCellData()->SetScalars(scalars);

    vtkNew<vtkPointDataToCellData> pointToCell;
    pointToCell->SetInputData(poly1);
    //pointToCell->PassCellDataOn();
    pointToCell->Update();

    // 创建颜色查找表
    vtkSmartPointer<vtkLookupTable> hueLut = vtkSmartPointer<vtkLookupTable>::New();
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->Build();

    vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper1->SetInputData(pointToCell->GetOutput());
    mapper1->SetScalarRange(0, 39);            // 设置标量值的范围
    mapper1->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper1);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    renderer->SetBackground(.1, .2, .3);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);

    renderWindow->SetSize(600, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST41

#ifdef TEST42

#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNew.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <algorithm>
#include <vector>

int main(int argc, char* argv[])
{
    vtkNew<vtkUnstructuredGridReader> reader;
    reader->SetFileName("elbow.vtk");
    reader->Update();

    // 获取三个方向的最大最小值
    std::vector<double> bounds(6);
    reader->GetOutput()->GetBounds(bounds.data());

    // 获取三个方向的范围值
    std::vector<double> range(3);
    for (int i = 0; i < 3; ++i)
    {
        range[i] = bounds[2 * i + 1] - bounds[2 * i];
    }

    vtkNew<vtkCellCenters> centers;
    centers->SetInputData(reader->GetOutput());
    centers->Update();

    // 对于vtkCellCenters过滤器，最后的输出只存在节点，不存在单元
    // 获取单元中心的坐标值
    // vtkPolyData* cellCenter = centers->GetOutput();
    // for (vtkIdType pointIndex = 0; pointIndex < cellCenter->GetNumberOfPoints(); ++pointIndex)
    // {
    //     double pointCoord[3];
    //     cellCenter->GetPoint(pointIndex, pointCoord);
    //     std::cout << "单元 " << pointIndex << " 的中心的坐标为：" << std::endl
    //               << "X : " << pointCoord[0] << std::endl
    //               << "Y : " << pointCoord[1] << std::endl
    //               << "Z : " << pointCoord[2] << std::endl;
    // }

    // 获取三个方向范围的最大值
    double maxValue = *std::max_element(range.begin(), range.end());
    double r = maxValue * 0.001;

    // 用球体来表示单元中心
    vtkNew<vtkSphereSource> sphere;
    sphere->SetPhiResolution(11);
    sphere->SetThetaResolution(11);
    sphere->SetRadius(r);

    vtkNew<vtkGlyph3DMapper> pointMapper;
    pointMapper->SetInputConnection(centers->GetOutputPort());
    // 以球的形式表现单元中心
    pointMapper->SetSourceConnection(sphere->GetOutputPort());

    vtkNew<vtkActor> pointActor;
    pointActor->SetMapper(pointMapper);

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(reader->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.5);
    actor->GetProperty()->EdgeVisibilityOn(); // 边界线
    actor->GetProperty()->SetInterpolationToFlat();

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->AddActor(actor);
    renderer->AddActor(pointActor);

    renderer->GradientBackgroundOn();
    renderer->SetBackground(1, 1, 1);
    renderer->SetBackground2(0.4, 0.55, 0.75);
    renderer->ResetCamera();

    renderWindow->Render();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST42

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

int main(int, char* [])
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
    sphereActor->GetProperty()->SetColor(
        colors->GetColor3d("MistyRose").GetData());

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
    vtkSmartPointer<vtkAlgorithm> algorithm =
        sphereActor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    auto srcReference = dynamic_cast<vtkSphereSource*>(algorithm.GetPointer());

    float origRadius = srcReference->GetRadius();
    for (int i = 0; i < 360; ++i)
    {
        // change radius of the spheresource
        srcReference->SetRadius(origRadius *
            (1 + sin((float)i / 180.0 * vtkMath::Pi())));
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
    void RandomPointInBounds(vtkPolyData* polydata, double p[3],
        vtkMinimalStandardRandomSequence* rng);

    double TimeOctree(vtkPolyData* polydata, int maxPoints, int numberOfTrials,
        vtkMinimalStandardRandomSequence* rng);
} // namespace

int main(int, char* [])
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
        std::cout << "Put " << results[i].first << " " << results[i].second
            << " in the table." << std::endl;
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
    void RandomPointInBounds(vtkPolyData* polydata, double p[3],
        vtkMinimalStandardRandomSequence* rng)
    {
        double bounds[6];
        polydata->GetBounds(bounds);

        for (auto i = 0; i < 3; ++i)
        {
            p[i] = bounds[i * 2] +
                (bounds[i * 2 + 1] - bounds[i * 2]) * rng->GetRangeValue(0.0, 1.0);
            rng->Next();
        }
    }

    double TimeOctree(vtkPolyData* polydata, int maxPoints, int numberOfTrials,
        vtkMinimalStandardRandomSequence* rng)
    {
        vtkNew<vtkTimerLog> timer;
        timer->StartTimer();

        // Create the tree
        vtkNew<vtkOctreePointLocator> octree;
        octree->SetDataSet(polydata);
        octree->SetMaximumPointsPerRegion(maxPoints);
        octree->BuildLocator();

        std::cout << "With maxPoints = " << maxPoints << " there are "
            << octree->GetNumberOfLeafNodes() << " leaf nodes." << std::endl;

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
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkImageMapToColors.h>
#include <vtkNamedColors.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSet.h>
#include <vtkAlgorithmOutput.h>
#include <vtkGlyph3D.h>

#define Two

int main(int, char* [])
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
        cell_line->InsertNextCell({ i,i + 1 });
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
    hueLut->SetHueRange(0.67, 0.0);            // 设定HSV颜色范围，色调H取值范围为0°～360°，从红色开始按逆时针方向计算，红色为0°/0.0，绿色为120°/0.34,蓝色为240°/0.67
    hueLut->SetRampToSQRT();
    hueLut->Build();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(poly);
    mapper->SetScalarRange(0, 9);            // 设置标量值的范围
    mapper->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetLineWidth(5);
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

#ifdef Two

    //左右两个窗口用来对比
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
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
    //vtkSmartPointer<vtkDataSet> poly2 = vtkSmartPointer<vtkDataSet>::New();
    vtkSmartPointer<vtkPolyData> poly2 = vtkSmartPointer<vtkPolyData>::New();
    poly2->DeepCopy(poly1);

    poly2->GetPointData()->SetScalars(scalars2);

    //vtkSmartPointer<vtkDataSetMapper> mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputData(poly2);
    mapper2->SetScalarRange(0, 9);            // 设置标量值的范围
    mapper2->SetLookupTable(hueLut);

    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->GetProperty()->SetLineWidth(5);
    actor2->SetMapper(mapper2);

    rightRenderer->AddActor(actor2);
#else
    renderer->AddActor(actor);
#endif

    //vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer();
    //auto p = dynamic_cast<vtkGlyph3D*>(algorithm.GetPointer());

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->SetSize(1200, 800);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST45

#ifdef TEST46

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkCellLocator.h>
#include <vtkGenericCell.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCoordinate.h>

int main(int, char* [])
{
    auto sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->Update();

    // Create the tree
    auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
    cellLocator->SetDataSet(sphereSource->GetOutput());
    cellLocator->BuildLocator();

    double testPoint[3] = { 0.6, 0.5, 0.0 };

    //Find the closest points to TestPoint
    auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
    double closestPoint[3];//the coordinates of the closest point will be returned here
    double closestPointDist2; //the squared distance to the closest point will be returned here
    vtkIdType cellId; //the cell id of the cell containing the closest point will be returned here
    int subId;
    cellLocator->FindClosestPoint(testPoint, closestPoint, assistCell, cellId, subId, closestPointDist2);

    std::cout << "Coordinates of closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;
    std::cout << "Squared distance to closest point: " << closestPointDist2 << std::endl;
    std::cout << "CellId: " << cellId << std::endl;

    auto testSource = vtkSmartPointer<vtkSphereSource>::New();
    testSource->SetRadius(0.01);
    testSource->SetCenter(testPoint);
    testSource->Update();

    auto pointSource = vtkSmartPointer<vtkSphereSource>::New();
    pointSource->SetRadius(0.01);
    pointSource->SetCenter(closestPoint);
    pointSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> testMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    testMapper->SetInputConnection(testSource->GetOutputPort());
    vtkSmartPointer<vtkPolyDataMapper> pointMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    pointMapper->SetInputConnection(pointSource->GetOutputPort());

    auto sphereActor = vtkSmartPointer<vtkActor>::New();
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->EdgeVisibilityOn();
    auto pointActor = vtkSmartPointer<vtkActor>::New();
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(1, 0, 0);
    auto testActor = vtkSmartPointer<vtkActor>::New();
    testActor->SetMapper(testMapper);
    testActor->GetProperty()->SetColor(0, 1, 0);

    auto render = vtkSmartPointer<vtkRenderer>::New();
    render->AddActor(sphereActor);
    render->AddActor(pointActor);
    render->AddActor(testActor);

    auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->SetSize(640, 480);
    renWin->AddRenderer(render);

    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renWin);

    renWin->Render();
    interactor->Initialize();
    interactor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST46

#ifdef TEST47

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>

#include <vtkResizingWindowToImageFilter.h>
#include <vtkWindowToImageFilter.h>

#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkPNGWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkVRMLExporter.h>

#include <vtkImageLuminance.h>

int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;

    //mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(0.2, 0.3, 0.4);
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小

    vtkNew<vtkWindowToImageFilter> windowToImage;
    windowToImage->SetInput(renWin);
    windowToImage->SetScale(2, 3);
    windowToImage->Update();

    vtkNew<vtkResizingWindowToImageFilter> img;
    img->SetInput(renWin);
    img->SetSize(1200, 1200);
    img->Update();
    //img->SetProgressShiftScale();

    //vtkNew<vtkWindowToImageFilter> img;
    //img->SetInput(renWin);
    //img->SetScale(100, 100);
    //img->Update();

    vtkNew<vtkImageLuminance> luminance;
    luminance->SetInputData(img->GetOutput());
    luminance->Update();

    // jpg
    //vtkNew<vtkJPEGWriter> jpg;
    //jpg->SetInputData(img->GetOutput());
    ////jpg->SetFileName("test");  //不会补全.jpg，但是保存的图片名加上.jpg以后也可以显示
    //jpg->SetFileName("test.jpg");
    //jpg->Write();

    //jpg->SetInputData(luminance->GetOutput());
    //jpg->SetFileName("test1.jpg");
    //jpg->Write();

    //// tiff
    //vtkNew<vtkTIFFWriter> tiff;
    //tiff->SetInputData(img->GetOutput());
    //tiff->SetFileName("test.tiff");
    //tiff->Write();

    //vtkNew<vtkTIFFWriter> tiff1;
    //tiff1->SetInputData(luminance->GetOutput());
    //tiff1->SetFileName("test1.tiff");
    //tiff1->Write();

    //// png
    //vtkNew<vtkPNGWriter> png;
    //png->SetInputData(img->GetOutput());
    //png->SetFileName("test.png");
    //png->Write();

    //vtkNew<vtkPNGWriter> png1;
    //png1->SetInputData(luminance->GetOutput());
    //png1->SetFileName("test1.png");
    //png1->Write();

    //// bmp
    //vtkNew<vtkBMPWriter> bmp;
    //bmp->SetInputData(img->GetOutput());
    //bmp->SetFileName("test.bmp");
    //bmp->Write();

    //vtkNew<vtkBMPWriter> bmp1;
    //bmp1->SetInputData(luminance->GetOutput());
    //bmp1->SetFileName("test1.bmp");
    //bmp1->Write();

    // *.ps 矢量图
    vtkNew<vtkPostScriptWriter> ps;
    ps->SetInputData(img->GetOutput());
    ps->SetFileName("test.ps");
    ps->Write();

    std::string exportFileName = "test.wrl";
    vtkNew<vtkVRMLExporter> exporter;
    exporter->SetFileName(exportFileName.c_str());
    exporter->SetActiveRenderer(renderer);
    exporter->SetRenderWindow(renWin);
    exporter->Update();

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;

}

#endif // TSET47

#ifdef TEST48

#include <vtkCamera.h>
#include <vtkChartXYZ.h>
#include <vtkContext3D.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlotLine3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSVGExporter.h>
#include <vtkContext2D.h>
#include <vtkContext3D.h>
#include <vtkContextScene.h>
#include <vtkContextActor.h>
#include <vtkContextItem.h>
#include <vtkRendererSource.h>


//----------------------------------------------------------------------------
int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    //renderer->Render();

    // Set up a 3D scene and add an XYZ chart to it.
    vtkNew<vtkContextView> view;
    view->GetRenderWindow()->SetSize(640, 480);
    view->GetRenderWindow()->SetWindowName("PlotLine3D");
    view->GetRenderer()->SetBackground(1.0, 1.0, 0.5);
    view->GetRenderer()->SetBackground2(1.0, 0.5, 1.0);
    view->GetRenderer()->SetBackgroundAlpha(0.5);
    view->GetRenderer()->GradientBackgroundOn();

    //view->GetRenderer()->AddActor(cubeActor);
    //view->GetRenderWindow()->SetMultiSamples(10);

    //vtkNew<vtkCamera> camera;
    //camera->SetPosition(1, 1, 1);//设置相机位置
    //camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //view->GetRenderer()->SetActiveCamera(camera);
    //view->GetRenderer()->ResetCamera();

    //vtkNew<vtkContext2D> context2D;
    //vtkNew<vtkContextItem> contextItem;
    //vtkNew<vtkContextActor> contextActor;


    vtkNew<vtkRendererSource> renderSource;
    renderSource->SetInput(renderer);
    auto imageData1 = renderSource->GetOutput();

    //view->GetScene()->AddItem(contextItem);
    view->GetContext()->DrawImage(0, 0, imageData1);


    view->GetRenderWindow()->Render();
    view->GetRenderer()->Render();
    vtkNew<vtkSVGExporter> svg;
    svg->SetRenderWindow(view->GetRenderWindow());
    //svg->SetActiveRenderer(view->GetRenderer());
    svg->SetFileName("contextViewSvg.svg");
    svg->Write();

    //view->GetInteractor()->Initialize();
    //view->GetInteractor()->Start();




    return EXIT_SUCCESS;
}

#endif // TEST48

#ifdef TEST49

#include "vtkHDRReader.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include <vtkCallbackCommand.h>
#include <vtkJPEGReader.h>
#include <vtkImageViewer2.h>

#include <iostream>

int main(int argc, char* argv[])
{
    std::string file = "test.jpg";
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();

    // Visualize
    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputData(jPEGReader->GetOutput());

    imageViewer->SetColorWindow(1620);
    imageViewer->SetColorLevel(1080);
    //imageViewer->SetSlice(50);
    //imageViewer->SetSliceOrientationToXY();
    //imageViewer->SetPosition(0, 100);

    //vtkNew<ColorCallback> getColorWindow;
    //getColorWindow->imageViewer = imageViewer;

    vtkNew<vtkRenderWindowInteractor> iren;
    imageViewer->SetupInteractor(iren);
    imageViewer->GetRenderWindow()->SetWindowName("HDRReader");
    imageViewer->Render();

    //iren->AddObserver(vtkCommand::EndInteractionEvent, getColorWindow);

    iren->Start();

    auto color1 = imageViewer->GetColorLevel();
    auto color2 = imageViewer->GetColorWindow();

    std::cout << color1 << "," << color2 << "\n";

    std::cout << imageViewer->GetRenderWindow()->GetSize()[0] << ",";
    std::cout << imageViewer->GetRenderWindow()->GetSize()[1];

    //system("pause");
    return EXIT_SUCCESS;
}

#endif // TEST49

#ifdef TEST50

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestSVGContextExport.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkSVGExporter.h"

#include "vtkAbstractMapper.h"
#include "vtkBrush.h"
#include "vtkCellArray.h"
#include "vtkContext2D.h"
#include "vtkContextItem.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLContextDevice2D.h"
#include "vtkPen.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPoints2D.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkTestingInteractor.h"
#include "vtkTextProperty.h"
#include "vtkTransform2D.h"
#include "vtkTriangle.h"
#include "vtkUnsignedCharArray.h"

#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>

#include <string>

//------------------------------------------------------------------------------
class ContextSVGTest : public vtkContextItem
{
    void SetSpritePoint(int x, int y, vtkImageData* sprite);

public:
    static ContextSVGTest* New();
    vtkTypeMacro(ContextSVGTest, vtkContextItem);
    // Paint event for the chart, called whenever the chart needs to be drawn
    bool Paint(vtkContext2D* painter) override;
};

void drawPolyLinePolyData(vtkContext2D* painter)
{
    // Setup points
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(50.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 50.0, 0.0);
    points->InsertNextPoint(50.0, 0.0, 0.0);

    // Define some colors
    unsigned char black[4] = { 0, 0, 0, 255 };

    // Setup the colors array
    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(4);
    colors->SetName("Colors");

    // Add the three colors we have created to the array
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);
    colors->InsertNextTypedTuple(black);

    vtkNew<vtkCellArray> polylines;
    vtkNew<vtkPolyLine> polyline;
    polyline->GetPointIds()->SetNumberOfIds(4);
    polyline->GetPointIds()->SetId(0, 0);
    polyline->GetPointIds()->SetId(1, 1);
    polyline->GetPointIds()->SetId(2, 2);
    polyline->GetPointIds()->SetId(3, 3);
    polylines->InsertNextCell(polyline);

    // Create a polydata object and add everything to it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(polylines);
    painter->GetPen()->SetWidth(2.0);
    painter->DrawPolyData(475, 200, polydata, colors, VTK_SCALAR_MODE_USE_POINT_DATA);
}

int main(int, char* [])
{

    vtkNew<vtkCubeSource> cube;

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(1, 0, 0);

    // Set up a 2D context view, context test object and add it to the scene
    vtkNew<vtkContextView> view;
    view->GetRenderer()->SetBackground(1.0, 1.0, 0.5);
    view->GetRenderer()->SetBackground2(1.0, 0.5, 1.0);
    view->GetRenderer()->SetBackgroundAlpha(0.5);
    view->GetRenderer()->GradientBackgroundOn();
    view->GetRenderWindow()->SetSize(800, 600);
    //view->GetRenderWindow()->GetGenericContext();

    vtkNew<ContextSVGTest> test;
    view->GetScene()->AddItem(test);
    view->GetRenderer()->AddActor(cubeActor);
    // Force the use of the freetype based rendering strategy
    vtkOpenGLContextDevice2D::SafeDownCast(view->GetContext()->GetDevice())
        ->SetStringRendererToFreeType();

    view->GetRenderWindow()->SetMultiSamples(0);
    view->GetRenderWindow()->Render();

    vtkNew<vtkSVGExporter> exp;
    exp->SetRenderWindow(view->GetRenderWindow());
    exp->SetFileName("test111.svg");
    exp->Write();

#if 0
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(view->GetRenderWindow());
    view->GetRenderWindow()->GetInteractor()->Initialize();
    view->GetRenderWindow()->GetInteractor()->Start();
#endif

    return EXIT_SUCCESS;
}

// Make our new derived class to draw a diagram
vtkStandardNewMacro(ContextSVGTest);

// This function aims to test the primitives provided by the 2D API.
bool ContextSVGTest::Paint(vtkContext2D* painter)
{

    // Reset painter state that we care about:
    painter->GetBrush()->SetTexture(nullptr);
    painter->GetBrush()->SetColor(0, 0, 0, 255);
    painter->GetPen()->SetColor(0, 0, 0, 255);
    painter->GetPen()->SetWidth(1.f);
    painter->GetTextProp()->SetUseTightBoundingBox(1);
    painter->GetTextProp()->SetOrientation(0.);
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetColor(0.0, 0.0, 0.0);
    painter->GetTextProp()->SetOpacity(1.);
    painter->GetTextProp()->SetFontSize(24);
    painter->GetTextProp()->SetBold(0);
    painter->GetTextProp()->SetItalic(0);
    painter->GetTextProp()->SetFontFamilyToArial();

    // Now to draw some points
    painter->GetPen()->SetColor(0, 0, 255);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoint(10, 10);
    painter->DrawPoint(790, 10);
    painter->DrawPoint(10, 590);
    painter->DrawPoint(790, 590);

    // Test the string drawing functionality of the context
    painter->DrawString(400, 25, "SVG is used as a backend to the context.");
    painter->GetTextProp()->SetFontFamilyToTimes();
    painter->GetTextProp()->SetColor(1, 0.2, 0.1);
    painter->GetTextProp()->SetOpacity(0.5);
    painter->GetTextProp()->SetOrientation(-38.);
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(475, 250, "Testing multi-\nline justified\nand rotated text.");

    drawPolyLinePolyData(painter);

    // Draw some individual lines of different thicknesses.
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetColor(255, i * 25, 0);
        painter->GetPen()->SetWidth(1.f + static_cast<float>(i));
        painter->DrawLine(10, 50 + float(i) * 10, 60, 50 + float(i) * 10);
    }

    // Draw some individual lines of different thicknesses.
    painter->GetPen()->SetWidth(10);
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetLineType(i % (vtkPen::DENSE_DOT_LINE + 1));
        painter->GetPen()->SetColor(static_cast<unsigned char>(float(i) * 25.0), 255, 128);
        painter->DrawLine(10, 250 + float(i) * 10, 60, 250 + float(i) * 10);
    }
    painter->GetPen()->SetLineType(vtkPen::SOLID_LINE);

    // Use the draw lines function now to draw a shape.
    vtkSmartPointer<vtkPoints2D> points = vtkSmartPointer<vtkPoints2D>::New();
    points->SetNumberOfPoints(30);
    for (int i = 0; i < 30; ++i)
    {
        double point[2] = { float(i) * 25.0 + 10.0, sin(float(i) / 5.0) * 100.0 + 200.0 };
        points->SetPoint(i, point);
    }
    painter->GetPen()->SetColor(0, 255, 0);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoly(points);

    // Test the markers
    float markerPoints[10 * 2];
    unsigned char markerColors[10 * 4];
    for (int i = 0; i < 10; ++i)
    {
        markerPoints[2 * i] = 500.0 + i * 30.0;
        markerPoints[2 * i + 1] = 20 * sin(markerPoints[2 * i]) + 375.0;

        markerColors[4 * i] = static_cast<unsigned char>(255 * i / 10.0);
        markerColors[4 * i + 1] = static_cast<unsigned char>(255 * (1.0 - i / 10.0));
        markerColors[4 * i + 2] = static_cast<unsigned char>(255 * (0.3));
        markerColors[4 * i + 3] = static_cast<unsigned char>(255 * (1.0 - ((i / 10.0) * 0.25)));
    }

    for (int style = VTK_MARKER_NONE + 1; style < VTK_MARKER_UNKNOWN; ++style)
    {
        // Increment the y values:
        for (int i = 0; i < 10; ++i)
        {
            markerPoints[2 * i + 1] += 35.0;
        }
        painter->GetPen()->SetWidth(style * 5 + 5);
        // Not highlighted:
        painter->DrawMarkers(style, false, markerPoints, 10, markerColors, 4);
        // Highlight the middle 4 points.
        painter->GetPen()->SetColorF(0.9, 0.8, 0.1, 0.5);
        painter->DrawMarkers(style, true, markerPoints + 3 * 2, 4);
    }

    // Draw some points of different widths.
    for (int i = 0; i < 10; ++i)
    {
        painter->GetPen()->SetColor(0, static_cast<unsigned char>(float(i) * 25.0), 255, 255);
        painter->GetPen()->SetWidth(1.0 + float(i));
        painter->DrawPoint(75, 50 + float(i) * 10);
    }

    painter->GetPen()->SetColor(0, 0, 255);
    painter->GetPen()->SetWidth(3.0);
    painter->DrawPoints(points);

    painter->GetPen()->SetColor(100, 200, 255);
    painter->GetPen()->SetWidth(3.0);
    painter->GetBrush()->SetColor(100, 255, 100);
    painter->DrawRect(100, 50, 200, 100);

    // Add in an arbitrary quad
    painter->GetPen()->SetColor(159, 0, 255);
    painter->GetPen()->SetWidth(1.0);
    painter->GetBrush()->SetColor(100, 55, 0, 200);
    painter->DrawQuad(350, 50, 375, 150, 525, 199, 666, 45);

    // Now to test out the transform...
    vtkNew<vtkTransform2D> transform;
    transform->Translate(145, 385);
    transform->Scale(0.25, 0.25);
    transform->Rotate(-45.);
    painter->PushMatrix();
    painter->SetTransform(transform);
    painter->GetPen()->SetColor(255, 0, 0);
    painter->GetPen()->SetWidth(6.0);
    painter->DrawPoly(points);

    transform->Identity();
    transform->Translate(0, 10);
    painter->AppendTransform(transform);
    painter->GetPen()->SetColor(0, 0, 200);
    painter->GetPen()->SetWidth(2.0);
    painter->DrawPoints(points);

    transform->Identity();
    transform->Translate(0, -20);
    painter->AppendTransform(transform);
    painter->GetPen()->SetColor(200, 0, 100);
    painter->GetPen()->SetWidth(5.0);
    painter->DrawPoints(points);

    transform->Identity();
    transform->Translate(20, 200);
    painter->SetTransform(transform);
    painter->GetPen()->SetColor(0, 0, 0);
    painter->GetPen()->SetWidth(1.0);
    painter->GetBrush()->SetColor(0, 0, 100, 69);
    // Draws smooth path (Full circle, testing oddball angles):
    painter->DrawEllipseWedge(100.0, 89.0, 20, 100, 15, 75, -26.23, 333.77);
    painter->DrawEllipseWedge(100.0, 89.0, 15, 15, 0, 0, -26.23, 333.77);
    painter->DrawEllipseWedge(125.0, 89.0, 20, 100, 0, 0, -26.23, 333.77);
    // Parital circle, more odd angles:
    painter->DrawEllipseWedge(150.0, 89.0, 20, 100, 15, 75, 403.f, 541.f);
    painter->DrawEllipseWedge(150.0, 89.0, 15, 75, 0, 0, 181.f, 403.f);
    // Smooth path:
    painter->DrawEllipticArc(100.0, 89.0, 20, 100, 0, 360);
    painter->DrawEllipticArc(100.0, 89.0, 15, 15, 0, 360);
    // Partial path:
    painter->DrawEllipticArc(150.0, 89.0, 20, 100, 43, 181);

    // Remove the transform:
    painter->PopMatrix();

    // Toss some images in:
    vtkNew<vtkRTAnalyticSource> imageSrc;
    imageSrc->SetWholeExtent(0, 49, 0, 49, 0, 0);
    imageSrc->SetMaximum(1.0);
    imageSrc->Update();
    vtkImageData* image = imageSrc->GetOutput();

    // convert to RGB bytes:
    vtkFloatArray* vals = static_cast<vtkFloatArray*>(image->GetPointData()->GetScalars());
    float imgRange[2];
    vals->GetValueRange(imgRange);
    float invRange = 1.f / (imgRange[1] - imgRange[0]);
    vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::New();
    scalars->SetNumberOfComponents(3);
    scalars->SetNumberOfTuples(vals->GetNumberOfTuples());
    for (vtkIdType i = 0; i < vals->GetNumberOfTuples(); ++i)
    {
        float val = vals->GetValue(i);
        val = (val - imgRange[0]) * invRange; // normalize to (0, 1)
        scalars->SetComponent(i, 0, val * 255);
        scalars->SetComponent(i, 1, (1.f - val) * 255);
        scalars->SetComponent(i, 2, (val * val) * 255);
    }
    image->GetPointData()->SetScalars(scalars);
    scalars->Delete();
    painter->DrawImage(10, 525, image);
    painter->DrawImage(65, 500, 2.f, image);
    painter->DrawImage(vtkRectf(170, 537.5f, 25, 25), image);

    // Test transparent text over geometry:
    painter->GetTextProp()->SetOrientation(0.);
    painter->GetTextProp()->SetFontSize(175);
    painter->GetTextProp()->SetColor(1., 0., 0.);
    painter->GetTextProp()->SetOpacity(0.25);
    painter->GetTextProp()->SetBold(1);
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(600, 450, "T");

    // Test text alignment:
    float alignX = 600.f;
    float alignY = 250.f;
    float alignW = 100.f * 0.5f;
    float alignH = 50.f * 0.5f;
    painter->GetPen()->SetWidth(1.f);
    painter->GetPen()->SetColor(0, 0, 0, 255);
    painter->DrawLine(alignX, alignY - alignH, alignX, alignY + alignH);
    painter->DrawLine(alignX - alignW, alignY, alignX + alignW, alignY);
    painter->GetTextProp()->SetFontSize(32);
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToBottom();
    painter->DrawString(alignX, alignY, "dag");
    painter->GetTextProp()->SetJustificationToLeft();
    painter->GetTextProp()->SetVerticalJustificationToBottom();
    painter->DrawString(alignX, alignY, "dig");
    painter->GetTextProp()->SetJustificationToRight();
    painter->GetTextProp()->SetVerticalJustificationToTop();
    painter->DrawString(alignX, alignY, "dog");
    painter->GetTextProp()->SetJustificationToLeft();
    painter->GetTextProp()->SetVerticalJustificationToTop();
    painter->DrawString(alignX, alignY, "dug");
    // Centering:
    float rect[4];
    const char* centerString = "Center";
    painter->ComputeStringBounds(centerString, rect);
    rect[0] += 350.f;
    rect[1] += 550.f;
    painter->GetBrush()->SetColor(0, 0, 0, 0);
    painter->DrawRect(rect[0], rect[1], rect[2], rect[3]);
    painter->GetTextProp()->SetJustificationToCentered();
    painter->GetTextProp()->SetVerticalJustificationToCentered();
    painter->DrawString(rect[0] + rect[2] * 0.5f, rect[1] + rect[3] * 0.5f, centerString);

    // Texturing:
    vtkNew<vtkImageData> pattern;
    pattern->SetDimensions(6, 6, 1);
    vtkNew<vtkUnsignedCharArray> patternScalars;
    patternScalars->SetNumberOfComponents(3);
    patternScalars->SetNumberOfTuples(36);
    patternScalars->FillValue(0);
    for (vtkIdType row = 0; row < 6; ++row)
    {
        for (vtkIdType col = 0; col < 6; ++col)
        {
            // Alternate red/blue:
            vtkIdType i = row * 6 + col;
            vtkIdType test = row + col;
            patternScalars->SetTypedComponent(i, (test % 2 == 0) ? 0 : 2, 255);
        }
    }
    pattern->GetPointData()->SetScalars(patternScalars);
    painter->GetBrush()->SetTexture(pattern);
    painter->GetBrush()->SetOpacity(0);

    // Stretching:
    painter->GetBrush()->SetTextureProperties(vtkBrush::Nearest | vtkBrush::Stretch);
    painter->DrawQuad(200, 485, 300, 400, 190, 420, 125, 390);

    // Tiling:
    painter->GetBrush()->SetTextureProperties(vtkBrush::Linear | vtkBrush::Repeat);
    painter->DrawQuad(300, 585, 400, 500, 290, 520, 230, 560);

    painter->GetBrush()->SetTexture(nullptr);

    // Some point sprites:
    vtkNew<vtkImageData> sprite;
    sprite->SetDimensions(25, 25, 1);
    vtkNew<vtkUnsignedCharArray> spriteScalars;
    spriteScalars->SetNumberOfComponents(3);
    spriteScalars->SetNumberOfTuples(25 * 25);
    spriteScalars->FillValue(0);
    sprite->GetPointData()->SetScalars(spriteScalars);
    std::vector<float> spritePoints;
    spritePoints.reserve(50);
    std::vector<unsigned char> spriteColors;
    spriteColors.reserve(100);
    for (int i = 0; i < 25; ++i)
    {
        this->SetSpritePoint(i, 0, sprite);
        this->SetSpritePoint(0, i, sprite);
        this->SetSpritePoint(i, i, sprite);
        this->SetSpritePoint(10, i, sprite);
        this->SetSpritePoint(i, 10, sprite);
        spritePoints.push_back(790.f);                                           // x
        spritePoints.push_back(50.f + i * 20);                                   // y
        spriteColors.push_back(static_cast<unsigned char>(127 + 128 / (i + 1))); // r
        spriteColors.push_back(static_cast<unsigned char>(255 - 128 / (i + 1))); // g
        spriteColors.push_back(static_cast<unsigned char>(64 + 128 / (i + 1)));  // b
        spriteColors.push_back(static_cast<unsigned char>(64 + 191 / (i + 1)));  // a
    }
    for (int i = 0; i < 10; ++i)
    {
        this->SetSpritePoint(24 - i, i, sprite);
    }
    painter->GetPen()->SetWidth(18);
    painter->DrawPointSprites(sprite, spritePoints.data(), 25, spriteColors.data(), 4);

    painter->GetPen()->SetColor(0, 255, 0, 64);
    painter->GetPen()->SetWidth(1);
    painter->DrawLine(781, 0, 781, 600);
    painter->DrawLine(799, 0, 799, 600);

    // Leaving this untested, since I can't find a viewer that seems to implement
    // clipPaths...webkit seems to not support userSpaceOnUse clipPath coords,
    // and forces objectBoundBox coords.
#if 0
  // Test clipping (why does Context2D not expose these directly??)
    vtkContextDevice2D* dev = painter->GetDevice();
    std::array<int, 4> clipRect = { 325, 385, 150, 100 }; // xywh
    painter->GetPen()->SetColor(0, 128, 255, 255);
    painter->GetBrush()->SetColor(0, 128, 255, 64);
    painter->DrawQuad(clipRect[0], clipRect[1],
        clipRect[0] + clipRect[2], clipRect[1],
        clipRect[0] + clipRect[2], clipRect[1] + clipRect[3],
        clipRect[0], clipRect[1] + clipRect[3]);

    dev->SetClipping(clipRect.data());
    dev->EnableClipping(true);
    painter->GetBrush()->SetColor(0, 255, 128, 128);
    painter->DrawWedge(clipRect[0] + clipRect[2] / 2.f,
        clipRect[1] + clipRect[3] / 2.f,
        75.f, 50.f, 0.f, 360.f);
    dev->EnableClipping(false);
#endif

    return true;
}

//------------------------------------------------------------------------------
void ContextSVGTest::SetSpritePoint(int x, int y, vtkImageData* sprite)
{
    unsigned char* ptr = static_cast<unsigned char*>(sprite->GetScalarPointer(x, y, 0));
    std::fill(ptr, ptr + 3, 255);
}


#endif // TEST50

#ifdef TEST51

#include <vtkPlaneSource.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNamedColors.h>

#include <array>
#include <vtkProperty.h>

#include "vtkSmartPointer.h"
#include "vtkLineSource.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"

#include "vtkInteractorStyleRubberBand2D.h"

#include <vtkSmartPointer.h>
#include <vtkRectilinearWipeWidget.h>
#include <vtkRectilinearWipeRepresentation.h>
#include <vtkJPEGReader.h>
#include <vtkImageRectilinearWipe.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty2D.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorStyleImage.h>
#include <vtkResliceImageViewer.h>
#include <vtkBMPReader.h>
#include <vtkInteractorStyleRubberBand3D.h>


class MyRubberBand : public vtkInteractorStyleRubberBand3D
{
public:
    static MyRubberBand* New();
    vtkTypeMacro(MyRubberBand, vtkInteractorStyleRubberBand3D);

    virtual void OnLeftButtonDown()
    {
        vtkInteractorStyleRubberBand3D::OnLeftButtonDown(); //必须有这一句
        //if (mExistActor)
        //{
        //    return;
        //}
        startPosition[0] = this->GetInteractor()->GetEventPosition()[0];
        startPosition[1] = this->GetInteractor()->GetEventPosition()[1];
        startPosition[2] = 0;

        std::cout << "startPosition: " << startPosition[0] << "  " << startPosition[1] << std::endl;
    }
    virtual void OnLeftButtonUp()
    {
        vtkInteractorStyleRubberBand3D::OnLeftButtonUp(); //必须有这一句

        //if (mExistActor)
        //{
            //return;
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

        point1[0] = left[0];  point1[1] = left[1];  point1[2] = 0;
        point2[0] = left[0];  point2[1] = right[1]; point2[2] = 0;
        point3[0] = right[0]; point3[1] = right[1]; point3[2] = 0;
        point4[0] = right[0]; point4[1] = left[1];  point4[2] = 0;

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
    vtkSmartPointer<vtkImageViewer2>  viewer;
    //vtkSmartPointer<vtkResliceImageViewer>  viewer;
    void SetRender(vtkImageViewer2* render)
    {
        viewer = render;
    }
protected:
    MyRubberBand()
    {
        //viewer = vtkSmartPointer<vtkImageViewer2>::New();
        //viewer->SetupInteractor(this->GetInteractor());
        mExistActor = false;
    }
    // 
private:
    double startPosition[3];
    double endPosition[3];

    bool mExistActor;
    //vtkImageViewer2 * mRender;

};
vtkStandardNewMacro(MyRubberBand);

int main(int, char* [])
{
    vtkSmartPointer<vtkNamedColors> colors =
        vtkSmartPointer<vtkNamedColors>::New();

    // Set the background color.
    std::array<unsigned char, 4> bkg{ { 26, 51, 77, 255 } };
    colors->SetColor("BkgColor", bkg.data());


    // Create a renderer, render window and interactor
    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    //renderWindowInteractor->SetRenderWindow(renderWindow);


    vtkSmartPointer<vtkBMPReader> reader1 =
        vtkSmartPointer<vtkBMPReader>::New();
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
    vtkSmartPointer<vtkRenderWindowInteractor> rwi =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    viewer->SetupInteractor(rwi);
    vtkSmartPointer<MyRubberBand> g_style = vtkSmartPointer<MyRubberBand>::New();

    g_style->SetInteractor(rwi);

    rwi->SetInteractorStyle(g_style);



    g_style->SetRender(viewer);


    //设置交互属性


    rwi->Start();
    return EXIT_SUCCESS;
}

#endif // TEST51

#ifdef TEST52

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkInteractorStyleRubberBand2D.h>

#include <string>

namespace {

    class vtkBorderCallback2 : public vtkCommand
    {
    public:
        vtkBorderCallback2()
        {
        }

        static vtkBorderCallback2* New()
        {
            return new vtkBorderCallback2;
        }

        virtual void Execute(vtkObject* caller, unsigned long, void*)
        {

            vtkBorderWidget* borderWidget = reinterpret_cast<vtkBorderWidget*>(caller);

            // 获取到的是viewport坐标，且pos2表示的是width和height并不是实际坐标
            auto pos1 = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition();
            auto pos2 = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition2();

            //std::cout << "======== pos\t" << pos1[0] << ",\t" << pos1[1] << ",\t" << pos2[0] << ",\t" << pos2[1] << "\n";

            // Get the world coordinates of the two corners of the box
            auto lowerLeftCoordinate = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPositionCoordinate();
            auto lowerLeft = lowerLeftCoordinate->GetComputedWorldValue(this->LeftRenderer);


            auto upperRightCoordinate = static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->GetPosition2Coordinate();
            auto upperRight = upperRightCoordinate->GetComputedWorldValue(this->LeftRenderer);
            //std::cout << "World:\t\t" << lowerLeft[0] << ", \t"<< lowerLeft[1] << ", \t" << lowerLeft[2] << ", \t" << upperRight[0] << ", \t" << upperRight[1] << ", \t" << upperRight[2] << std::endl;

            auto ret1 = upperRightCoordinate->GetComputedDisplayValue(this->LeftRenderer);
            auto ret2 = lowerLeftCoordinate->GetComputedDisplayValue(this->LeftRenderer);
            //std::cout << "Display:\t" << ret1[0] << ", \t" << ret1[1] << ", \t" << ret1[2] << ", \t" << ret2[0] << ", \t" << ret2[1] << ", \t" << ret2[2] << std::endl;

            auto ret3 = upperRightCoordinate->GetComputedViewportValue(this->LeftRenderer);
            auto ret4 = lowerLeftCoordinate->GetComputedViewportValue(this->LeftRenderer);
            //std::cout << "Viewport:\t" << ret3[0] << ", \t" << ret3[1] << ", \t" << ret3[2] <<", \t" << ret4[0] << ", \t" << ret4[1] << ", \t" << ret4[2] << std::endl;

            auto ret5 = upperRightCoordinate->GetComputedLocalDisplayValue(this->LeftRenderer);
            auto ret6 = lowerLeftCoordinate->GetComputedLocalDisplayValue(this->LeftRenderer);
            //std::cout << "Local:\t\t" << ret5[0] << ", \t" << ret5[1] << ", \t" << ret5[2] << ", \t" << ret6[0] << ", \t" << ret6[1] << ", \t" << ret6[2] << std::endl;

            auto size = this->LeftRenderer->GetSize();
            std::cout << size[0] << "," << size[1] << '\n';

            double* bounds = this->ImageActor->GetBounds();
            double xmin = bounds[0];
            double xmax = bounds[1];
            double ymin = bounds[2];
            double ymax = bounds[3];

            if ((lowerLeft[0] > xmin) && (upperRight[0] < xmax) &&
                (lowerLeft[1] > ymin) && (upperRight[1] < ymax))
            {
                this->ClipFilter->SetOutputWholeExtent(
                    vtkMath::Round(lowerLeft[0]), vtkMath::Round(upperRight[0]),
                    vtkMath::Round(lowerLeft[1]), vtkMath::Round(upperRight[1]), 0, 1);

                //std::cout << lowerLeft[0] << "," << upperRight[0] << "," << lowerLeft[1] << "," << upperRight[1] << std::endl;

                //this->ClipFilter->SetOutputWholeExtent(100, 100, 200, 200, 0, 1);
            }
            else
            {
                //std::cout << "box is NOT inside image" << std::endl;
            }
        }

        void SetLeftRenderer(vtkSmartPointer<vtkRenderer> renderer)
        {
            this->LeftRenderer = renderer;
        }
        void SetImageActor(vtkSmartPointer<vtkImageActor> actor)
        {
            this->ImageActor = actor;
        }
        void SetClipFilter(vtkSmartPointer<vtkImageClip> clip)
        {
            this->ClipFilter = clip;
        }

    private:
        vtkSmartPointer<vtkRenderer> LeftRenderer;
        vtkSmartPointer<vtkImageActor> ImageActor;
        vtkSmartPointer<vtkImageClip> ClipFilter;
    };

} // namespace

/*
vtkBorderWidget设置位置
    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->SetPosition(x1,y1)
    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())->SetPosition2(x2,y2)
    x1,y1是窗口左下角的viewport位置（即最大为1，最小为0）
    x2,y2实际是窗口的宽和高对应的比例，例如pos1(0.5,0.5),pos2(0.2,0.2)相当于setViewPort(0.5,0.5,0.7,0.7)
    vtk左下角为(0,0)右上角为(1,1)
*/

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> color;

    std::string inputFilename = "resource/test1.jpg";

    // Read the image
    vtkNew<vtkJPEGReader> jPEGReader;

    if (!jPEGReader->CanReadFile(inputFilename.c_str()))
    {
        std::cout << "Error: cannot read " << inputFilename << std::endl;
        return EXIT_FAILURE;
    }

    jPEGReader->SetFileName(inputFilename.c_str());
    jPEGReader->Update();

    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);
    // xmin, xmax, ymin, ymax
    std::cout << "extent: " << extent[0] << " " << extent[1] << " " <<
        extent[2] << " " << extent[3] << " " << extent[4] << " " <<
        extent[5] << std::endl;

    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputConnection(jPEGReader->GetOutputPort());

    vtkNew<vtkRenderWindow> renderWindow;

    vtkNew<vtkRenderWindowInteractor> interactor;

    //vtkNew<vtkInteractorStyleImage> style;
    vtkNew<vtkInteractorStyleRubberBand2D> style;
    interactor->SetInteractorStyle(style);

    vtkNew<vtkBorderWidget> borderWidget;
    borderWidget->SetInteractor(interactor);
    //static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
    //    ->GetBorderProperty()
    //    ->SetColor(color->GetColor3d("Lime").GetData());

    auto ret = borderWidget->GetBorderRepresentation();  //返回空，所以必须获取borderRepresentation再转换

    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
        ->SetBorderColor(1, 0, 0);

    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation());
    borderWidget->SelectableOff();

    interactor->SetRenderWindow(renderWindow);

    // Define viewport ranges in normalized coordinates
    // (xmin, ymin, xmax, ymax)
    double leftViewport[4] = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    // Setup both renderers
    vtkNew<vtkRenderer> leftRenderer;
    renderWindow->AddRenderer(leftRenderer);
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(color->GetColor3d("DarkSlateGray").GetData());

    vtkNew<vtkRenderer> rightRenderer;
    renderWindow->AddRenderer(rightRenderer);
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(color->GetColor3d("DimGray").GetData());

    leftRenderer->AddActor(imageActor);

    leftRenderer->ResetCamera();
    rightRenderer->ResetCamera();

    vtkNew<vtkImageClip> imageClip;
    imageClip->SetInputConnection(jPEGReader->GetOutputPort());
    jPEGReader->UpdateInformation();
    imageClip->SetOutputWholeExtent(jPEGReader->GetOutputInformation(0)->Get(
        vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
    imageClip->ClipDataOn();

    vtkNew<vtkImageActor> clipActor;
    clipActor->GetMapper()->SetInputConnection(imageClip->GetOutputPort());

    rightRenderer->AddActor(clipActor);

    vtkNew<vtkBorderCallback2> borderCallback;
    borderCallback->SetLeftRenderer(leftRenderer);
    borderCallback->SetImageActor(imageActor);
    borderCallback->SetClipFilter(imageClip);

    borderWidget->AddObserver(vtkCommand::InteractionEvent, borderCallback);

    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("ImageClip");
    renderWindow->Render();
    borderWidget->On();
    interactor->Start();

    return EXIT_SUCCESS;
}



#endif // TEST52

#ifdef TEST53

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkCommand.h>
#include <vtkImageActor.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleImage.h>
#include <vtkJPEGReader.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkXMLPolyDataReader.h>

#include <string>

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> color;
    std::string inputFilename = "file\\57_test1.jpg";

    // Read the image
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(inputFilename.c_str());
    jPEGReader->Update();
    // 图片大小
    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);

    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> interactor;

    // 图片交互器，滑动鼠标会让图片对比度变化，一会亮一会暗
    //vtkNew<vtkInteractorStyleImage> style;
    //interactor->SetInteractorStyle(style);

    interactor->SetRenderWindow(renderWindow);

    // 裁剪图片
    vtkNew<vtkImageClip> imageClip;
    imageClip->SetInputConnection(jPEGReader->GetOutputPort());
    //jPEGReader->UpdateInformation();
    //imageClip->SetOutputWholeExtent(jPEGReader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
    imageClip->SetOutputWholeExtent(0, 100, 0, 100, 0, 0);
    imageClip->ClipDataOn();
    imageClip->Update();

    // 添加图片actor
    vtkNew<vtkImageActor> imageActor;
    imageActor->GetMapper()->SetInputData(imageClip->GetOutput());  // 如果不调用vtkImageClip的Updata函数，此处不能用数据集直接设置
    //imageActor->GetMapper()->SetInputConnection(imageClip->GetOutputPort());
    vtkNew<vtkRenderer> renderer;
    renderWindow->AddRenderer(renderer);
    renderer->AddActor(imageActor);
    //renderer->ResetCamera();

    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("ImageClip");
    renderWindow->Render();
    renderWindow->Frame();
    renderWindow->CopyResultFrame();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST53

#ifdef TEST54
#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkImageData.h>

#include <vtkImageCanvasSource2D.h>

#include <string>

std::string file = "test.jpg";

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();
    // 图片大小
    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);

    // 创建一个纯色图片，用来给原图做阴影效果
    //vtkNew<vtkImageData> image;
    //image->SetDimensions(extent[1] + 1, extent[3] + 1, extent[5] + 1);

    // Create an image
    //vtkNew<vtkImageCanvasSource2D> drawing;
    //drawing->SetScalarTypeToUnsignedChar();
    //drawing->SetNumberOfScalarComponents(4);  //设置颜色分量（r,g,b,a)共4个，设置为3则只有(r,g,b)
    ////drawing->SetExtent(0, 200, 0, 200, 0, 0); //大小
    //drawing->SetExtent(0, extent[1]+1, 0, extent[3]+1, 0, 0); //大小

    //drawing->SetDrawColor(255, 0, 0, 255);  // 注意此处的颜色值为[0,255]，第4个参数是alpha，可以理解为透明度
    //drawing->FillBox(0, 200, 0, 200);   //画一个矩形（实心）
    //drawing->SetDrawColor(255, 0, 0, 0);
    //drawing->FillBox(50, 100, 50, 100);
    ////drawing->DrawCircle(100, 100, 50); //画一个圆圈（非实心）
    //drawing->Update();

    vtkNew<vtkImageBlend> blend;
    blend->AddInputConnection(jPEGReader->GetOutputPort());
    blend->AddInputConnection(jPEGReader->GetOutputPort());
    //blend->AddInputData(jPEGReader->GetOutput());
    //blend->AddInputData(drawing->GetOutput());
    // 设置不透明度
    blend->SetOpacity(0, 0.5);
    blend->SetOpacity(1, 0.5);

    // Display the result
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

    vtkNew<vtkImageViewer2> imageViewer;
    imageViewer->SetInputConnection(blend->GetOutputPort());
    imageViewer->SetupInteractor(renderWindowInteractor);
    imageViewer->GetRenderer()->ResetCamera();
    imageViewer->GetRenderer()->SetBackground(
        colors->GetColor3d("Peru").GetData());
    imageViewer->GetRenderWindow()->SetWindowName("CombineImages");

    imageViewer->GetRenderer()->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST54

#ifdef TEST55

#include <vtkActor.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkSphereSource.h>

#include <array>

int main(int, char* [])
{

    vtkNew<vtkNamedColors> colors;
    //colors->SetColor("Bkg", 0.2, 0.3, 0.4);

    // A sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(4.0);
    sphereSource->SetPhiResolution(4);
    sphereSource->SetThetaResolution(8);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("LogoWidget");

    renderer->AddActor(actor);

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Create an image
    vtkNew<vtkImageCanvasSource2D> drawing;
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetNumberOfScalarComponents(4);  //设置颜色分量（r,g,b,a)共4个，设置为3则只有(r,g,b)
    drawing->SetExtent(0, 200, 0, 200, 0, 0); //大小



    // Clear the image
    // Note: SetDrawColour() uses double values of the rgb colors in the
    //       range [0 ... 255]
    //       So SetDrawColour(255, 255, 255) is white.
    drawing->SetDrawColor(255, 0, 0, 255);
    drawing->FillBox(0, 200, 0, 200);
    drawing->SetDrawColor(255, 0, 0, 0);
    drawing->FillBox(50, 100, 50, 100);
    //drawing->DrawCircle(100, 100, 50);
    drawing->Update();

    vtkNew<vtkLogoRepresentation> logoRepresentation;
    logoRepresentation->SetImage(drawing->GetOutput());
    logoRepresentation->SetPosition(0, 0);
    logoRepresentation->SetPosition2(0.4, 0.4);
    logoRepresentation->GetImageProperty()->SetOpacity(.7);

    vtkNew<vtkLogoWidget> logoWidget;
    logoWidget->SetInteractor(renderWindowInteractor);
    logoWidget->SetRepresentation(logoRepresentation);

    renderWindow->Render();
    logoWidget->On();
    renderer->SetBackground(colors->GetColor3d("MidnightBLue").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST55

#ifdef TEST56

#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
//#include <vtkImageReader2.h>
//#include <vtkImageReader2Factory.h>
//#include <vtkImageViewer2.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkRenderWindow.h>

#include <array>

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkRenderer> render;
    vtkNew<vtkImageActor> actor;
    vtkNew<vtkRenderWindow> window;

    std::string file = "test.jpg";

    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName(file.c_str());
    reader->Update();

    vtkImageData* image = reader->GetOutput();
    auto extent = image->GetExtent();


    // Draw a circle in the center of the image
    //vtkNew<vtkImageCanvasSource2D> drawing;

    vtkSmartPointer drawing = vtkSmartPointer<vtkImageCanvasSource2D>::New();
    drawing->SetScalarTypeToUnsignedChar();
    drawing->SetNumberOfScalarComponents(4);  //设置颜色为4维，即rgba
    drawing->SetExtent(image->GetExtent());
    drawing->Update();

    auto type1 = reader->GetOutput()->GetScalarType();
    auto type2 = drawing->GetOutput()->GetScalarType();
    auto type3 = drawing->GetOutput()->GetScalarPointer();

    drawing->SetDrawColor(0, 0, 0, 100); // 黑色，alpha为100
    drawing->FillBox(0, extent[1], 0, extent[3]);
    // FillBox的参数一定要注意：0:lowerX,1:upperX,2:lowerY,3:upperY
    drawing->SetDrawColor(255, 255, 255, 0);// 白色，alpha为0，即完全透明
    drawing->FillBox(50, 500, 50, 500);
    //drawing->SetDrawColor(drawColor2.data());
    //drawing->DrawCircle(center[0], center[1], radius);

    vtkNew<vtkImageBlend> blend;
    //blend->AddInputConnection(imgReader->GetOutputPort());
    //blend->AddInputConnection(drawing->GetOutputPort());
    blend->AddInputData(reader->GetOutput());
    blend->AddInputData(drawing->GetOutput());
    blend->SetOpacity(0, 1.); //第0张输入图片，不透明度为1
    blend->SetOpacity(1, 1.);
    blend->Update();

    // 重新绘制可以覆盖掉之前的
    drawing->SetDrawColor(0, 0, 0, 100); // 黑色，alpha为100
    drawing->FillBox(0, extent[1], 0, extent[3]);
    drawing->SetDrawColor(255, 0, 0, 255);
    drawing->FillBox(500, 800, 500, 800);
    drawing->Update();  //如果要覆盖之前的，必须在此处调用update

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    //actor->GetMapper()->SetInputData(blend->GetOutput());  //使用独立数据集方式绘制不出来，得使用管道方式
    actor->GetMapper()->SetInputConnection(blend->GetOutputPort());
    render->AddActor(actor);
    window->AddRenderer(render);
    window->SetInteractor(renderWindowInteractor);
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST56

#ifdef TEST57

#include <vtkCamera.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageMandelbrotSource.h>
#include <vtkImageMapper3D.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkCommand.h>

#include <filesystem>

class Callback :public vtkCommand
{
public:
    static Callback* New()
    {
        return new Callback;
    }
    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {


        auto window = static_cast<vtkWindow*>(caller);
        auto w = window->GetSize()[0];
        auto h = window->GetSize()[1];
        double w_h = static_cast<double>(w) / static_cast<double>(h);
        double x_y = m_xDistance / m_yDistance;

        std::cout << "resize window:\t" << m_camera->GetParallelScale() << '\t' << w << ',' << h << '\t' <<
            m_xDistance << ',' << m_yDistance << "       \t" << w_h << "      \t" << x_y << '\n';

        if (w_h > x_y)
        {
            m_camera->SetParallelScale(0.5 * m_yDistance);
        }
        else
        {
            m_camera->SetParallelScale(0.5 * m_xDistance / w_h);
        }

        //if (w > h)
        //{
        //    m_camera->SetParallelScale(0.5 * m_yDistance);
        //}
        //else
        //{
        //    m_camera->SetParallelScale(0.5 * m_yDistance / w_h);
        //}

    }
    void SetData(vtkSmartPointer<vtkCamera> camera, double x, double y)
    {
        m_camera = camera;
        m_xDistance = x;
        m_yDistance = y;
    }
private:
    vtkSmartPointer<vtkCamera> m_camera;
    double m_xDistance{ 0.0 };
    double m_yDistance{ 0.0 };
};

int main(int, char* [])
{
    auto path = std::filesystem::current_path();
    std::string file = path.string() + "\\file\\57_test1.jpg";
    //std::string file = "57_test2.jpg";

    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName(file.c_str());
    jPEGReader->Update();

    int extent[6];
    jPEGReader->GetOutput()->GetExtent(extent);
    double origin[3];
    jPEGReader->GetOutput()->GetOrigin(origin);
    double spacing[3];
    jPEGReader->GetOutput()->GetSpacing(spacing);

    vtkNew<vtkImageActor> imageActor;
    imageActor->SetInputData(jPEGReader->GetOutput());

    // Setup renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(imageActor);
    renderer->ResetCamera();

    // Setup render window
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("FillWindow");

    vtkCamera* camera = renderer->GetActiveCamera();
    camera->ParallelProjectionOn();

    double xc = origin[0] + 0.5 * (extent[0] + extent[1]) * spacing[0];
    double yc = origin[1] + 0.5 * (extent[2] + extent[3]) * spacing[1];
    double xd = (extent[1] - extent[0] + 1) * spacing[0]; // not used
    double yd = (extent[3] - extent[2] + 1) * spacing[1];

    //float d = camera->GetDistance();
    //camera->SetParallelScale(0.5f * static_cast<float>(yd));
    //camera->SetParallelScale(0.5f * static_cast<float>(xd));
    //camera->SetFocalPoint(xc, yc, 0.0);
    //camera->SetPosition(xc, yc, +d);

    //if (renderWindow->GetSize()[0] < renderWindow->GetSize()[1])
    //{
    //    camera->Zoom(double(renderWindow->GetSize()[0]) / renderWindow->GetSize()[1]);
    //}
    renderWindow->SetSize(400, 800);
    auto w = renderWindow->GetSize()[0];
    auto h = renderWindow->GetSize()[1];
    //w>h x>y
    //w>h x<y
    //camera->SetParallelScale(0.5 * yd);
    //w<h x<y
    //w<h x>y
    camera->SetParallelScale(0.5 * xd / ((double)w / h));

    auto cb = Callback::New();
    cb->SetData(camera, xd, yd);
    renderWindow->AddObserver(vtkCommand::WindowResizeEvent, cb);


    // 1和2效果一样
    // *** 1
    //camera->SetParallelScale(0.5 * yd);
    //camera->Zoom(w/h);
    // *** 2
    //camera->SetParallelScale(0.5 * yd/((double)w/h));

    /*zoom的源码：*/
    //this->SetParallelScale(this->ParallelScale / amount);

    // Setup render window interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkNew<vtkInteractorStyleImage> style;
    renderWindowInteractor->SetInteractorStyle(style);
    //renderWindowInteractor->SetInteractorStyle(nullptr);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->Render();
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST57

#ifdef TEST58

#include <vtkCubeSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

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

int main(int, char* [])
{
    vtkNew<vtkCubeSource> cube;
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    //actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    //camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);//设置相机位置
    camera->SetFocalPoint(0, 0, 0);//设置相机焦点

    //renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    //RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);//设置window大小
    auto cb = CallBack::New();
    renWin->AddObserver(vtkCommand::WindowResizeEvent, cb);

    //RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    //数据交互
    renWin->Render();
    iren->Start();

    return 0;

}

#endif // TEST58

#ifdef TEST59

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkLineWidget2.h>
#include <vtkLineRepresentation.h>
#include <vtkCommand.h>

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

        vtkLineWidget2* lineWidget =
            reinterpret_cast<vtkLineWidget2*>(caller);

        // Get the actual box coordinates of the line
        vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
        static_cast<vtkLineRepresentation*>(lineWidget->GetRepresentation())->GetPolyData(polydata);

        // Display one of the points, just so we know it's working
        double p[3];
        polydata->GetPoint(0, p);
        std::cout << "P: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    vtkLineCallback() {}

};


int main(int, char* [])
{
    vtkSmartPointer<vtkSphereSource> sphereSource =
        vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->Update();

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // A renderer and render window
    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    //renderer->AddActor(actor);

    // An interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkLineWidget2> lineWidget =
        vtkSmartPointer<vtkLineWidget2>::New();
    lineWidget->SetInteractor(renderWindowInteractor);
    lineWidget->CreateDefaultRepresentation();

    // You could do this if you want to set properties at this point:
    //vtkSmartPointer<vtkLineRepresentation> lineRepresentation = 
      //vtkSmartPointer<vtkLineRepresentation>::New();
    //lineWidget->SetRepresentation(lineRepresentation);

    vtkSmartPointer<vtkLineCallback> lineCallback =
        vtkSmartPointer<vtkLineCallback>::New();

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

#include "vtkTextRenderer.h"
#include "vtkNew.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStdString.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"
#include "vtkUnicodeString.h"
#include <windows.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    //if (argc < 2)
    //{
    //    cerr << "Missing font filename." << endl;
    //    return EXIT_FAILURE;
    //}

    //std::string uncodeFontFile(argv[1]);

    vtkSmartPointer<vtkTextRenderer> tren = vtkSmartPointer<vtkTextRenderer>::New();
    if (tren == nullptr)
    {
        std::cerr << "Object factory cannot find vtkTextRenderer override.\n";
        return EXIT_FAILURE;
    }

    if (strcmp(tren->GetClassName(), "vtkMathTextFreeTypeTextRenderer") != 0)
    {
        std::cerr << "Object factory returning unrecognized vtkTextRenderer "
            "override: " << tren->GetClassName() << std::endl;
        return EXIT_FAILURE;
    }

    std::string str = "Sample multiline\ntext rendered\nusing FreeTypeTools.";
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
    //actor15->GetTextProperty()->SetFontFile(uncodeFontFile.c_str());
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
#include <vtkCommand.h>

namespace {

    void SelectionChangedCallbackFunction(vtkObject* caller,
        long unsigned int eventId,
        void* clientData, void* callData);

}

int main(int, char* [])
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
    //style->AddObserver(vtkCommand::SelectionChangedEvent, [](vtkObject* caller, unsigned long, void*, void*) {});

    renderWindowInteractor->SetInteractorStyle(style);

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {

    void SelectionChangedCallbackFunction(vtkObject* vtkNotUsed(caller),
        long unsigned int vtkNotUsed(eventId),
        void* vtkNotUsed(clientData),
        void* callData)
    {
        std::cout << "SelectionChanged callback" << std::endl;

        unsigned int* rect = reinterpret_cast<unsigned int*>(callData);
        unsigned int pos1X = rect[0];
        unsigned int pos1Y = rect[1];
        unsigned int pos2X = rect[2];
        unsigned int pos2Y = rect[3];

        std::cout << "Start x: " << pos1X << " Start y: " << pos1Y
            << " End x: " << pos2X << " End y: " << pos2Y << std::endl;
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
                //unsigned char* pixels = this->PixelArray->GetPointer(0);
                unsigned char* pixels = nullptr;
                //this->Interactor->GetRenderWindow()->SetRGBACharPixelData(
                //    0, 0, 300, 300, pixels, 0);
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



            std::cout << "Start position: " << this->StartPosition[0] << " "
                << this->StartPosition[1] << std::endl;
            std::cout << "End position: " << this->EndPosition[0] << " "
                << this->EndPosition[1] << std::endl;
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

int main(int, char* [])
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

#ifdef TEST62

#include <vtkActor.h>
#include <vtkAreaPicker.h>
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


#if VTK_VERSION_NUMBER >= 89000000000ULL
#define VTK890 1
#endif

namespace {
    // Define interaction style
    class InteractorStyle : public vtkInteractorStyleRubberBandPick
    {
    public:
        static InteractorStyle* New();
        vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBandPick);

        InteractorStyle()
        {
            this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            this->SelectedActor = vtkSmartPointer<vtkActor>::New();
            this->SelectedActor->SetMapper(SelectedMapper);
        }

        virtual void OnLeftButtonUp() override
        {
            vtkNew<vtkNamedColors> colors;

            // Forward events
            vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

            vtkPlanes* frustum =
                static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())
                ->GetFrustum();

            vtkNew<vtkExtractGeometry> extractGeometry;
            extractGeometry->SetImplicitFunction(frustum);
            extractGeometry->SetInputData(this->Points);
            extractGeometry->Update();

            vtkNew<vtkVertexGlyphFilter> glyphFilter;
            glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
            glyphFilter->Update();

            vtkPolyData* selected = glyphFilter->GetOutput();
            std::cout << "Selected " << selected->GetNumberOfPoints() << " points."
                << std::endl;
            std::cout << "Selected " << selected->GetNumberOfCells() << " cells."
                << std::endl;
            this->SelectedMapper->SetInputData(selected);
            this->SelectedMapper->ScalarVisibilityOff();

            vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(
                selected->GetPointData()->GetArray("OriginalIds"));
            for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
            {
                std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
            }

            this->SelectedActor->GetProperty()->SetColor(
                colors->GetColor3d("Red").GetData());
            this->SelectedActor->GetProperty()->SetPointSize(5);

            this->CurrentRenderer->AddActor(SelectedActor);
            this->GetInteractor()->GetRenderWindow()->Render();
            this->HighlightProp(NULL);
        }

        void SetPoints(vtkSmartPointer<vtkPolyData> points)
        {
            this->Points = points;
        }

    private:
        vtkSmartPointer<vtkPolyData> Points;
        vtkSmartPointer<vtkActor> SelectedActor;
        vtkSmartPointer<vtkDataSetMapper> SelectedMapper;
    };

    vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(20);
    pointSource->Update();

    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputConnection(pointSource->GetOutputPort());
#if VTK890
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
#else
    idFilter->SetIdsArrayName("OriginalIds");
#endif
    idFilter->Update();

    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    vtkPolyData* input = surfaceFilter->GetOutput();

    // Create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(input);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(3);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("HighlightSelectedPoints");

    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    renderWindow->Render();

    vtkNew<InteractorStyle> style;
    style->SetPoints(input);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST62

#ifdef TEST63

#define vtk_official

#ifdef vtk_official
//https://kitware.github.io/vtk-examples/site/Cxx/Qt/ImageDataToQImage/
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <QApplication>
#include <QColor>
#include <QImage>

// Create a green 50x50 imageData for demonstration purposes
vtkSmartPointer<vtkImageData> createDemoImageData()
{
    vtkNew<vtkImageData> image;
    image->SetDimensions(50, 50, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

    int width = image->GetDimensions()[0];
    int height = image->GetDimensions()[1];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char* pixel =
                static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            pixel[0] = 0;
            pixel[1] = 255;
            pixel[2] = 0;
        }
    }

    return image;
}

// The actual conversion code
QImage vtkImageDataToQImage(vtkSmartPointer<vtkImageData> imageData)
{
    if (!imageData)
    {
        return QImage();
    }

    /// \todo retrieve just the UpdateExtent
    int width = imageData->GetDimensions()[0];
    int height = imageData->GetDimensions()[1];
    QImage image(width, height, QImage::Format_RGB32);
    QRgb* rgbPtr = reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
    unsigned char* colorsPtr =
        reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

    // Loop over the vtkImageData contents.
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // Swap the vtkImageData RGB values with an equivalent QColor
            *(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
            colorsPtr += imageData->GetNumberOfScalarComponents();
        }

        rgbPtr -= width * 2;
    }

    return image;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QImage qimage = vtkImageDataToQImage(createDemoImageData());
    qimage.save("qimage.png");

    return EXIT_SUCCESS;
}

#else

//https://tieba.baidu.com/p/4297027534?red_tag=3118028977
#include <QImage>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkJPEGReader.h>

#include <string>
QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars);
QImage createQImage(vtkImageData* imageData);

int main()
{
    std::string inputFilename = "test.jpg";
    std::string outputFilename = "save.jpg";

    vtkNew<vtkJPEGReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update(); // 必须Update，不然GetOutput返回值为空

    auto image = createQImage(reader->GetOutput());
    image.save(outputFilename.c_str());

    return 0;
}

QImage createQImage1(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex = 0;


    QRgb* qImageBits = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);


    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++);


            QRgb color = qRgba(tuple[0], tuple[0], tuple[0], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }


    return qImage;
}


QImage createQImage2(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex = 0;


    QRgb* qImageBits = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);


    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 2);


            QRgb color = qRgba(tuple[0], tuple[0], tuple[0], tuple[1]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }


    return qImage;
}


QImage createQImage3(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex = 0;


    QRgb* qImageBits = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);


    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 3);


            QRgb color = qRgba(tuple[0], tuple[1], tuple[2], 255);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }

    // Qt图片和VTK图片的原点不同，qt是左上角，vtk是左下角
    return qImage.mirrored(false, true);
}

QImage createQImage4(int width, int height, vtkUnsignedCharArray* scalars)
{
    QImage qImage(width, height, QImage::Format_ARGB32);
    vtkIdType tupleIndex = 0;
    int qImageBitIndex = 0;


    QRgb* qImageBits = (QRgb*)qImage.bits();
    unsigned char* scalarTuples = scalars->GetPointer(0);


    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            unsigned char* tuple = scalarTuples + (tupleIndex++ * 4);


            QRgb color = qRgba(tuple[0], tuple[1], tuple[2], tuple[3]);
            *(qImageBits + (qImageBitIndex++)) = color;
        }
    }


    return qImage;
}


QImage createQImage(vtkImageData* imageData)
{
    if (!imageData)
        return QImage();


    int width = imageData->GetDimensions()[0];
    int height = imageData->GetDimensions()[1];
    vtkUnsignedCharArray* scalars = vtkUnsignedCharArray::SafeDownCast(imageData->GetPointData()->GetScalars());


    if (!width || !height || !scalars)
        return QImage();


    switch (scalars->GetNumberOfComponents())
    {
    case 1:
        return createQImage1(width, height, scalars);
    case 2:
        return createQImage2(width, height, scalars);
    case 3:
        return createQImage3(width, height, scalars);
    case 4:
        return createQImage4(width, height, scalars);
    }


    return QImage();
}
#endif // vtk_official

#endif // TEST63

#ifdef TEST64

#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageReslice.h"
#include "vtkInteractorStyleImage.h"
#include "vtkImageMapToColors.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageReader.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkLookupTable.h"
#include "vtkOutlineFilter.h"
#include "vtkDICOMImageReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkPlaneSource.h"
#include "vtkPlane.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkBiDimensionalWidget.h"
#include "vtkAutoInit.h"
#include "vtkAxesActor.h"
#include "vtkTransform.h"
#include "vtkTextActor.h"
#include "vtkProperty2D.h"
#include "vtkResliceCursorThickLineRepresentation.h"

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
            vtkResliceCursor* rc = rep->GetResliceCursorActor()->GetCursorAlgorithm()->GetResliceCursor();
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
    vtkResliceCursorCallback() {}
    vtkImagePlaneWidget* IPW[3];
    vtkResliceCursorWidget* RCW[3];
};

int main()
{
    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName("file\\dicom_testdata");
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
        color[i] = 1;
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

    vtkSmartPointer<vtkResliceCursorCallback> cbk = vtkSmartPointer<vtkResliceCursorCallback>::New();
    vtkSmartPointer< vtkResliceCursor > resliceCursor = vtkSmartPointer< vtkResliceCursor >::New();
    resliceCursor->SetCenter(reader->GetOutput()->GetCenter());
    resliceCursor->SetThickMode(0);
    resliceCursor->SetThickness(10, 10, 10);
    resliceCursor->SetImage(reader->GetOutput());

    vtkSmartPointer< vtkResliceCursorWidget > resliceCursorWidget[3];
    vtkSmartPointer< vtkResliceCursorThickLineRepresentation > resliceCursorRep[3];

    double viewUp[3][3] = { { 0, 0, -1 }, { 0, 0, 1 }, { 0, 1, 0 } };
    for (int i = 0; i < 3; i++)
    {
        resliceCursorWidget[i] = vtkSmartPointer< vtkResliceCursorWidget >::New();
        resliceCursorWidget[i]->SetInteractor(iren);

        resliceCursorRep[i] = vtkSmartPointer< vtkResliceCursorThickLineRepresentation >::New();
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
        camPos[i] = 1;
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
    //ren[3]->AddActor(outlineActor);
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

    vtkSmartPointer< vtkInteractorStyleImage > style = vtkSmartPointer< vtkInteractorStyleImage >::New();
    iren->SetInteractorStyle(style);
    iren->Initialize();
    iren->Start();
    return EXIT_SUCCESS;
}
#endif // TEST64

#ifdef TEST65

#include <vtkActor.h>
#include <vtkBorderRepresentation.h>
#include <vtkBorderWidget.h>
#include <vtkCommand.h>
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
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>

namespace {
    class vtkCustomBorderWidget : public vtkBorderWidget
    {
    public:
        static vtkCustomBorderWidget* New();
        vtkTypeMacro(vtkCustomBorderWidget, vtkBorderWidget);

        //static void SelectAction(vtkAbstractWidget*);
        //static void TranslateAction(vtkAbstractWidget*);
        //static void EndSelectAction(vtkAbstractWidget*);
        static void MoveAction(vtkAbstractWidget*);
        //static void HoverLeaveAction(vtkAbstractWidget*);

        static void EndSelectAction(vtkAbstractWidget* w);

        vtkCustomBorderWidget();
    };

    vtkStandardNewMacro(vtkCustomBorderWidget);

} // namespace

//void SelectionChangedCallbackFunction(vtkObject* caller,
//    long unsigned int eventId,
//    void* clientData, void* callData) {}

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    // Sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(4.0);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(
        colors->GetColor3d("DarkOliveGreen").GetData());

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BorderWidget");

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkCustomBorderWidget> borderWidget;
    borderWidget->SetInteractor(renderWindowInteractor);
    borderWidget->CreateDefaultRepresentation();
    borderWidget->SelectableOff();

    // Add the actors to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    // Render an image (lights and cameras are created automatically)
    renderWindowInteractor->Initialize();
    renderWindow->Render();
    borderWidget->On();

    //vtkNew<vtkCallbackCommand> selectionChangedCallback;
    //selectionChangedCallback->SetCallback(SelectionChangedCallbackFunction);
    //borderWidget->AddObserver(vtkWidgetEvent::Move, selectionChangedCallback);

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {
    vtkCustomBorderWidget::vtkCustomBorderWidget()
    {
        // 在此处设置了回调函数，才可以调用到action
        this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent, vtkWidgetEvent::EndSelect, this, vtkCustomBorderWidget::EndSelectAction);
        this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent, vtkWidgetEvent::Move, this, vtkCustomBorderWidget::MoveAction);
    }

    void vtkCustomBorderWidget::EndSelectAction(vtkAbstractWidget* w)
    {
        vtkBorderWidget* borderWidget = dynamic_cast<vtkBorderWidget*>(w);

        // Get the actual box coordinates/planes
        // vtkNew<vtkPolyData> polydata;

        // Get the bottom left corner
        //auto lowerLeft =
        //    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
        //    ->GetPosition();
        //std::cout << "Lower left: " << lowerLeft[0] << " " << lowerLeft[1]
        //    << std::endl;

        //auto upperRight =
        //    static_cast<vtkBorderRepresentation*>(borderWidget->GetRepresentation())
        //    ->GetPosition2();
        //std::cout << "Upper right: " << upperRight[0] << " " << upperRight[1]
        //    << std::endl;

        vtkBorderWidget::EndSelectAction(w);
    }

    void vtkCustomBorderWidget::MoveAction(vtkAbstractWidget* w)
    {
        std::cout << "test\n";
        vtkBorderWidget::MoveAction(w);
    }

} // namespace```

#endif // TEST65

#ifdef TEST66

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkProperty.h>

#include <iostream>
#include <array>

namespace
{
    std::array<float, 4 * 3> vertices{
        0,0,0,
        1,0,0,
        1,1,0,
        0,1,0
    };

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
        vtkSmartPointer<vtkActor> m_actor{ nullptr };
    };

    vtkStandardNewMacro(MyCommand);
}

int main(int, char* [])
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    cells->InsertNextCell({ 0,1,2,3 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(polyData);

    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);

    //renderer
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

#ifdef TEST67

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace {
    class vtkMyCallback : public vtkCommand
    {
    public:
        static vtkMyCallback* New()
        {
            return new vtkMyCallback;
        }
        virtual void Execute(vtkObject* caller, unsigned long, void*)
        {
            // Here we use the vtkBoxWidget to transform the underlying coneActor
            // (by manipulating its transformation matrix).
            vtkNew<vtkTransform> t;
            vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
            widget->GetTransform(t);
            widget->GetProp3D()->SetUserTransform(t);
        }
    };
} // namespace

int main(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkConeSource> cone;

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("BurlyWood").GetData());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(coneActor);
    renderer->SetBackground(colors->GetColor3d("Blue").GetData());

    vtkNew<vtkRenderWindow> window;
    window->AddRenderer(renderer);
    window->SetSize(300, 300);
    window->SetWindowName("BoxWidget");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(window);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    interactor->SetInteractorStyle(style);

    vtkNew<vtkBoxWidget> boxWidget;
    boxWidget->SetInteractor(interactor);

    boxWidget->SetProp3D(coneActor);
    boxWidget->SetPlaceFactor(1.25); // Make the box 1.25x larger than the actor
    boxWidget->PlaceWidget();

    vtkNew<vtkMyCallback> callback;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    boxWidget->On();

    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}


#endif // TEST67

