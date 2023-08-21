/**
 * 001.自定义 Filter 继承自 vtkPolyDataAlgorithm
 * 101.vtkProbeFilter 探针的基础使用
 * 102.vtkGlyph3D vtkProbeFilter vtkSampleFunction vtkThreshold
 *
 * 201.vtkWarpScalar vtkWarpVector 根据标量或向量值在指定方向对顶点进行偏移

 */

#define TEST003

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
