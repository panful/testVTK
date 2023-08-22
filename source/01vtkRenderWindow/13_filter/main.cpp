/**
 * 001.自定义 Filter 继承自 vtkPolyDataAlgorithm
 * 002.从经过 Filter 变换后的数据获取原始数据
 *
 * 101.vtkProbeFilter 探针的基础使用
 * 102.vtkGlyph3D vtkProbeFilter vtkSampleFunction vtkThreshold
 *
 * 201.vtkWarpScalar vtkWarpVector 根据标量或向量值在指定方向对顶点进行偏移
 *
 * 301.vtkAppendPolyData 合并多个 vtkPolyData
 * 302.合并带有Scalars、Vectors的vtkPolyData
 * 303.使用不同方式设置Scalars数据时，合并后的polyData没有Scalars数据
 *
 * 401 vtkCellDataToPointData 单元数据转顶点数据
 * 402 vtkPointDataToCellData 顶点数据转单元数据
 * 403 vtkCellCenters 获取单元中心即格心，并用球体标注格心
 */

#define TEST002

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

/*
 * https://kitware.github.io/vtk-examples/site/Cxx/Visualization/RandomProbe/
 *
 * vtkSampleFunction 用于将函数或隐式函数转换为VTK渲染或分析的数据结构
 * vtkThreshold 用于对数据集进行阈值处理，检测数据集的某个特定区域
 */

int main(int argc, char* argv[])
{
    int resolution = 50;

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
    randomProbe->GetOutput()->GetPointData()->SetActiveScalars("vtkValidPointMask");

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
    actor->GetProperty()->SetColor(1, 0, 0);

    // Visualize
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
