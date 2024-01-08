/**
 * 001. 自定义 Filter 继承自 vtkPolyDataAlgorithm
 * 002. 从经过 Filter 变换后的数据获取原始数据
 *
 * 101. vtkProbeFilter 探针的基础使用，探针某些时候插值的结果并不准确
 * 102. vtkGlyph3D vtkProbeFilter vtkSampleFunction vtkThreshold 使用隐函数随机采样
 * 103. vtkPointInterpolator 插值，类似探针，比探针结果更准确
 *
 * 201. vtkWarpScalar vtkWarpVector 根据标量或向量值在指定方向对顶点进行偏移
 * 202. vtkShrinkFilter 收缩单元格 vtkShrinkPolyData
 * 203. vtkElevationFilter 沿指定方向生成Scalars，可以将输入数据的标量值作为高程，也可以设置变换函数定义高程变化
 * 204. vtkSimpleElevationFilter 用于简单的高程变换
 *
 * 301. vtkAppendPolyData 合并多个 vtkPolyData
 * 302. 合并带有Scalars、Vectors的vtkPolyData
 * 303. 使用不同方式设置Scalars数据时，合并后的polyData没有Scalars数据
 * 304. vtkMergeFilter
 * 305. vtkAppendFilter
 *
 * 401. vtkCellDataToPointData 单元数据转顶点数据
 * 402. vtkPointDataToCellData 顶点数据转单元数据
 * 403. vtkCellCenters 获取单元中心即格心，并用球体标注格心
 *
 * 501. vtkExtractGeometry vtkExtractPolyDataGeometry 提取被视锥体包围(或没有包围)的单元格
 * 502. vtkClipPolyData 一般用于裁剪输入平面的一侧，保留另一侧
 * 503. vtkExtractCells 提取指定单元，类似设置间隔
 * 504. vtkSelectPolyData 选择多边形网格的一部分，生成选择标量
 * 505. vtkExtractUnstructuredGrid 提取非结构化网格的子集
 * 506. vtkRectilinearGridGeometryFilter 从直线网格中提取几何体（点、线、面、体）
 * 507. vtkPolyDataSilhouette 提取观察者视角下物体的轮廓
 * 508. vtkFeatureEdges 提取网格(多边形数据)的特征边
 * 509. vtkExtractEdges 提取构成网格所有单元边界
 * 510. vtkOutlineFilter 生成一个输入网格的包围盒
 *
 * 601. vtkDataSetSurfaceFilter 将 vtkUnstructuredGrid 转换为 vtkPolyData，还可以将体网格转换为表面数据，从而简化模型
 * 602. vtkGeometryFilter 从数据集中提取边界（3D单元提取2D面），可以将任何数据转换为多边形数据，类似 vtkDataSetSurfaceFilter
 * 603. vtkDataSetToDataObjectFilter 将数据集(vtkDataSet)转换为数据对象(vtkDataObject)
 * 604. vtkDataObjectToDataSetFilter 将数据对象(vtkDataObject)转换为数据集(vtkDataSet) vtkFieldDataToAttributeDataFilter
 * 605. 序列化反序列化vtk数据
 *
 * 701.vtkLODProp3D 对于绘制大型网格可以提高渲染效率
 * 702.decimation多边形削减、网格简化 vtkDecimate vtkDecimatePro vtkQuadricClustering vtkQuardricDecimation
 * 703.平滑过滤器 vtkWindowedSincPolyDataFilter vtkSmoothPolyDataFilter
 * 704.vtkCleanPolyData
 *
 * 801. vtkSampleFunction生成 vtkImageData 求等值面（几何数据）
 * 802. vtkSampleFunction生成 vtkImageData 转几何数据
 * 803. PerlinNoise 使用 vtkSampleFunction 生成噪声图片
 */

#define TEST511

#ifdef TEST001

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTubeFilter.h>

class MyFilter : public vtkPolyDataAlgorithm
{
public:
    vtkTypeMacro(MyFilter, vtkPolyDataAlgorithm);

    static MyFilter* New();

protected:
    MyFilter()           = default;
    ~MyFilter() override = default;

    // int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override
    // {
    // }

    int RequestData(vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector) override
    {
        // 获取输入数据
        vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
        vtkPolyData* inputData = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

        // 创建输出数据
        vtkInformation* outInfo = outputVector->GetInformationObject(0);
        vtkPolyData* outputData = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

        // 对输入的数据进行处理，并设置到输出数据中
        outputData->DeepCopy(inputData);

        return 1;
    }

    // int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override
    // {
    // }

private:
    MyFilter(const MyFilter&)       = delete;
    void operator=(const MyFilter&) = delete;
};

vtkStandardNewMacro(MyFilter);

int main()
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(2.0, 1.0, 0.0);
    points->InsertNextPoint(3.0, 1.0, 0.0);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    vtkNew<MyFilter> filter;
    filter->SetInputData(polyData);
    filter->Update();

    //------------------------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(filter->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    //------------------------------------------------------------------------------
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

#endif // TEST001

#ifdef TEST002

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph2D.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>

#include <vtkAlgorithmOutput.h>

#include <iostream>

int main(int, char*[])
{
    // 顶点数据
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(0.0, 1.0, 0.0);

    // 每一个顶点的标量
    vtkNew<vtkDoubleArray> pointScalars;
    pointScalars->InsertNextValue(1.);
    pointScalars->InsertNextValue(4.);
    pointScalars->InsertNextValue(3.);
    pointScalars->InsertNextValue(2.);

    // 每一个顶点的向量
    vtkNew<vtkDoubleArray> pointVectors;
    pointVectors->SetNumberOfComponents(3); // 3维向量 {x,y,z}
    pointVectors->InsertNextTuple3(5.0, 0.0, 0.0);
    pointVectors->InsertNextTuple3(0.0, 3.0, 0.0);
    pointVectors->InsertNextTuple3(1.0, 1.0, 1.0);
    pointVectors->InsertNextTuple3(-1.0, -1.0, 0.0);

    vtkNew<vtkPolyData> polydata;
    polydata->SetPoints(points);
    polydata->GetPointData()->SetScalars(pointScalars);
    polydata->GetPointData()->SetVectors(pointVectors);

    //--------------------------------------------------------------------------------------------
    // 矢量图的图案
    vtkNew<vtkArrowSource> source;
    source->Update();

    // 颜色映射表
    vtkNew<vtkLookupTable> colorTable;
    colorTable->SetHueRange(0.67, 0.0);
    colorTable->SetRange(1, 4);
    colorTable->Build();

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(polydata);             // 顶点数据，即矢量图每一个图案的位置
    glyph->SetSourceData(source->GetOutput()); // 资源数据，即矢量图的图案
    glyph->SetColorModeToColorByScalar();      // 颜色由标量决定
    glyph->SetScaleModeToScaleByScalar();      // 大小由标量决定
    glyph->SetVectorModeToUseVector();         // 方向由向量决定

    //--------------------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetLookupTable(colorTable);
    mapper->SetScalarRange(colorTable->GetRange());
    mapper->SetInputConnection(glyph->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //--------------------------------------------------------------------------------------
    // 从经过 vtkAlgorithm 变换后的数据获取源数据（注意区分使用的是独立数据集还是管道连接方式）
    if (vtkSmartPointer<vtkAlgorithm> algorithm = actor->GetMapper()->GetInputConnection(0, 0)->GetProducer())
    {
        // 如果没有经过algorithm变换，reference为nullptr
        if (auto reference = vtkGlyph3D::SafeDownCast(algorithm.GetPointer()))
        {
            if (auto originSource = vtkPolyData::SafeDownCast(reference->GetInput()))
            {
                auto originPointNum = originSource->GetNumberOfPoints(); // 源数据顶点个数
                auto originCellNum  = originSource->GetNumberOfCells();  // 源数据单元个数

                auto _scalars = originSource->GetPointData()->GetScalars();
                auto _vectors = originSource->GetPointData()->GetVectors();

                for (vtkIdType i = 0; i < originPointNum; i++)
                {
                    double pt[3] { 0 };
                    originSource->GetPoint(i, pt);

                    auto scalar = _scalars->GetVariantValue(i).ToDouble();
                    auto vector = _vectors->GetTuple3(i);

                    // 原始数据的顶点、标量、向量数据
                    std::cout << "point( " << pt[0] << ", " << pt[1] << ", " << pt[2] << " )\tScalar: " << scalar << "\tVector: " << vector[0] << ", "
                              << vector[1] << ", " << vector[2] << '\n';
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->SetWindowName("vector");
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST002

#ifdef TEST101

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProbeFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (long long i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i + 0, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 标量值
    vtkNew<vtkDoubleArray> scalars;
    scalars->SetName("Scalars");
    for (int i = 0; i < cells->GetNumberOfCells(); i++)
    {
        scalars->InsertNextValue(i);
    }

    vtkNew<vtkPolyData> poly;
    poly->SetPoints(points);
    poly->SetPolys(cells);
    poly->GetCellData()->AddArray(scalars); // 原始数据必须使用AddArray添加标量或矢量数据
    poly->GetCellData()->SetActiveScalars("Scalars");

    //--------------------------------------------------------------------------
    // 探针
    vtkNew<vtkPoints> probePoints;
    probePoints->InsertNextPoint(0.5, 1.5, 0);
    probePoints->InsertNextPoint(2.5, 1.5, 0);
    probePoints->InsertNextPoint(4.5, 1.5, 0);
    probePoints->InsertNextPoint(6.5, 1.5, 0);
    probePoints->InsertNextPoint(8.5, 1.5, 0);

    vtkNew<vtkPolyData> probePoly;
    probePoly->SetPoints(probePoints);

    vtkNew<vtkProbeFilter> probeFilter;
    probeFilter->SetInputData(probePoly); // 需要探测的点
    probeFilter->SetSourceData(poly);     // 需要探测的原始数据
    probeFilter->Update();

    auto out = probeFilter->GetOutput();
    out->Print(std::cout);

    // 获取到的是探测的5个点（probePoints）在原始数据中插值得到的标量值
    auto probeScalars = out->GetPointData()->GetArray("Scalars");
    std::cout << probeScalars->GetRange()[0] << '\t' << probeScalars->GetRange()[1] << '\n';

    //--------------------------------------------------------------------------
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->Build();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(poly);
    mapper->SetScalarRange(scalars->GetRange());
    mapper->SetLookupTable(hueLut);
    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST101

#ifdef TEST102

#include <vtkActor.h>
#include <vtkCone.h>
#include <vtkDataSetMapper.h>
#include <vtkGlyph3D.h>
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

/*
 * https://kitware.github.io/vtk-examples/site/Cxx/Visualization/RandomProbe/
 *
 * vtkThreshold 用于对数据集进行阈值处理，检测数据集的某个特定区域
 */

int main(int argc, char* argv[])
{
    int resolution = 50;

    // 用来采样的圆锥体，注意不是被采样的几何体
    vtkNew<vtkCone> implicitCone;
    implicitCone->SetAngle(30.0);

    double radius = 1.;
    double xMin   = -radius * 2.0;
    double xMax   = radius * 2.0;
    vtkNew<vtkSampleFunction> sampledCone;
    sampledCone->SetSampleDimensions(resolution, resolution, resolution);
    sampledCone->SetModelBounds(xMin, xMax, xMin, xMax, xMin, xMax);
    sampledCone->SetImplicitFunction(implicitCone);

    vtkNew<vtkThreshold> thresholdCone;
    thresholdCone->SetInputConnection(sampledCone->GetOutputPort());
    thresholdCone->SetLowerThreshold(0);
    thresholdCone->SetThresholdFunction(vtkThreshold::THRESHOLD_LOWER);
    thresholdCone->Update();

    // 被采样的几何体
    vtkNew<vtkPointSource> randomPoints;
    randomPoints->SetCenter(0.0, 0.0, 0.0);
    randomPoints->SetNumberOfPoints(10000);
    randomPoints->SetDistributionToUniform();
    randomPoints->SetRadius(xMax);
    randomPoints->Update();

    // 用随机点（圆锥体）探测圆锥体数据集（一大堆顶点）
    vtkNew<vtkProbeFilter> randomProbe;
    randomProbe->SetInputConnection(0, randomPoints->GetOutputPort());
    randomProbe->SetInputConnection(1, thresholdCone->GetOutputPort());
    randomProbe->Update();
    randomProbe->GetOutput()->GetPointData()->SetActiveScalars("vtkValidPointMask");

    vtkNew<vtkThresholdPoints> selectPoints;
    selectPoints->SetInputConnection(randomProbe->GetOutputPort());
    selectPoints->ThresholdByUpper(1.0);

    //------------------------------------------------------
    // 将采样得到的点可视化
    vtkNew<vtkSphereSource> sphere;
    sphere->SetRadius(0.025);

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetSourceConnection(sphere->GetOutputPort());
    glyph->SetInputConnection(selectPoints->GetOutputPort());

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(glyph->GetOutputPort());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("RandomProbe");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST102

#ifdef TEST103

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkDoubleArray.h>
#include <vtkGaussianKernel.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLinearKernel.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPointInterpolator.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (size_t i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (long long i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i + 0, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 标量值
    vtkNew<vtkDoubleArray> scalars;
    scalars->SetName("Scalars"); // 名称必须设置
    for (int i = 0; i < cells->GetNumberOfCells(); i++)
    {
        scalars->InsertNextValue(i);
    }

    vtkNew<vtkPolyData> poly;
    poly->SetPoints(points);
    poly->SetPolys(cells);
    poly->GetCellData()->AddArray(scalars); // 原始数据必须使用 AddArray 添加Scalars/Vectors
    poly->GetCellData()->SetActiveScalars("Scalars");

    vtkNew<vtkCellDataToPointData> c2p;
    c2p->SetInputData(poly);
    c2p->Update();

    //--------------------------------------------------------------------------
    // 需要插值的点
    vtkNew<vtkPoints> filterPoints;
    filterPoints->InsertNextPoint(1., 1., 0);
    filterPoints->InsertNextPoint(2., 1., 0);
    filterPoints->InsertNextPoint(3., 1., 0);
    filterPoints->InsertNextPoint(4., 1., 0);
    filterPoints->InsertNextPoint(5., 1., 0);
    filterPoints->InsertNextPoint(6., 1., 0);
    filterPoints->InsertNextPoint(7., 1., 0);
    filterPoints->InsertNextPoint(8., 1., 0);

    vtkNew<vtkPolyData> seeds;
    seeds->SetPoints(filterPoints);

    // 高斯核
    vtkNew<vtkGaussianKernel> gaussianKernel;
    gaussianKernel->SetSharpness(2.0);
    gaussianKernel->SetRadius(2.0);

    // 线性插值核
    vtkNew<vtkLinearKernel> linearKernel;
    linearKernel->SetKernelFootprintToNClosest(); // 使用N个最接近的点插值
    linearKernel->SetNumberOfPoints(4);           // 最接近点的个数

    vtkNew<vtkPointInterpolator> filter;
    filter->SetInputData(seeds);
    filter->SetSourceData(c2p->GetOutput()); // 必须使用pointData的Scalars/Vectors
    filter->SetKernel(linearKernel);         // 设置插值内核，默认是vtkLinearKernel
    filter->Update();

    auto out = filter->GetOutput();
    // out->Print(std::cout);

    double range[2] {};
    auto filterScalars = out->GetPointData()->GetArray("Scalars");
    filterScalars->GetRange(range);
    std::cout << "Range: " << range[0] << '\t' << range[1] << '\n';

    // 插值得到的标量值
    for (vtkIdType i = 0; i < filterScalars->GetNumberOfValues(); ++i)
    {
        std::cout << i << '\t' << filterScalars->GetVariantValue(i).ToDouble() << '\n';
    }

    //--------------------------------------------------------------------------
    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->Build();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(poly);
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(hueLut);
    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST103

#ifdef TEST201

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWarpScalar.h>

#include <iostream>

// vtkWarpScalar vtkWarpVector

int main(int, char*[])
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(1, 0, 0);
    points->InsertNextPoint(1, 1, 0);
    points->InsertNextPoint(0, 1, 0);

    vtkNew<vtkDoubleArray> scalars;
    scalars->InsertNextValue(1);
    scalars->InsertNextValue(2);
    scalars->InsertNextValue(2);
    scalars->InsertNextValue(3);

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetScalars(scalars);

    vtkNew<vtkDataSetMapper> gridMapper;
    gridMapper->SetInputData(polyData);
    gridMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> gridActor;
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->SetColor(1, 0, 0);

    //--------------------------------------------------------------------------------
    // 根据标量值将每个顶点在法线方向(没有法线则使用默认法线，垂直于表面)上偏移一定距离，距离由Factor和标量值决定
    vtkNew<vtkWarpScalar> warpScalar;
    warpScalar->SetInputData(polyData);
    warpScalar->SetScaleFactor(0.1);
    warpScalar->Update();

    auto scalarPolyData = warpScalar->GetOutput();
    vtkNew<vtkDataSetMapper> scalarMapper;
    scalarMapper->SetInputData(scalarPolyData);
    scalarMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> scalarActor;
    scalarActor->SetMapper(scalarMapper);
    scalarActor->GetProperty()->SetColor(0, 1, 0);

    //----------------------------------------------------------------------
    std::cout << "Origin Points:\n";
    for (vtkIdType i = 0; i < polyData->GetNumberOfPoints(); ++i)
    {
        auto pPoint = polyData->GetPoint(i);
        std::cout << pPoint[0] << '\t' << pPoint[1] << '\t' << pPoint[2] << '\n';
    }
    std::cout << "Wrap Scalar Points:\n";
    for (vtkIdType i = 0; i < scalarPolyData->GetNumberOfPoints(); ++i)
    {
        auto pPoint = scalarPolyData->GetPoint(i);
        std::cout << pPoint[0] << '\t' << pPoint[1] << '\t' << pPoint[2] << '\n';
    }

    //----------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(scalarActor);
    renderer->AddActor(gridActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST201

#ifdef TEST202

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkShrinkFilter.h>
#include <vtkSphereSource.h>

// 会有z冲突现象

int main(int, char*[])
{
    // vtkNew<vtkSphereSource> source;
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);
    actor->GetProperty()->EdgeVisibilityOn();

    // 将单元格收缩到其质心（单元的质心），通过收缩因子控制收缩多少
    // 新生成的几何数据，原来连接起来的单元将会断开
    vtkNew<vtkShrinkFilter> shrink;
    shrink->SetInputConnection(source->GetOutputPort());
    shrink->SetShrinkFactor(.5); // 收缩因子，越小则生成的单元越小

    vtkNew<vtkDataSetMapper> mapper2;
    mapper2->SetInputConnection(shrink->GetOutputPort());

    vtkNew<vtkProperty> back;
    back->SetColor(1, 0, 0);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(0, 1, 0);

    //-------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TESt202

#ifdef TEST203

#include <vtkActor.h>
#include <vtkElevationFilter.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSuperEllipsoid.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProjectSphereFilter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

// https://kitware.github.io/vtk-examples/site/Cxx/Visualization/ProjectSphere/

int main(int, char*[])
{
    // 超椭球体
    vtkNew<vtkParametricSuperEllipsoid> surface;
    surface->SetN1(2.0);
    surface->SetN2(0.5);

    vtkNew<vtkParametricFunctionSource> source;
    source->SetParametricFunction(surface);
    source->Update();

    // 高程图，用于生成标量值，将2维数据转换为三维
    vtkNew<vtkElevationFilter> elevationFilter;
    elevationFilter->SetInputConnection(source->GetOutputPort());
    elevationFilter->SetLowPoint(0.0, 0.0, -1.);
    elevationFilter->SetHighPoint(0.0, 0.0, 1.);
    elevationFilter->Update();

    vtkNew<vtkPolyData> pd1;
    pd1->DeepCopy(elevationFilter->GetOutput());

    // 展开球体
    vtkNew<vtkProjectSphereFilter> sphereProject1;
    sphereProject1->SetInputConnection(elevationFilter->GetOutputPort());
    sphereProject1->Update();

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputConnection(sphereProject1->GetOutputPort());
    mapper1->SetScalarRange(sphereProject1->GetOutput()->GetPointData()->GetScalars()->GetRange());
    vtkNew<vtkActor> actor1;
    actor1->SetMapper(mapper1);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputData(pd1);
    mapper2->SetScalarRange(pd1->GetPointData()->GetScalars()->GetRange());
    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    std::cout << "left range\t" << pd1->GetPointData()->GetScalars()->GetRange()[0] << '\t' << pd1->GetPointData()->GetScalars()->GetRange()[1]
              << "\nright range\t" << sphereProject1->GetOutput()->GetPointData()->GetScalars()->GetRange()[0] << '\t'
              << sphereProject1->GetOutput()->GetPointData()->GetScalars()->GetRange()[1] << '\n';

    double leftViewport[4]  = { 0.0, 0.0, 0.5, 1.0 };
    double rightViewport[4] = { 0.5, 0.0, 1.0, 1.0 };

    vtkNew<vtkRenderer> rightRenderer;
    rightRenderer->SetViewport(rightViewport);
    rightRenderer->SetBackground(.3, .2, .1);
    rightRenderer->AddActor(actor1);

    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->SetViewport(leftViewport);
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->AddActor(actor2);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->AddRenderer(rightRenderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST203

#ifdef TEST204

#include <vtkActor.h>
#include <vtkDelaunay2D.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSimpleElevationFilter.h>

int main(int, char*[])
{
    vtkNew<vtkPoints> points;

    unsigned int GridSize = 10;
    for (unsigned int x = 0; x < GridSize; x++)
    {
        for (unsigned int y = 0; y < GridSize; y++)
        {
            points->InsertNextPoint(x, y, (x + y) / (y + 1));
            std::cout << x << " " << y << " " << (x + y) / (y + 1) << std::endl;
        }
    }

    double bounds[6];
    points->GetBounds(bounds);

    vtkNew<vtkPolyData> inputPolyData;
    inputPolyData->SetPoints(points);

    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(inputPolyData);
    delaunay->Update();

    vtkNew<vtkSimpleElevationFilter> elevationFilter;
    elevationFilter->SetInputConnection(delaunay->GetOutputPort());
    elevationFilter->SetVector(0.0, 0.0, 1);
    elevationFilter->Update();

    vtkNew<vtkPolyData> output;
    output->ShallowCopy(dynamic_cast<vtkPolyData*>(elevationFilter->GetOutput()));

    vtkFloatArray* elevation = dynamic_cast<vtkFloatArray*>(output->GetPointData()->GetArray("Elevation"));

    vtkNew<vtkLookupTable> colorLookupTable;
    colorLookupTable->SetTableRange(bounds[4], bounds[5]);
    colorLookupTable->Build();

    vtkNew<vtkUnsignedCharArray> colors;
    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    for (vtkIdType i = 0; i < output->GetNumberOfPoints(); i++)
    {
        double val = elevation->GetValue(i);
        // std::cout << "val: " << val << std::endl;

        double dcolor[3];
        colorLookupTable->GetColor(val, dcolor);
        std::cout << "dcolor: " << dcolor[0] << " " << dcolor[1] << " " << dcolor[2] << std::endl;

        unsigned char color[3];
        for (unsigned int j = 0; j < 3; j++)
        {
            color[j] = 255 * dcolor[j] / 1.0;
        }
        std::cout << "color: " << (int)color[0] << " " << (int)color[1] << " " << (int)color[2] << std::endl;

        colors->InsertNextTypedTuple(color);
    }

    output->GetPointData()->AddArray(colors);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(output);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST204

#ifdef TEST301

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main()
{
    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(0, 0, 0);
    sphere->Update();
    vtkNew<vtkConeSource> cone;
    cone->SetCenter(1., 1., 1.);
    cone->Update();
    vtkNew<vtkCubeSource> cube;
    cube->SetCenter(2., 2., 2.);
    cube->Update();

    std::cout << "Sphere\t points: " << sphere->GetOutput()->GetNumberOfPoints() << "\tcells: " << sphere->GetOutput()->GetNumberOfCells() << '\n';
    std::cout << "Cone\t points: " << cone->GetOutput()->GetNumberOfPoints() << "\tcells: " << cone->GetOutput()->GetNumberOfCells() << '\n';
    std::cout << "Cube\t points: " << cube->GetOutput()->GetNumberOfPoints() << "\tcells: " << cube->GetOutput()->GetNumberOfCells() << '\n';

    // 将多个vtkPolyData合并为一个vtkPolyData
    vtkNew<vtkAppendPolyData> appendPolyData;
    appendPolyData->AddInputData(sphere->GetOutput());
    appendPolyData->AddInputData(cone->GetOutput());
    appendPolyData->AddInputData(cube->GetOutput());
    appendPolyData->Update();

    // 新生成的vtkPolyData的顶点、单元个数是原来的所有vtkPolyData的顶点、单元的个数之和
    std::cout << "Append\t points: " << appendPolyData->GetOutput()->GetNumberOfPoints()
              << "\tcells: " << appendPolyData->GetOutput()->GetNumberOfCells() << '\n';

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(appendPolyData->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    //------------------------------------------------------------------------------
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

    return EXIT_SUCCESS;
}

#endif // TEST301

#ifdef TEST302

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    //----------------------------------------------------------------------
    // PolyData0
    vtkNew<vtkPoints> points0;
    points0->InsertNextPoint(0.0, 0.0, 0.0);
    points0->InsertNextPoint(1.0, 0.0, 0.0);
    points0->InsertNextPoint(2.0, 1.0, 0.0);
    points0->InsertNextPoint(3.0, 1.0, 0.0);

    vtkNew<vtkCellArray> cells0;
    cells0->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars0;
    scalars0->SetName("Scalars");
    scalars0->InsertNextValue(1.0);
    scalars0->InsertNextValue(2.0);
    scalars0->InsertNextValue(3.0);
    scalars0->InsertNextValue(4.0);

    vtkNew<vtkPolyData> polyData0;
    polyData0->SetPoints(points0);
    polyData0->SetLines(cells0);
    polyData0->GetPointData()->AddArray(scalars0);
    polyData0->GetPointData()->SetActiveScalars("Scalars");

    //----------------------------------------------------------------------
    // PolyData1
    vtkNew<vtkPoints> points1;
    points1->InsertNextPoint(0.0, 0.0, 1.0);
    points1->InsertNextPoint(1.0, 0.0, 1.0);
    points1->InsertNextPoint(2.0, 1.0, 1.0);
    points1->InsertNextPoint(3.0, 1.0, 1.0);

    vtkNew<vtkCellArray> cells1;
    cells1->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars1;
    scalars1->SetName("Scalars");
    scalars1->InsertNextValue(4.0);
    scalars1->InsertNextValue(3.0);
    scalars1->InsertNextValue(2.0);
    scalars1->InsertNextValue(1.0);

    vtkNew<vtkPolyData> polyData1;
    polyData1->SetPoints(points1);
    polyData1->SetLines(cells1);
    polyData1->GetPointData()->AddArray(scalars1);
    polyData1->GetPointData()->SetActiveScalars("Scalars");

    //----------------------------------------------------------------------
    // PolyData2
    vtkNew<vtkPoints> points2;
    points2->InsertNextPoint(0.0, 0.0, 2.0);
    points2->InsertNextPoint(1.0, 0.0, 2.0);
    points2->InsertNextPoint(2.0, 1.0, 2.0);
    points2->InsertNextPoint(3.0, 1.0, 2.0);

    vtkNew<vtkCellArray> cells2;
    cells2->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars2;
    scalars2->SetName("Scalars");
    scalars2->InsertNextValue(4.0);
    scalars2->InsertNextValue(2.0);
    scalars2->InsertNextValue(2.0);
    scalars2->InsertNextValue(4.0);

    vtkNew<vtkPolyData> polyData2;
    polyData2->SetPoints(points2);
    polyData2->SetLines(cells2);
    polyData2->GetPointData()->AddArray(scalars2);
    polyData2->GetPointData()->SetActiveScalars("Scalars");

    //----------------------------------------------------------------------
    // 合并vtkPolyData
    vtkNew<vtkAppendPolyData> appendPolyData;
    appendPolyData->AddInputData(polyData0);
    appendPolyData->AddInputData(polyData1);
    appendPolyData->AddInputData(polyData2);
    appendPolyData->Update();

    vtkNew<vtkLookupTable> lut;
    lut->SetHueRange(0.67, 0.0);
    lut->SetRange(1, 4);
    lut->Build();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(appendPolyData->GetOutput());
    mapper->SetLookupTable(lut);
    mapper->ScalarVisibilityOn();
    mapper->SetScalarRange(1, 4);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //------------------------------------------------------------------------------
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

#endif // TEST302

#ifdef TEST303

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    //----------------------------------------------------------------------
    // PolyData0
    vtkNew<vtkPoints> points0;
    points0->InsertNextPoint(0.0, 0.0, 0.0);
    points0->InsertNextPoint(1.0, 0.0, 0.0);
    points0->InsertNextPoint(2.0, 1.0, 0.0);
    points0->InsertNextPoint(3.0, 1.0, 0.0);

    vtkNew<vtkCellArray> cells0;
    cells0->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars0;
    scalars0->SetName("Scalars");
    scalars0->InsertNextValue(1.0);
    scalars0->InsertNextValue(2.0);
    scalars0->InsertNextValue(3.0);
    scalars0->InsertNextValue(4.0);

    vtkNew<vtkPolyData> polyData0;
    polyData0->SetPoints(points0);
    polyData0->SetLines(cells0);
    polyData0->GetPointData()->AddArray(scalars0);
    polyData0->GetPointData()->SetActiveScalars("Scalars");
    std::cout << "polyData0\t Scalars:" << polyData0->GetPointData()->GetScalars()->GetNumberOfValues() << '\n';

    //----------------------------------------------------------------------
    // PolyData1
    vtkNew<vtkPoints> points1;
    points1->InsertNextPoint(0.0, 0.0, 1.0);
    points1->InsertNextPoint(1.0, 0.0, 1.0);
    points1->InsertNextPoint(2.0, 1.0, 1.0);
    points1->InsertNextPoint(3.0, 1.0, 1.0);

    vtkNew<vtkCellArray> cells1;
    cells1->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars1;
    scalars1->SetName("Scalars");
    scalars1->InsertNextValue(4.0);
    scalars1->InsertNextValue(3.0);
    scalars1->InsertNextValue(2.0);
    scalars1->InsertNextValue(1.0);

    vtkNew<vtkPolyData> polyData1;
    polyData1->SetPoints(points1);
    polyData1->SetLines(cells1);
    polyData1->GetPointData()->AddArray(scalars1);
    polyData1->GetPointData()->SetActiveScalars("Scalars");
    std::cout << "polyData1\t Scalars:" << polyData1->GetPointData()->GetScalars()->GetNumberOfValues() << '\n';

    //----------------------------------------------------------------------
    // PolyData2
    vtkNew<vtkPoints> points2;
    points2->InsertNextPoint(0.0, 0.0, 2.0);
    points2->InsertNextPoint(1.0, 0.0, 2.0);
    points2->InsertNextPoint(2.0, 1.0, 2.0);
    points2->InsertNextPoint(3.0, 1.0, 2.0);

    vtkNew<vtkCellArray> cells2;
    cells2->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkDoubleArray> scalars2;
    // scalars2->SetName("Scalars");
    scalars2->InsertNextValue(4.0);
    scalars2->InsertNextValue(2.0);
    scalars2->InsertNextValue(2.0);
    scalars2->InsertNextValue(4.0);

    vtkNew<vtkPolyData> polyData2;
    polyData2->SetPoints(points2);
    polyData2->SetLines(cells2);
    polyData2->GetPointData()->SetScalars(scalars2);
    // polyData2->GetPointData()->AddArray(scalars2);
    // polyData2->GetPointData()->SetActiveScalars("Scalars");
    std::cout << "polyData2\t Scalars:" << polyData2->GetPointData()->GetScalars()->GetNumberOfValues() << '\n';

    //----------------------------------------------------------------------
    // 合并vtkPolyData
    // polyData2使用SetScalars()设置Scalars数据，其他两个使用AddArray()设置Scalars数据
    // 只要不同的vtkPolyData的Scalars数据的名称相同就能合并，名称不同时才不能合并(SetName()的参数相同)
    // 和使用SetScalars还是AddArray没有关系
    vtkNew<vtkAppendPolyData> appendPolyData;
    appendPolyData->AddInputData(polyData0);
    appendPolyData->AddInputData(polyData1);
    appendPolyData->AddInputData(polyData2);
    appendPolyData->Update();

    if (auto scalars = appendPolyData->GetOutput()->GetPointData()->GetScalars())
    {
        std::cout << "Append\t Scalars:" << scalars->GetNumberOfValues() << '\n';
    }
    else
    {
        std::cout << "Append PolyData does not have Scalars data\n";
    }

    vtkNew<vtkLookupTable> lut;
    lut->SetHueRange(0.67, 0.0);
    lut->SetRange(1, 4);
    lut->Build();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(appendPolyData->GetOutput());
    mapper->SetLookupTable(lut);
    mapper->ScalarVisibilityOn();
    mapper->SetScalarRange(1, 4);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //------------------------------------------------------------------------------
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

#ifdef TEST401

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataSetMapper.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (int i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 每个单元的标量数据
    vtkNew<vtkFloatArray> scalars;
    for (vtkIdType i = 0; i < cells->GetNumberOfCells(); i++)
    {
        scalars->InsertNextValue(static_cast<float>(i));
    }

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetCellData()->SetScalars(scalars);

    // 单元标量数据转为顶点数据
    vtkNew<vtkCellDataToPointData> cellToPoint;
    cellToPoint->SetInputData(polyData);
    cellToPoint->PassCellDataOff(); // 关闭将原始数据的单元数据也拷贝到输出数据中
    cellToPoint->Update();

    auto c2pPolyData = cellToPoint->GetOutput();
    c2pPolyData->Print(std::cout);

    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->Build();

    double range[2] {};
    polyData->GetCellData()->GetScalars()->GetRange(range);

    double range2[2] {};
    c2pPolyData->GetPointData()->GetScalars()->GetRange(range2);

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(hueLut);
    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper2;
    mapper2->SetInputData(c2pPolyData);
    mapper2->SetScalarRange(range2);
    mapper2->SetLookupTable(hueLut);
    mapper2->ScalarVisibilityOn();

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    //-----------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->SetViewport(0., 0., .5, 1.);

    vtkNew<vtkRenderer> renderer2;
    renderer2->AddActor(actor2);
    renderer2->SetBackground(.1, .2, .3);
    renderer2->SetViewport(.5, 0., 1., 1.);

    //-----------------------------------------------------------------------------
    // 左边原始单元数据（格心）右边经过转换的顶点数据（格点）
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->AddRenderer(renderer2);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST401

#ifdef TEST402

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkPointDataToCellData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (int i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 每个顶点的标量数据
    vtkNew<vtkFloatArray> scalars;
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
        scalars->InsertNextValue(static_cast<float>(i));
    }

    // 每个顶点的矢量数据
    vtkNew<vtkFloatArray> vectors;
    vectors->SetNumberOfComponents(3);
    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++)
    {
        vectors->InsertNextTuple3(static_cast<float>(i), static_cast<float>(i), static_cast<float>(i));
    }

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetPointData()->SetScalars(scalars);
    polyData->GetPointData()->SetVectors(vectors);

    // 顶点数据转为单元数据
    vtkNew<vtkPointDataToCellData> point2Cell;
    point2Cell->SetInputData(polyData);
    point2Cell->PassPointDataOff();
    point2Cell->Update();

    auto p2cPolyData = point2Cell->GetOutput();
    p2cPolyData->Print(std::cout);

    vtkNew<vtkLookupTable> hueLut;
    hueLut->SetHueRange(0.67, 0.0);
    hueLut->Build();

    double range[2] {};
    polyData->GetPointData()->GetScalars()->GetRange(range);

    double range2[2] {};
    p2cPolyData->GetCellData()->GetScalars()->GetRange(range2);

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(hueLut);
    mapper->ScalarVisibilityOn();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper2;
    mapper2->SetInputData(p2cPolyData);
    mapper2->SetScalarRange(range2);
    mapper2->SetLookupTable(hueLut);
    mapper2->ScalarVisibilityOn();

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);

    //-----------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);
    renderer->SetViewport(0., 0., .5, 1.);

    vtkNew<vtkRenderer> renderer2;
    renderer2->AddActor(actor2);
    renderer2->SetBackground(.1, .2, .3);
    renderer2->SetViewport(.5, 0., 1., 1.);

    //-----------------------------------------------------------------------------
    // 右边经过转换的单元数据（格心）左边原始顶点数据（格点）
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->AddRenderer(renderer2);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST402

#ifdef TEST403

#include <vtkActor.h>
#include <vtkCellCenters.h>
#include <vtkCellData.h>
#include <vtkDataSetMapper.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    // 顶点
    vtkNew<vtkPoints> points;
    for (int i = 0; i < 10; i++)
    {
        points->InsertNextPoint(i, 0, 0);
        points->InsertNextPoint(i, 1, 0);
        points->InsertNextPoint(i, 2, 0);
        points->InsertNextPoint(i, 3, 0);
    }

    // 拓扑
    vtkNew<vtkCellArray> cells;
    for (vtkIdType i = 0; i < 33; i += 4)
    {
        cells->InsertNextCell({ i, i + 1, i + 5, i + 4 });
        cells->InsertNextCell({ i + 1, i + 2, i + 6, i + 5 });
        cells->InsertNextCell({ i + 2, i + 3, i + 7, i + 6 });
    }

    // 每个单元的标量数据
    vtkNew<vtkFloatArray> scalars;
    for (vtkIdType i = 0; i < cells->GetNumberOfCells(); i++)
    {
        scalars->InsertNextValue(static_cast<float>(i));
    }

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetPolys(cells);
    polyData->GetCellData()->SetScalars(scalars);
    std::cout << "Origin PolyData\t points: " << polyData->GetNumberOfPoints() << "\tcells: " << polyData->GetNumberOfCells() << '\n';

    vtkNew<vtkCellCenters> centers;
    centers->SetInputData(polyData);
    centers->Update();

    // 对于vtkCellCenters过滤器，最后的输出只存在顶点，不存在单元，原始数据有几个单元，就有几个顶点
    // 如果原始数据的单元有标量或向量数据，那么cellCenter也会有标量或向量数据
    auto cellCenter = centers->GetOutput();
    // cellCenter->Print(std::cout);
    std::cout << "Cell Center\t points: " << cellCenter->GetNumberOfPoints() << "\tcells: " << cellCenter->GetNumberOfCells() << '\n';
    std::cout << "Cell Center\tScalars: " << cellCenter->GetPointData()->GetScalars()->GetNumberOfValues() << '\n';

    // 使用球标记单元的中心
    vtkNew<vtkSphereSource> sphere;
    sphere->Update();

    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputData(cellCenter);
    glyph->SetSourceData(sphere->GetOutput());
    glyph->SetScaleFactor(0.3);
    glyph->SetScaleModeToDataScalingOff();
    glyph->Update();

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    //-----------------------------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper2;
    mapper2->SetInputData(glyph->GetOutput());
    mapper2->ScalarVisibilityOff();

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(0, 1, 0);

    //-----------------------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST403

#ifdef TEST501

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>

namespace {
class MyStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBand3D);

    virtual void OnLeftButtonUp() override
    {
        vtkInteractorStyleRubberBand3D::OnLeftButtonUp();

        if (this->CurrentRenderer)
        {
            auto x0 = this->StartPosition[0];
            auto y0 = this->StartPosition[1];
            auto x1 = this->EndPosition[0];
            auto y1 = this->EndPosition[1];

            std::cout << "-------------------------------------------\nstart\t" << x0 << '\t' << y0 << "\nend\t" << x1 << '\t' << y1 << '\n';

            vtkNew<vtkAreaPicker> picker;
            vtkNew<vtkDataSetMapper> mapper;
            if (0 != picker->AreaPick(x0, y0, x1, y1, this->CurrentRenderer))
            {
                auto planes = picker->GetFrustum();

                // 提取输入的polyData被视锥体包围或没有包围的单元格
                // 可能会提取到背面的单元，可以利用远近剔除背面的面
                vtkNew<vtkExtractGeometry> extract;
                extract->SetInputData(m_polyData);
                extract->SetImplicitFunction(planes);
                extract->ExtractInsideOn();        // 提取内部还是外部
                extract->ExtractBoundaryCellsOn(); // 是否提取没有被完全包围的单元（单元一部分在视锥体内部），默认关闭
                extract->ExtractOnlyBoundaryCellsOn(); // 仅提取一部分位于视锥体内部一部分在外部的单元
                extract->Update();

                std::cout << "-- origin\npoints\t" << m_polyData->GetNumberOfPoints() << "\ncells\t" << m_polyData->GetNumberOfCells() << '\n';
                std::cout << "-- extract\npoints\t" << extract->GetOutput()->GetNumberOfPoints() << "\ncells\t"
                          << extract->GetOutput()->GetNumberOfCells() << '\n';

                mapper->SetInputData(extract->GetOutput());
                mapper->SetResolveCoincidentTopologyToPolygonOffset(); // 始终显示在球的上层
                mapper->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -1.e6);
            }

            m_extractActor->SetMapper(mapper);
        }

        this->Interactor->Render();
    }

    void SetPolyData(const vtkSmartPointer<vtkPolyData>& poly)
    {
        m_polyData = poly;
        m_extractActor->GetProperty()->SetColor(1, 0, 0);
        this->CurrentRenderer->AddActor(m_extractActor);
    }

private:
    vtkSmartPointer<vtkPolyData> m_polyData {};
    vtkSmartPointer<vtkActor> m_extractActor { vtkSmartPointer<vtkActor>::New() };
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkSphereSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetColor(0., 1., 0.);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<MyStyle> style;
    style->SetCurrentRenderer(renderer);
    style->SetPolyData(source->GetOutput());

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST501

#ifdef TEST502

#include <vtkActor.h>
#include <vtkClipPolyData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> source;
    source->Update();

    // 裁剪的平面
    // Origin是经过的点，Normal表示该平面垂直于此方向
    vtkNew<vtkPlane> clipPlane;
    clipPlane->SetNormal(1.0, 0.0, 0.0);
    clipPlane->SetOrigin(0.0, 0.0, 0.0);

    vtkNew<vtkClipPolyData> clipper;
    clipper->SetInputConnection(source->GetOutputPort());
    clipper->SetClipFunction(clipPlane);
    clipper->SetValue(0.0); // 隐式函数裁剪、标量裁剪的值
    clipper->GenerateClippedOutputOn();

    vtkNew<vtkPolyDataMapper> superquadricMapper;
    superquadricMapper->SetInputConnection(clipper->GetOutputPort());

    vtkNew<vtkActor> superquadricActor;
    superquadricActor->SetMapper(superquadricMapper);

    vtkNew<vtkProperty> backFaces;
    backFaces->SetColor(1, 0, 0);

    superquadricActor->SetBackfaceProperty(backFaces);

    vtkNew<vtkPolyDataMapper> clippedAwayMapper;
    clippedAwayMapper->SetInputData(clipper->GetClippedOutput());
    clippedAwayMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> clippedAwayActor;
    clippedAwayActor->SetMapper(clippedAwayMapper);
    clippedAwayActor->GetProperty()->SetColor(1, 1, 0);
    clippedAwayActor->GetProperty()->SetOpacity(0.3);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(superquadricActor);
    renderer->AddActor(clippedAwayActor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleTrackballCamera> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST502

#ifdef TEST503

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractCells.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkUnstructuredGrid.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetColor(0., 1., 0.);

    vtkNew<vtkIdList> idList;
    for (vtkIdType i = 0; i < source->GetOutput()->GetNumberOfCells(); ++i)
    {
        if (0 == i % 2)
        {
            // 提取单元id为偶数的所有单元，即每隔一个单元提取一次
            idList->InsertNextId(i);
        }
    }

    vtkNew<vtkExtractCells> extract;
    extract->SetInputData(source->GetOutput());
    extract->SetCellList(idList);
    extract->Update();

    std::cout << "origin cells\t" << source->GetOutput()->GetNumberOfCells() << '\n';
    std::cout << "extract cells\t" << extract->GetOutput()->GetNumberOfCells() << '\n';

    vtkNew<vtkDataSetMapper> mapper2;
    mapper2->SetInputData(extract->GetOutput());
    mapper2->SetResolveCoincidentTopologyToPolygonOffset(); // 始终显示在球的上层
    mapper2->SetRelativeCoincidentTopologyPolygonOffsetParameters(0, -1.e6);

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(1., 0., 0.);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST503

#ifdef TEST504

#include <vtkClipPolyData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSelectPolyData.h>
#include <vtkSphereSource.h>

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetPhiResolution(10);
    sphereSource->SetThetaResolution(10);
    sphereSource->SetRadius(1.0);
    sphereSource->Update();

    vtkNew<vtkPoints> selectionPoints;
    selectionPoints->InsertNextPoint(-1, -1, 1);
    selectionPoints->InsertNextPoint(1, -1, 1);
    selectionPoints->InsertNextPoint(1, 1, 1);
    selectionPoints->InsertNextPoint(-1, 1, 1);

    // 指定选择的区域
    vtkNew<vtkSelectPolyData> loop;
    loop->SetInputConnection(sphereSource->GetOutputPort());
    loop->SetLoop(selectionPoints);
    loop->GenerateUnselectedOutputOff();
    loop->GenerateSelectionScalarsOn();
    loop->SetSelectionModeToSmallestRegion();
    loop->InsideOutOn();
    loop->Update();

    std::cout << "Number of output port\t" << loop->GetNumberOfOutputPorts() << '\n';

    vtkNew<vtkClipPolyData> clip;
    clip->SetInputConnection(loop->GetOutputPort());

    vtkNew<vtkPolyDataMapper> clipMapper;
    clipMapper->SetInputConnection(clip->GetOutputPort());
    // clipMapper->SetInputConnection(loop->GetOutputPort(2));
    clipMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> clipActor;
    clipActor->SetMapper(clipMapper);
    clipActor->GetProperty()->SetColor(0, 1, 0);
    clipActor->GetProperty()->EdgeVisibilityOn();

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(clipActor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST504

#ifdef TEST506

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
    std::array<double, 5> x = { 0, 1, 2, 3, 4 };
    std::array<double, 4> y = { 5, 6, 7, 8 };
    std::array<double, 3> z = { 9, 10, 11 };

    // 通过定义三个指定x-y-z方向坐标的阵列来创建直线栅格。
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

    // 从直线网格中提取几何体，通过指定SetExtent()可以提取到点、线、面、体
    vtkNew<vtkRectilinearGridGeometryFilter> plane;
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 4, 0, 0, 0, 2); // 提取XYZ方向上的坐标序号，X提取第0到第4个，Y只要第0个，Z提取第0到第2个，相当于提取了一个平面
    plane->Update();

    vtkNew<vtkDataSetMapper> rgridMapper;
    rgridMapper->SetInputData(plane->GetOutput());

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

#endif // TEST506

#ifdef TEST507

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricTorus.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataSilhouette.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// 每一帧都会计算，模型比较大时会非常卡

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkParametricTorus> surface;
    vtkNew<vtkParametricFunctionSource> source;
    source->SetUResolution(20);
    source->SetVResolution(20);
    source->SetParametricFunction(surface);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1., 0., 0.);
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkPolyDataSilhouette> silhouette;
    silhouette->SetInputData(source->GetOutput());
    silhouette->SetCamera(renderer->GetActiveCamera());
    silhouette->SetEnableFeatureAngle(0);

    vtkNew<vtkPolyDataMapper> mapper2;
    mapper2->SetInputConnection(silhouette->GetOutputPort());

    vtkNew<vtkActor> actor2;
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetColor(0., 1., 0.);
    actor2->GetProperty()->SetLineWidth(5);
    renderer->AddActor(actor2);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);

    renderer->ResetCamera();
    renderWindow->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST507

#ifdef TEST508

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDiskSource.h>
#include <vtkFeatureEdges.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

enum class PrimitiveType
{
    Poly,   // 四边形，面
    Line,   // 四边形，线框
    Disk,   // 圆盘
    Sphere, // 球面
    Arrow,  // 箭头
    Box,    // 开口的盒子（5个四边形）
};

/// @details 生成vtkFeatureEdges的输入图形
vtkSmartPointer<vtkPolyData> CreatePolyData(PrimitiveType pt)
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> polyCells;
    polyCells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkCellArray> lineCells;
    lineCells->InsertNextCell({ 0, 1 });
    lineCells->InsertNextCell({ 1, 2 });
    lineCells->InsertNextCell({ 2, 3 });
    lineCells->InsertNextCell({ 3, 0 });

    vtkNew<vtkPolyData> polys;
    polys->SetPoints(points);
    polys->SetPolys(polyCells);

    vtkNew<vtkPolyData> lines;
    lines->SetPoints(points);
    lines->SetLines(lineCells);

    vtkNew<vtkDiskSource> disk;
    disk->Update();

    vtkNew<vtkSphereSource> sphere;
    sphere->Update();

    vtkNew<vtkArrowSource> arrow;
    arrow->Update();

    vtkNew<vtkPoints> box_points;
    box_points->InsertNextPoint(0., 0., 1.);
    box_points->InsertNextPoint(1., 0., 1.);
    box_points->InsertNextPoint(1., 1., 1.);
    box_points->InsertNextPoint(0., 1., 1.);
    box_points->InsertNextPoint(0., 0., 0.);
    box_points->InsertNextPoint(1., 0., 0.);
    box_points->InsertNextPoint(1., 1., 0.);
    box_points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> box_cells;
    box_cells->InsertNextCell({ 0, 1, 2, 3 });
    box_cells->InsertNextCell({ 1, 5, 6, 2 });
    box_cells->InsertNextCell({ 5, 4, 7, 6 });
    box_cells->InsertNextCell({ 4, 0, 3, 7 });
    box_cells->InsertNextCell({ 4, 5, 1, 0 });

    vtkNew<vtkPolyData> box;
    box->SetPoints(box_points);
    box->SetPolys(box_cells);

    switch (pt)
    {
    case PrimitiveType::Poly:
        return polys;
    case PrimitiveType::Line:
        return lines;
    case PrimitiveType::Disk:
        return disk->GetOutput();
    case PrimitiveType::Sphere:
        return sphere->GetOutput();
    case PrimitiveType::Arrow:
        return arrow->GetOutput();
    case PrimitiveType::Box:
        return box;
    default:
        return polys;
    }
}
} // namespace

int main(int, char*[])
{
    auto polyData = CreatePolyData(PrimitiveType::Disk);

    // 边界边：只被一个多边形或者一条边包围的边
    // 特征边：需要设置一个特征角的阈值，当包含同一条边的两个三角形的法向量的夹角大于该阈值时，即为一个特征边
    // 流行边：只被两个多边形包含的边
    // 非流形边：被三个或者三个以上多边形包围的边
    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputData(polyData);
    featureEdges->BoundaryEdgesOn();
    featureEdges->FeatureEdgesOn();
    featureEdges->ManifoldEdgesOn();
    featureEdges->NonManifoldEdgesOn();
    featureEdges->SetFeatureAngle(0.); // 特征边夹角阈值（默认30°）
    featureEdges->PassLinesOff();      // 如果开启，输入的Lines单元都将被传递到输出中
    featureEdges->ColoringOn();        // 根据边界类型着色
    featureEdges->Update();

    auto numberOfEdges = featureEdges->GetOutput()->GetNumberOfCells();
    std::cout << "Number of Edges: " << numberOfEdges << '\n';

    //---------------------------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1., 0., 0.);
    actor->GetProperty()->LightingOff();

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputData(featureEdges->GetOutput());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->LightingOff();

    //----------------------------------------------------------------------------------
    // 左边是原始图形，右边是提取的边界
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->AddActor(actor);
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->SetViewport(0., 0., .5, 1.);
    leftRenderer->ResetCamera();

    vtkNew<vtkRenderer> rightRenderer;
    rightRenderer->AddActor(edgeActor);
    rightRenderer->SetBackground(.3, .2, .1);
    rightRenderer->SetViewport(.5, 0., 1., 1.);
    rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->AddRenderer(rightRenderer);
    renderWindow->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST508

#ifdef TEST509

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDiskSource.h>
#include <vtkExtractEdges.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

enum class PrimitiveType
{
    Poly,   // 四边形，面
    Line,   // 四边形，线框
    Disk,   // 圆盘
    Sphere, // 球面
    Arrow,  // 箭头
    Box,    // 开口的盒子（5个四边形）
    Plane,  // 一个'田'字线框
};

/// @details 生成vtkFeatureEdges的输入图形
vtkSmartPointer<vtkPolyData> CreatePolyData(PrimitiveType pt)
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> polyCells;
    polyCells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkCellArray> lineCells;
    lineCells->InsertNextCell({ 0, 1 });
    lineCells->InsertNextCell({ 1, 2 });
    lineCells->InsertNextCell({ 2, 3 });
    lineCells->InsertNextCell({ 3, 0 });

    vtkNew<vtkPolyData> polys;
    polys->SetPoints(points);
    polys->SetPolys(polyCells);

    vtkNew<vtkPolyData> lines;
    lines->SetPoints(points);
    lines->SetLines(lineCells);

    vtkNew<vtkDiskSource> disk;
    disk->Update();

    vtkNew<vtkSphereSource> sphere;
    sphere->Update();

    vtkNew<vtkArrowSource> arrow;
    arrow->Update();

    vtkNew<vtkPoints> box_points;
    box_points->InsertNextPoint(0., 0., 1.);
    box_points->InsertNextPoint(1., 0., 1.);
    box_points->InsertNextPoint(1., 1., 1.);
    box_points->InsertNextPoint(0., 1., 1.);
    box_points->InsertNextPoint(0., 0., 0.);
    box_points->InsertNextPoint(1., 0., 0.);
    box_points->InsertNextPoint(1., 1., 0.);
    box_points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> box_cells;
    box_cells->InsertNextCell({ 0, 1, 2, 3 });
    box_cells->InsertNextCell({ 1, 5, 6, 2 });
    box_cells->InsertNextCell({ 5, 4, 7, 6 });
    box_cells->InsertNextCell({ 4, 0, 3, 7 });
    box_cells->InsertNextCell({ 4, 5, 1, 0 });

    vtkNew<vtkPolyData> box;
    box->SetPoints(box_points);
    box->SetPolys(box_cells);

    vtkNew<vtkPoints> plane_points;
    plane_points->InsertNextPoint(0., 0., 0.);
    plane_points->InsertNextPoint(0., 1., 0.);
    plane_points->InsertNextPoint(0., 2., 0.);
    plane_points->InsertNextPoint(1., 0., 0.);
    plane_points->InsertNextPoint(1., 1., 0.);
    plane_points->InsertNextPoint(1., 2., 0.);
    plane_points->InsertNextPoint(2., 0., 0.);
    plane_points->InsertNextPoint(2., 1., 0.);
    plane_points->InsertNextPoint(2., 2., 0.);

    vtkNew<vtkCellArray> plane_cells;
    plane_cells->InsertNextCell({ 0, 1, 2 });
    plane_cells->InsertNextCell({ 3, 4, 5 });
    plane_cells->InsertNextCell({ 6, 7, 8 });
    plane_cells->InsertNextCell({ 0, 3, 6 });
    plane_cells->InsertNextCell({ 1, 4, 7 });
    plane_cells->InsertNextCell({ 2, 5, 8 });

    vtkNew<vtkPolyData> plane;
    plane->SetPoints(plane_points);
    plane->SetLines(plane_cells);

    switch (pt)
    {
    case PrimitiveType::Poly:
        return polys;
    case PrimitiveType::Line:
        return lines;
    case PrimitiveType::Disk:
        return disk->GetOutput();
    case PrimitiveType::Sphere:
        return sphere->GetOutput();
    case PrimitiveType::Arrow:
        return arrow->GetOutput();
    case PrimitiveType::Box:
        return box;
    case PrimitiveType::Plane:
        return plane;
    default:
        return polys;
    }
}
} // namespace

int main(int, char*[])
{
    auto polyData = CreatePolyData(PrimitiveType::Disk);

    vtkNew<vtkExtractEdges> extracter;
    extracter->SetInputData(polyData);
    extracter->UseAllPointsOff(); // 如果关闭，输入的某些点可能不会在输出中，因为某些点不一定用到，顶点ID也会变化
    extracter->Update();

    auto numberOfEdges = extracter->GetOutput()->GetNumberOfCells();
    std::cout << "Number of Edges: " << numberOfEdges << '\n';

    //---------------------------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1., 0., 0.);
    actor->GetProperty()->LightingOff();

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputData(extracter->GetOutput());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->LightingOff();

    //----------------------------------------------------------------------------------
    // 左边是原始图形，右边是提取的边界
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->AddActor(actor);
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->SetViewport(0., 0., .5, 1.);
    leftRenderer->ResetCamera();

    vtkNew<vtkRenderer> rightRenderer;
    rightRenderer->AddActor(edgeActor);
    rightRenderer->SetBackground(.3, .2, .1);
    rightRenderer->SetViewport(.5, 0., 1., 1.);
    rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->AddRenderer(rightRenderer);
    renderWindow->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST509

#ifdef TEST510

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkDiskSource.h>
#include <vtkExtractEdges.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {

enum class PrimitiveType
{
    Poly,   // 四边形，面
    Line,   // 四边形，线框
    Disk,   // 圆盘
    Sphere, // 球面
    Arrow,  // 箭头
    Box,    // 开口的盒子（5个四边形）
    Plane,  // 一个'田'字线框
};

/// @details 生成vtkFeatureEdges的输入图形
vtkSmartPointer<vtkPolyData> CreatePolyData(PrimitiveType pt)
{
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(1., 0., 0.);
    points->InsertNextPoint(1., 1., 0.);
    points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> polyCells;
    polyCells->InsertNextCell({ 0, 1, 2, 3 });

    vtkNew<vtkCellArray> lineCells;
    lineCells->InsertNextCell({ 0, 1 });
    lineCells->InsertNextCell({ 1, 2 });
    lineCells->InsertNextCell({ 2, 3 });
    lineCells->InsertNextCell({ 3, 0 });

    vtkNew<vtkPolyData> polys;
    polys->SetPoints(points);
    polys->SetPolys(polyCells);

    vtkNew<vtkPolyData> lines;
    lines->SetPoints(points);
    lines->SetLines(lineCells);

    vtkNew<vtkDiskSource> disk;
    disk->Update();

    vtkNew<vtkSphereSource> sphere;
    sphere->Update();

    vtkNew<vtkArrowSource> arrow;
    arrow->Update();

    vtkNew<vtkPoints> box_points;
    box_points->InsertNextPoint(0., 0., 1.);
    box_points->InsertNextPoint(1., 0., 1.);
    box_points->InsertNextPoint(1., 1., 1.);
    box_points->InsertNextPoint(0., 1., 1.);
    box_points->InsertNextPoint(0., 0., 0.);
    box_points->InsertNextPoint(1., 0., 0.);
    box_points->InsertNextPoint(1., 1., 0.);
    box_points->InsertNextPoint(0., 1., 0.);

    vtkNew<vtkCellArray> box_cells;
    box_cells->InsertNextCell({ 0, 1, 2, 3 });
    box_cells->InsertNextCell({ 1, 5, 6, 2 });
    box_cells->InsertNextCell({ 5, 4, 7, 6 });
    box_cells->InsertNextCell({ 4, 0, 3, 7 });
    box_cells->InsertNextCell({ 4, 5, 1, 0 });

    vtkNew<vtkPolyData> box;
    box->SetPoints(box_points);
    box->SetPolys(box_cells);

    vtkNew<vtkPoints> plane_points;
    plane_points->InsertNextPoint(0., 0., 0.);
    plane_points->InsertNextPoint(0., 1., 0.);
    plane_points->InsertNextPoint(0., 2., 0.);
    plane_points->InsertNextPoint(1., 0., 0.);
    plane_points->InsertNextPoint(1., 1., 0.);
    plane_points->InsertNextPoint(1., 2., 0.);
    plane_points->InsertNextPoint(2., 0., 0.);
    plane_points->InsertNextPoint(2., 1., 0.);
    plane_points->InsertNextPoint(2., 2., 0.);

    vtkNew<vtkCellArray> plane_cells;
    plane_cells->InsertNextCell({ 0, 1, 2 });
    plane_cells->InsertNextCell({ 3, 4, 5 });
    plane_cells->InsertNextCell({ 6, 7, 8 });
    plane_cells->InsertNextCell({ 0, 3, 6 });
    plane_cells->InsertNextCell({ 1, 4, 7 });
    plane_cells->InsertNextCell({ 2, 5, 8 });

    vtkNew<vtkPolyData> plane;
    plane->SetPoints(plane_points);
    plane->SetLines(plane_cells);

    switch (pt)
    {
    case PrimitiveType::Poly:
        return polys;
    case PrimitiveType::Line:
        return lines;
    case PrimitiveType::Disk:
        return disk->GetOutput();
    case PrimitiveType::Sphere:
        return sphere->GetOutput();
    case PrimitiveType::Arrow:
        return arrow->GetOutput();
    case PrimitiveType::Box:
        return box;
    case PrimitiveType::Plane:
        return plane;
    default:
        return polys;
    }
}
} // namespace

int main(int, char*[])
{
    auto polyData = CreatePolyData(PrimitiveType::Arrow);

    vtkNew<vtkOutlineFilter> outLine;
    outLine->SetInputData(polyData);
    outLine->GenerateFacesOff();   // 是否将边框以面的形式生成
    outLine->SetCompositeStyle(0); // 复合数据集创建边框的样式
    outLine->Update();

    auto numberOfEdges = outLine->GetOutput()->GetNumberOfCells();
    std::cout << "Number of Edges: " << numberOfEdges << '\n';

    //---------------------------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0., 1., 0.);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1., 0., 0.);
    actor->GetProperty()->LightingOff();

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputData(outLine->GetOutput());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->LightingOff();

    //----------------------------------------------------------------------------------
    // 左边是原始图形，右边是提取的边界
    vtkNew<vtkRenderer> leftRenderer;
    leftRenderer->AddActor(actor);
    leftRenderer->SetBackground(.1, .2, .3);
    leftRenderer->SetViewport(0., 0., .5, 1.);
    leftRenderer->ResetCamera();

    vtkNew<vtkRenderer> rightRenderer;
    rightRenderer->AddActor(edgeActor);
    rightRenderer->SetBackground(.3, .2, .1);
    rightRenderer->SetViewport(.5, 0., 1., 1.);
    rightRenderer->SetActiveCamera(leftRenderer->GetActiveCamera());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(leftRenderer);
    renderWindow->AddRenderer(rightRenderer);
    renderWindow->SetSize(1200, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST510

#ifdef TEST601

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkPoints> points;
    // 0,1,2
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(1, 2, 0);

    // 3,4,5,6
    points->InsertNextPoint(3, 0, 0);
    points->InsertNextPoint(5, 0, 0);
    points->InsertNextPoint(4, 0, 2);
    points->InsertNextPoint(4, 2, 1);

    // 7,8,9,10,11,12,13,14
    points->InsertNextPoint(6, 0, 0);
    points->InsertNextPoint(8, 0, 0);
    points->InsertNextPoint(8, 0, 2);
    points->InsertNextPoint(6, 0, 2);
    points->InsertNextPoint(6, 2, 0);
    points->InsertNextPoint(9, 2, 0);
    points->InsertNextPoint(9, 2, 3);
    points->InsertNextPoint(6, 2, 3);

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);

    // 三角形
    vtkIdType ids_triangle[] { 0, 1, 2 };
    usg->InsertNextCell(VTK_TRIANGLE, 3, ids_triangle);

    // 四面体
    vtkIdType ids_tetra[] { 3, 4, 5, 6 };
    usg->InsertNextCell(VTK_TETRA, 4, ids_tetra);

    // 六面体
    vtkIdType ids_hexahedron[] { 7, 8, 9, 10, 11, 12, 13, 14 };
    usg->InsertNextCell(VTK_HEXAHEDRON, 8, ids_hexahedron);

    vtkNew<vtkDataSetSurfaceFilter> filter;
    filter->SetInputData(usg);
    filter->Update();

    auto output = filter->GetOutput();

    std::cout << "UnstructuredGrid\nPoints\t" << usg->GetNumberOfPoints() << "\nCells\t" << usg->GetNumberOfCells() << '\n';
    std::cout << "DataSetSurfaceFilter\nPoints\t" << output->GetNumberOfPoints() << "\nCells\t" << output->GetNumberOfCells() << '\n';

    vtkNew<vtkDataSetMapper> mapper;
    // mapper->SetInputData(usg);
    mapper->SetInputData(output);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST601

#ifdef TEST602

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

vtkSmartPointer<vtkDataObject> CreateUns()
{
    vtkNew<vtkPoints> points;
    // 0,1,2
    points->InsertNextPoint(0, 0, 0);
    points->InsertNextPoint(2, 0, 0);
    points->InsertNextPoint(1, 2, 0);

    // 3,4,5,6
    points->InsertNextPoint(3, 0, 0);
    points->InsertNextPoint(5, 0, 0);
    points->InsertNextPoint(4, 0, 2);
    points->InsertNextPoint(4, 2, 1);

    // 7,8,9,10,11,12,13,14
    points->InsertNextPoint(6, 0, 0);
    points->InsertNextPoint(8, 0, 0);
    points->InsertNextPoint(8, 0, 2);
    points->InsertNextPoint(6, 0, 2);
    points->InsertNextPoint(6, 2, 0);
    points->InsertNextPoint(9, 2, 0);
    points->InsertNextPoint(9, 2, 3);
    points->InsertNextPoint(6, 2, 3);

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);

    // 三角形
    vtkIdType ids_triangle[] { 0, 1, 2 };
    usg->InsertNextCell(VTK_TRIANGLE, 3, ids_triangle);

    // 四面体
    vtkIdType ids_tetra[] { 3, 4, 5, 6 };
    usg->InsertNextCell(VTK_TETRA, 4, ids_tetra);

    // 六面体
    vtkIdType ids_hexahedron[] { 7, 8, 9, 10, 11, 12, 13, 14 };
    usg->InsertNextCell(VTK_HEXAHEDRON, 8, ids_hexahedron);

    std::cout << "Before\nPoints\t" << usg->GetNumberOfPoints() << "\nCells\t" << usg->GetNumberOfCells() << '\n';

    return usg;
}

// 原本6面体是1个单元，可以从6面体提取6个边界（面）

int main()
{
    vtkNew<vtkGeometryFilter> filter;
    filter->SetInputData(CreateUns());
    // filter->SetExcludedFacesData() // 设置提取时需要排除的单元列表
    filter->Update();

    auto output = filter->GetOutput();
    std::cout << "After:\nPoints\t" << output->GetNumberOfPoints() << "\nCells\t" << output->GetNumberOfCells() << '\n';

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(output);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST602

#ifdef TEST603

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDataObjectWriter.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetToDataObjectFilter.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkFieldDataToAttributeDataFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

int main()
{
    vtkNew<vtkPoints> points;
    // 0,1,2
    points->InsertNextPoint(0., 0., 0.);
    points->InsertNextPoint(2., 0., 0.);
    points->InsertNextPoint(1., 2., 0.);

    // 3,4,5,6
    points->InsertNextPoint(3., 0., 0.);
    points->InsertNextPoint(5., 0., 0.);
    points->InsertNextPoint(4., 0., 2.);
    points->InsertNextPoint(4., 2., 1.);

    // 7,8,9,10,11,12,13,14
    points->InsertNextPoint(6., 0., 0.);
    points->InsertNextPoint(8., 0., 0.);
    points->InsertNextPoint(8., 0., 2.);
    points->InsertNextPoint(6., 0., 2.);
    points->InsertNextPoint(6., 2., 0.);
    points->InsertNextPoint(9., 2., 0.);
    points->InsertNextPoint(9., 2., 3.);
    points->InsertNextPoint(6., 2., 3.);

    vtkNew<vtkUnstructuredGrid> usg;
    usg->SetPoints(points);

    // 三角形
    vtkIdType ids_triangle[] { 0, 1, 2 };
    usg->InsertNextCell(VTK_TRIANGLE, 3, ids_triangle);

    // 四面体
    vtkIdType ids_tetra[] { 3, 4, 5, 6 };
    usg->InsertNextCell(VTK_TETRA, 4, ids_tetra);

    // 六面体
    vtkIdType ids_hexahedron[] { 7, 8, 9, 10, 11, 12, 13, 14 };
    usg->InsertNextCell(VTK_HEXAHEDRON, 8, ids_hexahedron);

    // 顶点Scalar数据
    vtkNew<vtkDoubleArray> ptScalars0;
    ptScalars0->SetName("ptScalars0");
    vtkNew<vtkDoubleArray> ptScalars1;
    ptScalars1->SetName("ptScalars1");
    for (vtkIdType i = 0; i < usg->GetNumberOfPoints(); ++i)
    {
        ptScalars0->InsertNextValue(static_cast<double>(i));
        ptScalars1->InsertNextValue(static_cast<double>(i * 2));
    }
    usg->GetPointData()->AddArray(ptScalars0);
    usg->GetPointData()->AddArray(ptScalars1);

    // 单元Vector数据
    vtkNew<vtkDoubleArray> cellVectors0;
    cellVectors0->SetName("cellVectors0");
    cellVectors0->SetNumberOfComponents(3);
    vtkNew<vtkDoubleArray> cellVectors1;
    cellVectors1->SetName("cellVectors1");
    cellVectors1->SetNumberOfComponents(3);
    for (vtkIdType i = 0; i < usg->GetNumberOfCells(); ++i)
    {
        auto val = static_cast<double>(i);
        cellVectors0->InsertNextTuple3(val, val, val);
        cellVectors1->InsertNextTuple3(val * 2, val * 2, val * 2);
    }
    usg->GetCellData()->AddArray(cellVectors0);
    usg->GetCellData()->AddArray(cellVectors1);

    // 将DataSet转换为DataObject
    vtkNew<vtkDataSetToDataObjectFilter> filter;
    filter->CellDataOn(); // 打开数据集(DataSet)单元数据(Cell)到数据对象(DataObject)的转换，默认开启
    filter->GeometryOn();
    filter->TopologyOn();
    filter->FieldDataOn();
    filter->PointDataOn();
    filter->ModernTopologyOn();
    filter->LegacyTopologyOn();
    filter->SetInputData(usg);
    filter->Update();

    auto fieldData = filter->GetOutput()->GetFieldData();
    for (vtkIdType i = 0; i < fieldData->GetNumberOfArrays(); ++i)
    {
        auto arrayName = fieldData->GetArrayName(i);
        std::cout << "array " << i << " name: " << std::left << std::setw(20) << arrayName;

        auto arr = fieldData->GetArray(arrayName);
        std::cout << "type: " << std::setw(10) << arr->GetDataTypeAsString();

        auto pFloat = reinterpret_cast<float*>(arr->GetVoidPointer(0));
        auto size   = arr->GetNumberOfTuples();
        auto ncom   = arr->GetNumberOfComponents();
        std::cout << "size: " << std::setw(3) << size << "component: " << std::setw(3) << ncom << std::endl;
    }

    // 将数据对象写入文件
    vtkNew<vtkDataObjectWriter> writer;
    writer->SetFileName("dataObject.txt");
    writer->SetInputData(filter->GetOutput());
    writer->Write();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(usg);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->SetSize(800, 600);
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST603

#ifdef TEST604

// 官方示例 https://examples.vtk.org/site/Cxx/Modelling/FinanceFieldData/

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataObjectReader.h>
#include <vtkDataObjectToDataSetFilter.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkFieldDataToAttributeDataFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

int main()
{
    vtkNew<vtkDataObjectReader> reader;
    reader->SetFileName("../resources/dataObject.txt");
    reader->ReadAllColorScalarsOn();
    reader->ReadAllFieldsOn();
    reader->ReadAllScalarsOn();
    reader->ReadAllNormalsOn();
    reader->ReadAllVectorsOn();
    reader->Update();
    // reader->Print(std::cout);

    auto nFieldData    = reader->GetNumberOfFieldDataInFile();
    auto fieldDataName = reader->GetFieldDataNameInFile(0);

    // 顶点、单元数据
    vtkNew<vtkDataObjectToDataSetFilter> do2ds;
    do2ds->SetInputData(reader->GetOutput());
    do2ds->SetDataSetTypeToUnstructuredGrid();
    do2ds->SetPointComponent(0, "Points", 0);
    do2ds->SetPointComponent(1, "Points", 0);
    do2ds->SetPointComponent(2, "Points", 0);
    do2ds->SetCellTypeComponent("CellTypes", 0);
    do2ds->SetCellConnectivityComponent("Cells", 0); // {n0,p0,p1,p2,n1,p3,p4,p5...} n表示单元的顶点个数，p表示构成单元的顶点索引
    do2ds->Update();
    // do2ds->GetOutput()->Print(std::cout);

    // 顶点属性数据
    vtkNew<vtkFieldDataToAttributeDataFilter> fd2ad;
    fd2ad->SetInputData(do2ds->GetOutput());
    fd2ad->SetOutputAttributeDataToPointData();
    fd2ad->SetScalarComponent(0, "ptScalars1", 0);
    fd2ad->Update();

    auto scalars = fd2ad->GetOutput()->GetPointData()->GetScalars();
    std::cout << "Point scalars:\n";
    for (vtkIdType i = 0; i < scalars->GetNumberOfTuples(); ++i)
    {
        std::cout << scalars->GetTuple1(i) << ' ';
    }
    std::cout << std::endl;

    // 单元属性数据
    vtkNew<vtkFieldDataToAttributeDataFilter> fd2adCell;
    fd2adCell->SetInputData(do2ds->GetOutput());
    fd2adCell->SetOutputAttributeDataToCellData();
    fd2adCell->SetVectorComponent(0, "cellVectors1", 0);
    fd2adCell->SetVectorComponent(1, "cellVectors1", 0);
    fd2adCell->SetVectorComponent(2, "cellVectors1", 0);
    fd2adCell->Update();

    auto vectors = fd2adCell->GetOutput()->GetCellData()->GetVectors();
    std::cout << "Cell Vectors:\n";
    for (vtkIdType i = 0; i < vectors->GetNumberOfTuples(); ++i)
    {
        auto vec = vectors->GetTuple3(i);
        std::cout << i << " : " << vec[0] << ' ' << vec[1] << ' ' << vec[2] << '\n';
    }

    //----------------------------------------------------------
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(do2ds->GetOutput());
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);

    renderWindow->Render();
    interactor->Start();
}

#endif // TEST604

#ifdef TEST605

// This example shows how to manually create a structured grid.
// The basic idea is to instantiate vtkStructuredGrid, set its dimensions,
// and then assign points defining the grid coordinate. The number of
// points must equal the number of points implicit in the dimensions
// (i.e., dimX*dimY*dimZ). Also, data attributes (either point or cell)
// can be added to the dataset.
//
// https://vtk.org/doc/nightly/html/classvtkDataWriter.html

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDoubleArray.h>
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

#include <vtkDataSetWriter.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridReader.h>
#include <vtkStructuredGridWriter.h>
#include <vtkWriter.h>

#include <vtkDataSetMapper.h>

#include <fstream>

int main(int, char*[])
{
    double x[3], v[3], rMin = 0.5, rMax = 1.0, deltaRad, deltaZ;
    double radius, theta;
    static int dims[3] = { 13, 11, 11 };

    // Create the structured grid.
    vtkNew<vtkStructuredGrid> sgrid;
    sgrid->SetDimensions(dims);

    // We also create the points and vectors. The points
    // form a hemi-cylinder of data.
    vtkNew<vtkDoubleArray> vectors;
    vectors->SetNumberOfComponents(3);
    vectors->SetNumberOfTuples(dims[0] * dims[1] * dims[2]);
    vtkNew<vtkPoints> points;
    points->Allocate(dims[0] * dims[1] * dims[2]);

    deltaZ   = 2.0 / (dims[2] - 1);
    deltaRad = (rMax - rMin) / (dims[1] - 1);
    v[2]     = 0.0;
    for (auto k = 0; k < dims[2]; k++)
    {
        x[2]         = -1.0 + k * deltaZ;
        auto kOffset = k * dims[0] * dims[1];
        for (auto j = 0; j < dims[1]; j++)
        {
            radius       = rMin + j * deltaRad;
            auto jOffset = j * dims[0];
            for (auto i = 0; i < dims[0]; i++)
            {
                theta       = i * vtkMath::RadiansFromDegrees(15.0);
                x[0]        = radius * cos(theta);
                x[1]        = radius * sin(theta);
                v[0]        = -x[1];
                v[1]        = x[0];
                auto offset = i + jOffset + kOffset;
                points->InsertPoint(offset, x);
                vectors->InsertTuple(offset, v);
            }
        }
    }
    sgrid->SetPoints(points);
    sgrid->GetPointData()->SetVectors(vectors);

    //---------------写文件-----------------
    vtkNew<vtkStructuredGridWriter> writer;
    writer->SetFileTypeToASCII();
    writer->SetFileName("writerStructuredGrid.txt");
    writer->SetInputData(sgrid);
    writer->Write();

    // 写文件的具体代码：
    // vtkStructuredGridWriter.cxx -> line:30
    // void vtkStructuredGridWriter::WriteData();

    vtkNew<vtkDataSetWriter> writer2;
    writer2->SetFileName("writerDataSet.txt");
    writer2->SetFileTypeToASCII();
    writer2->SetInputData(vtkDataSet::SafeDownCast(sgrid));
    writer2->Write();
    //--------------------------------

    //----------------读文件----------------
    vtkNew<vtkStructuredGridReader> reader;
    reader->SetFileName("writerStructuredGrid.txt");
    reader->Update();

    vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
    geometryFilter->SetInputConnection(reader->GetOutputPort());
    geometryFilter->Update();
    //--------------------------------

    // Mapper
    // 读上来的DataSet转为PolyData拓扑结构有些问题
    vtkNew<vtkPolyDataMapper> mapperPolyData;
    // mapperPolyData->SetInputConnection(geometryFilter->GetOutputPort());
    mapperPolyData->SetInputData(geometryFilter->GetOutput());

    vtkNew<vtkDataSetMapper> mapperDataSet;
    mapperDataSet->SetInputData(sgrid);
    // mapperDataSet->SetInputData(reader->GetOutput());

    // Actor
    vtkNew<vtkActor> sgridActor;
    // sgridActor->SetMapper(mapperDataSet);
    sgridActor->SetMapper(mapperPolyData);

    sgridActor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("SGrid");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    renderer->AddActor(sgridActor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();
    renWin->SetSize(640, 480);

    // interact with data
    renWin->Render();
    iren->Start();

    return EXIT_SUCCESS;
}

#endif // TEST605

#ifdef TEST701

#include <vtkCallbackCommand.h>
#include <vtkLODProp3D.h>
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
void RefreshCallback(vtkObject* vtkNotUsed(caller), long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData));
}

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    // High res sphere
    vtkNew<vtkPolyDataMapper> highResMapper;
    vtkNew<vtkProperty> propertyHighRes;
    {
        vtkNew<vtkSphereSource> highResSphereSource;
        highResSphereSource->SetThetaResolution(100);
        highResSphereSource->SetPhiResolution(100);
        highResSphereSource->Update();

        highResMapper->SetInputConnection(highResSphereSource->GetOutputPort());

        propertyHighRes->SetDiffuseColor(0, 1, 0);
        propertyHighRes->SetInterpolationToFlat();
    }

    // Low res sphere
    vtkNew<vtkPolyDataMapper> lowResMapper;
    vtkNew<vtkProperty> propertyLowRes;
    {
        vtkNew<vtkSphereSource> lowResSphereSource;
        lowResSphereSource->SetThetaResolution(10);
        lowResSphereSource->SetPhiResolution(10);
        lowResSphereSource->Update();

        lowResMapper->SetInputConnection(lowResSphereSource->GetOutputPort());

        propertyLowRes->SetDiffuseColor(1, 0, 0);
        propertyLowRes->SetInterpolationToFlat();
    }

    vtkNew<vtkLODProp3D> prop;
    auto id1 = prop->AddLOD(lowResMapper, propertyLowRes, 0.0);
    auto id2 = prop->AddLOD(highResMapper, propertyHighRes, 0.0);

    std::cout << "There are " << prop->GetNumberOfLODs() << " LODs. "
              << "ID1: " << id1 << "\tID2: " << id2 << '\n';

    // A renderer and render window
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("LODProp3D");

    // prop->SetAllocatedRenderTime(1e-6,renderer);
    prop->SetAllocatedRenderTime(1e-12, renderer);

    // An interactor
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actors to the scene
    renderer->AddActor(prop);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    vtkNew<vtkCallbackCommand> refreshCallback;
    refreshCallback->SetCallback(RefreshCallback);
    refreshCallback->SetClientData(prop);

    renderWindow->AddObserver(vtkCommand::ModifiedEvent, refreshCallback);

    renderWindow->Render();

    // Begin mouse interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

namespace {
void RefreshCallback(vtkObject* vtkNotUsed(caller), long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData))
{
    auto lodProp = static_cast<vtkLODProp3D*>(clientData);
    std::cout << "Last rendered LOD ID: " << lodProp->GetLastRenderedLODID() << std::endl;
}
} // namespace

#endif // TEST701

#ifdef TEST801

#include <vtkActor.h>
#include <vtkCone.h>
#include <vtkContourFilter.h>
#include <vtkCylinder.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOutlineFilter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int, char*[])
{
    vtkNew<vtkPlane> function1;

    vtkNew<vtkCone> function2;
    function2->SetAngle(30.);

    vtkNew<vtkCylinder> function3;
    function3->SetRadius(5.);

    vtkNew<vtkSphere> function4;
    function4->SetRadius(5.);

    vtkNew<vtkSampleFunction> sample;
    sample->SetImplicitFunction(function3);
    sample->SetSampleDimensions(3, 3, 3);        // 指定要采样的数据的维度，数字越大采样的点越多
    sample->SetModelBounds(-2, 2, -2, 2, -2, 2); // 指定采样的区域，坐标范围
    sample->ComputeNormalsOff();
    sample->CappingOff();
    sample->Update();

    double range[2] {};
    sample->GetOutput()->GetScalarRange(range);
    std::cout << "Range:\n" << range[0] << '\t' << range[1] << '\n';

    //---------------------------------------------------------
    // 等值线，每一个等值线就是一个对应的隐式函数
    // 例如隐式函数是vtkSphere，那么等值线（面）就是一个球面
    vtkNew<vtkContourFilter> contours;
    contours->SetInputConnection(sample->GetOutputPort());
    contours->GenerateValues(6, range);

    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputConnection(contours->GetOutputPort());
    contourMapper->SetScalarRange(range);

    vtkNew<vtkActor> contourActor;
    contourActor->GetProperty()->LightingOff();
    contourActor->SetMapper(contourMapper);

    double bounds[6] {};
    contourActor->GetBounds(bounds);
    std::cout << "Bounds:\n"
              << bounds[0] << '\t' << bounds[1] << '\n'
              << bounds[2] << '\t' << bounds[3] << '\n'
              << bounds[4] << '\t' << bounds[5] << '\n';

    //---------------------------------------------------------
    // 边框
    vtkNew<vtkOutlineFilter> outline;
    outline->SetInputConnection(sample->GetOutputPort());

    vtkNew<vtkPolyDataMapper> outlineMapper;
    outlineMapper->SetInputConnection(outline->GetOutputPort());

    vtkNew<vtkActor> outlineActor;
    outlineActor->SetMapper(outlineMapper);
    outlineActor->GetProperty()->SetColor(0., 0., 0.);

    //---------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(contourActor);
    renderer->AddActor(outlineActor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("Samplefunction");

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST801

#ifdef TEST802

#include <vtkActor.h>
#include <vtkCone.h>
#include <vtkContourFilter.h>
#include <vtkCylinder.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkImageDataToPointSet.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOutlineFilter.h>
#include <vtkPlane.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSampleFunction.h>
#include <vtkSphere.h>

int main(int, char*[])
{
    vtkNew<vtkPlane> function1;

    vtkNew<vtkCone> function2;
    function2->SetAngle(30.);

    vtkNew<vtkCylinder> function3;
    function3->SetRadius(5.);

    vtkNew<vtkSphere> function4;
    function4->SetRadius(5.);

    vtkNew<vtkSampleFunction> sample;
    sample->SetImplicitFunction(function3);
    sample->SetSampleDimensions(3, 3, 3);        // 指定要采样的数据的维度，数字越大采样的点越多
    sample->SetModelBounds(-2, 2, -2, 2, -2, 2); // 指定采样的区域，坐标范围
    sample->ComputeNormalsOff();
    sample->CappingOff();
    sample->Update();

    double range[2] {};
    sample->GetOutput()->GetScalarRange(range);
    std::cout << "Range:\n" << range[0] << '\t' << range[1] << '\n';

    //---------------------------------------------------------
    // 将 vtkImageData转换为几何数据并渲染
    // 采样的维度决定了几何数据顶点的个数，采样的区域决定了几何数据的范围
    vtkNew<vtkImageDataToPointSet> imageDataToPointSet;
    imageDataToPointSet->SetInputData(sample->GetOutput());
    imageDataToPointSet->Update();

    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(imageDataToPointSet->GetOutput());
    mapper->SetScalarRange(range);

    vtkNew<vtkActor> actor;
    actor->GetProperty()->LightingOff();
    actor->GetProperty()->EdgeVisibilityOn();
    actor->SetMapper(mapper);

    std::cout << "Number of Points:\n"
              << imageDataToPointSet->GetOutput()->GetNumberOfPoints() << "\nNumber of Cells:\n"
              << imageDataToPointSet->GetOutput()->GetNumberOfCells() << '\n';

    double bounds[6] {};
    actor->GetBounds(bounds);
    std::cout << "Bounds:\n"
              << bounds[0] << '\t' << bounds[1] << '\n'
              << bounds[2] << '\t' << bounds[3] << '\n'
              << bounds[4] << '\t' << bounds[5] << '\n';

    //---------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);
    renderWindow->SetWindowName("Samplefunction");

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    interactor->SetInteractorStyle(style);

    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST802

#ifdef TEST803

#include <vtkImageData.h>
#include <vtkJPEGWriter.h>
#include <vtkNew.h>
#include <vtkPerlinNoise.h>
#include <vtkSampleFunction.h>

int main(int, char*[])
{
    vtkNew<vtkPerlinNoise> perlinNoise;
    perlinNoise->SetAmplitude(255.); // 设置振幅，默认为1，越大生成的图片颜色值越多。比如振幅为1，颜色值可能只有1和255
    perlinNoise->SetFrequency(1024., 1024., 1024.); // 设置频率
    perlinNoise->SetPhase(0., 0., 0.);              // 设置相位，可以移动噪声函数

    vtkNew<vtkSampleFunction> sample;
    sample->SetImplicitFunction(perlinNoise);
    sample->SetSampleDimensions(512, 512, 1);
    sample->ComputeNormalsOff();
    sample->SetOutputScalarTypeToUnsignedChar();
    sample->Update();

    std::cout << "Image is " << sample->GetOutput()->GetDataDimension() << "D\n";
    std::cout << "number of pixels: " << sample->GetOutput()->GetNumberOfPoints() << '\n';

    int dimensions[3] {};
    sample->GetOutput()->GetDimensions(dimensions);
    std::cout << "Dimensions: " << dimensions[0] << ' ' << dimensions[1] << ' ' << dimensions[2] << '\n';

    vtkNew<vtkJPEGWriter> writer;
    writer->SetInputData(sample->GetOutput());
    writer->SetFileName("perlinNosize.jpg");
    writer->Write();

    std::cout << "save image success\n";

    return EXIT_SUCCESS;
}

#endif // TEST803
