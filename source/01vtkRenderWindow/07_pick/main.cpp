/*
 * vtk拾取相关的类 https://blog.csdn.net/lovelanjuan/article/details/7301773
 *
 * 11.框选拾取 vtkAreaPicker
 * 28.actor拾取 vtkPropPicker 基于硬件拾取
 * 29.vtk拾取汇总，框选拾取，范围拾取，点拾取，单元拾取
 * 30.范围拾取 vtkAreaPicker 点到actor的最短距离 vtkCellLocator
 * 62 vtkInteractorStyleRubberBandPick 框选拾取
 * 19 单元拾取 cellpick
 * 35 拾取并标记
 * 39.拾取actor并找出距离拾取点最近的顶点
 * 40 拾取vtkAssembly vtkActor装配器
 *
 * 33 拾取 vtkAxesActor  PickPropFrom()
 * 34 拾取 vtkOrientationMarkerWidget 中的 vtkAxesActor
 * 36 拾取 vtkOrientationMarkerWidget 中的 vtkAxesActor，鼠标在坐标轴区域按下时坐标轴一直旋转，定时器的使用
 * 37 框选并删除3维模型的指定三角面片
 *
 * 多图层拾取 01_03_TEST45
 */

#define TEST19

#ifdef TEST11

// https://kitware.github.io/vtk-examples/site/Cxx/Picking/HighlightSelectedPoints/

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkExtractGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
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
class MyStyle : public vtkInteractorStyleRubberBandPick
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBandPick);

    MyStyle()
    {
        this->SelectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
        this->SelectedActor  = vtkSmartPointer<vtkActor>::New();
        this->SelectedActor->SetMapper(SelectedMapper);
    }

    virtual void OnLeftButtonUp() override
    {
        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

        vtkNew<vtkExtractGeometry> extractGeometry;
        extractGeometry->SetImplicitFunction(frustum);
        extractGeometry->SetInputData(this->Points);
        extractGeometry->Update();

        vtkNew<vtkVertexGlyphFilter> glyphFilter;
        glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
        glyphFilter->Update();

        vtkPolyData* selected = glyphFilter->GetOutput();
        std::cout << "Selected " << selected->GetNumberOfPoints() << " points." << std::endl;
        std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
        this->SelectedMapper->SetInputData(selected);
        this->SelectedMapper->ScalarVisibilityOff();

        vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(selected->GetPointData()->GetArray("OriginalIds"));
        for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
        {
            std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
        }

        this->SelectedActor->GetProperty()->SetColor(1., 0., 0.);
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

vtkStandardNewMacro(MyStyle);
} // namespace

/*
 * 按下R开始框选拾取
 * 通过构建一个视锥体，然后用这个视锥体对需要拾取的几何体进行提取操作
 */

int main(int, char*[])
{
    vtkNew<vtkPointSource> pointSource;
    pointSource->SetNumberOfPoints(20);
    pointSource->Update();

    vtkNew<vtkIdFilter> idFilter;
    idFilter->SetInputConnection(pointSource->GetOutputPort());
    idFilter->SetCellIdsArrayName("OriginalIds");
    idFilter->SetPointIdsArrayName("OriginalIds");
    idFilter->Update();

    vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
    surfaceFilter->SetInputConnection(idFilter->GetOutputPort());
    surfaceFilter->Update();

    vtkPolyData* input = surfaceFilter->GetOutput();

    //--------------------------------------------------------------------
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(input);
    mapper->ScalarVisibilityOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(5);
    actor->GetProperty()->SetColor(0., 1., 0.);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkAreaPicker> areaPicker;
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetPicker(areaPicker);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    style->SetPoints(input);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST11

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
    vtkTypeMacro(MouseInteractorHighLightActor, vtkInteractorStyleTrackballCamera);

    MouseInteractorHighLightActor()
    {
        LastPickedActor    = NULL;
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
            // std::cout << "restore color\t";
            this->LastPickedActor->GetProperty()->DeepCopy(this->LastPickedProperty);
        }
        this->LastPickedActor = picker->GetActor();
        if (this->LastPickedActor)
        {
            std::cout << "picked actor:\t" << theActors[this->LastPickedActor] << '\t';

            // std::cout << "set color\t";
            //  Save the property of the picked actor so that we can

            // 将上一次拾取的图元恢复颜色
            // restore it next time
            this->LastPickedProperty->DeepCopy(this->LastPickedActor->GetProperty());

            // 将这一次拾取的图元设置为红色
            // Highlight the picked actor by changing its properties
            this->LastPickedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

            // std::cout << LastPickedActor->GetClassName();

            // this->LastPickedActor->GetProperty()->SetDiffuse(1.0);
            // this->LastPickedActor->GetProperty()->SetSpecular(0.0);
            // this->LastPickedActor->GetProperty()->EdgeVisibilityOn();
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
        actor->GetProperty()->SetSpecularColor(colors->GetColor3d("White").GetData());
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
#include <vtkMinimalStandardRandomSequence.h>
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
#include <vtkSphereSource.h>

#include <vtkAreaPicker.h>
#include <vtkCellPicker.h>
#include <vtkPicker.h>
#include <vtkPointPicker.h>
#include <vtkPropPicker.h>
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

                auto pickPosition   = areaPicker->GetPickPosition();
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
                    auto prop     = props->GetNextProp3D();
                    auto assembly = vtkAssembly::SafeDownCast(prop);
                    auto parts    = assembly->GetParts();
                    auto num2     = parts->GetNumberOfItems();

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

                auto pickPosition = abstractPicker->GetPickPosition(); // 模型坐标，不是世界坐标，如果多个actor叠加，返回前面的actor坐标
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
                    auto pickActor    = propPicker->GetActor();
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
} // namespace

int main(int, char*[])
{
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

#if (1)
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

        // renderer->AddActor(actor);
    }

    renderer->AddActor(assembly);
#else
    std::array<float, 4 * 3> vertices { 10, 10, 0, 20, 10, 0, 20, 20, 0, 10, 20, 0 };

    std::array<long long, 4 * 2> indices { 0, 1, 1, 2, 2, 3, 3, 0 };

    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    for (size_t i = 0; i < indices.size(); i += 2)
    {
        cells->InsertNextCell({ indices[i], indices[i + 1] });
    }

    polyData->SetPoints(points);
    polyData->SetLines(cells);

    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(cubeMapper);
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->SetLineWidth(5);

    // vtkNew<vtkAssembly> assembly;
    // assembly->AddPart(actor);

    // renderer->AddActor(assembly);
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

#include <vtkAbstractPicker.h>
#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellLocator.h>
#include <vtkCellPicker.h>
#include <vtkCoordinate.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProp3DCollection.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWorldPointPicker.h>

#include <vtkMath.h>
#include <vtkMatrix4x4.h>

#include <array>
#include <iostream>

// 四条线段组成一个正方形
namespace {
std::array<float, 4 * 3> vertices1 { 10, 10, 0, 20, 10, 0, 20, 20, 0, 10, 20, 0 };

std::array<float, 4 * 3> vertices2 { 10, 10, 10, 20, 10, 10, 20, 20, 10, 10, 20, 10 };

std::array<long long, 4 * 2> indices { 0, 1, 1, 2, 2, 3, 3, 0 };

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
                double closestPoint[3];   // the coordinates of the closest point will be returned here
                double closestPointDist2; // the squared distance to the closest point will be returned here
                vtkIdType cellId;         // the cell id of the cell containing the closest point will be returned here
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
                double closestPoint[3];   // the coordinates of the closest point will be returned here
                double closestPointDist2; // the squared distance to the closest point will be returned here
                vtkIdType cellId;         // the cell id of the cell containing the closest point will be returned here
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
            // coord->SetCoordinateSystemToNormalizedDisplay();
            // coord->SetCoordinateSystemToView();
            coord->SetValue(posX, posY, 0.);
            auto displayValue = coord->GetComputedDisplayValue(this->CurrentRenderer);
            auto worldValue   = coord->GetComputedWorldValue(this->CurrentRenderer);
            std::cout << "world value\t" << worldValue[0] << '\t' << worldValue[1] << '\t' << worldValue[2] << '\n';
            std::cout << "display  value\t" << displayValue[0] << '\t' << displayValue[1] << '\t' << displayValue[2] << '\n';

            // auto picker = this->Interactor->GetPicker();
            // picker->Pick(posX, posY, 0, this->CurrentRenderer);

            vtkNew<vtkAreaPicker> picker;
            this->Interactor->SetPicker(picker);
            double bound = 5;
            picker->AreaPick(posX - bound, posY - bound, posX + bound, posY + bound, this->CurrentRenderer);
            auto props = picker->GetProp3Ds();
            props->InitTraversal();
            std::cout << "pick number: " << props->GetNumberOfItems() << '\n';

            auto pickPosition   = picker->GetPickPosition();   // 模型坐标系，返回的是拾取点在模型坐标系中的值
            auto selectionPoint = picker->GetSelectionPoint(); // 窗口坐标

            std::cout << "pickPosition\t" << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';
            std::cout << "selectionPoint\t" << selectionPoint[0] << '\t' << selectionPoint[1] << '\t' << selectionPoint[2] << '\n';

            auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
            cellLocator->SetDataSet(m_polyData);
            cellLocator->BuildLocator();

            // Find the closest points to TestPoint
            auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
            double closestPoint[3];   // the coordinates of the closest point will be returned here
            double closestPointDist2; // the squared distance to the closest point will be returned here
            vtkIdType cellId;         // the cell id of the cell containing the closest point will be returned here
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
                double modelPos[4] { closestPoint[0], closestPoint[1], closestPoint[2], 0.0 };
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
            // double ssss[4]{ closestPoint[0],closestPoint[1],closestPoint[2],0.0 };
            double ssss[4] { 10, 10, 0.0, 0.0 };
            auto mat  = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
            auto xxxx = mat->MultiplyPoint(ssss);

            // 世界坐标转屏幕坐标
            vtkNew<vtkCoordinate> coord1;
            coord1->SetCoordinateSystemToWorld();
            coord1->SetValue(xxxx);
            auto displayValue1 = coord1->GetComputedDisplayValue(this->CurrentRenderer);
            std::cout << "display  value\t" << displayValue1[0] << '\t' << displayValue1[1] << '\t' << displayValue1[2] << '\n';
            std::cout << std::hypot(displayValue1[0] - posX, displayValue1[1] - posY)
                      << '\n'; // 鼠标点击的位置与获取到的距离最近的点之间的距离（屏幕坐标）

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
                auto actor        = picker->GetActor();

                std::cout << actor << '\n';
                std::cout << "pickPostion : " << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';

                vtkNew<vtkRegularPolygonSource> polygonSource;
                polygonSource->GeneratePolygonOff();
                polygonSource->SetNumberOfSides(30);
                polygonSource->SetRadius(0.5);
                polygonSource->SetCenter(pickPosition);

                vtkNew<vtkPolyDataMapper> mapper;
                mapper->SetInputConnection(polygonSource->GetOutputPort());

                // vtkNew<vtkActor> actor;
                // actor->SetMapper(mapper);
                // actor->GetProperty()->SetColor(1, 0, 0);

                // this->CurrentRenderer->AddActor2D(actor);
            }
        }

        if (!this->Interactor)
        {
            auto posX = this->Interactor->GetEventPosition()[0];
            auto posY = this->Interactor->GetEventPosition()[1];

            // std::cout << "X : " << posX << "\tY : " << posY << '\n';

            vtkNew<vtkCoordinate> coord;
            coord->SetCoordinateSystemToDisplay();
            coord->SetValue(posX, posY, 0.);
            auto worldValue = coord->GetComputedWorldValue(this->CurrentRenderer);
            // std::cout << "world value\t" << worldValue[0] << '\t' << worldValue[1] << '\t' << worldValue[2] << '\n';

            auto cellLocator = vtkSmartPointer<vtkCellLocator>::New();
            cellLocator->SetDataSet(m_polyData);
            cellLocator->BuildLocator();

            // Find the closest points to TestPoint
            auto assistCell = vtkSmartPointer<vtkGenericCell>::New();
            double closestPoint[3];   // the coordinates of the closest point will be returned here
            double closestPointDist2; // the squared distance to the closest point will be returned here
            vtkIdType cellId;         // the cell id of the cell containing the closest point will be returned here
            int subId;

            cellLocator->FindClosestPoint(worldValue, closestPoint, assistCell, cellId, subId, closestPointDist2);
            // std::cout << "closest point: " << closestPoint[0] << " " << closestPoint[1] << " " << closestPoint[2] << std::endl;

            auto modelMat     = this->CurrentRenderer->GetActiveCamera()->GetModelTransformMatrix();
            auto modelViewMat = this->CurrentRenderer->GetActiveCamera()->GetModelViewTransformMatrix();
            auto viewTransMat = this->CurrentRenderer->GetActiveCamera()->GetViewTransformMatrix();
            auto eyeTransMat  = this->CurrentRenderer->GetActiveCamera()->GetEyeTransformMatrix();

            auto cameraPos = this->CurrentRenderer->GetActiveCamera()->GetPosition();
            std::cout << "camera\t" << cameraPos[0] << ',' << cameraPos[1] << ',' << cameraPos[2] << '\n';

            // std::cout << "model mat: ";
            // for (size_t i = 0; i < 16; ++i)
            //{
            //     std::cout << modelMat->GetData()[i] << ',';
            // }
            // std::cout << "\nmodel view mat: ";
            // for (size_t i = 0; i < 16; ++i)
            //{
            //     std::cout << modelViewMat->GetData()[i] << ',';
            // }
            // std::cout << "\nview trans mat: ";
            // for (size_t i = 0; i < 16; ++i)
            //{
            //     std::cout << viewTransMat->GetData()[i] << ',';
            // }
            // std::cout << "\neye trans mat: ";
            // for (size_t i = 0; i < 16; ++i)
            //{
            //     std::cout << eyeTransMat->GetData()[i] << ',';
            // }
            // std::cout << '\n';

            double modelPos[4] { closestPoint[0], closestPoint[1], closestPoint[2], 0.0 };
            auto worldValue1 = modelMat->MultiplyPoint(modelPos);

            vtkNew<vtkMatrix4x4> matInvert;
            vtkMatrix4x4::Invert(modelMat, matInvert);

            vtkNew<vtkMatrix4x4> matTranspose;
            vtkMatrix4x4::Transpose(modelMat, matTranspose);

            auto modelPos1 = matInvert->MultiplyPoint(worldValue1);
            auto modelPos2 = matTranspose->MultiplyPoint(worldValue1);

            // std::cout << modelPos[0] << ',' << modelPos[1] << ',' << modelPos[2] << '\n';
            // std::cout << modelPos1[0] << ',' << modelPos1[1] << ',' << modelPos1[2] << '\n';
            // std::cout << modelPos2[0] << ',' << modelPos2[1] << ',' << modelPos2[2] << '\n';
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
    vtkSmartPointer<vtkPolyData> m_polyData { nullptr };
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

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
        cells->InsertNextCell({ 0, 1, 2, 3 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputData(polyData);

        // actor
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

        cells->InsertNextCell({ 0, 1, 2, 3 });

        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        // mapper
        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputData(polyData);

        // actor
        vtkNew<vtkActor> cubeActor;
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(1, 1, 0);

        renderer->AddActor(cubeActor);
    }

    // 从actor获取顶点数据
    // auto dataset = cubeActor->GetMapper()->GetInput();
    // vtkNew<vtkIdList> pts;
    // dataset->GetCellPoints(1, pts);
    // auto num = pts->GetNumberOfIds();
    // auto id1 = pts->GetId(0);
    // auto id2 = pts->GetId(1);
    // double pt[3]{ 0. };
    // dataset->GetPoint(id1, pt);
    // dataset->GetPoint(id2, pt);

    // renderer
    renderer->GetActiveCamera()->ParallelProjectionOff();
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600);

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<InteractorStyle> style;
    // style->SetPolyData(polyData);
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST30

#ifdef TEST39

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkCoordinate.h>
#include <vtkFloatArray.h>
#include <vtkGenericCell.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkLookupTable.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <array>
#include <iostream>

namespace {
std::array<float, 6 * 3 * 3> vertices {
    0.0f, 0.0f, 0.0f,                                                                         // 0
    0.0f, 1.0f, 0.0f, -.1f, 0.9f, -.1f, -.1f, 0.9f, 0.1f, 0.1f, 0.9f, -.1f, 0.1f, 0.9f, 0.1f, // 5

    1.0f, 0.0f, 0.0f,                                                                         // 6
    1.0f, 1.0f, 0.0f, 0.9f, 0.9f, -.1f, 0.9f, 0.9f, 0.1f, 1.1f, 0.9f, -.1f, 1.1f, 0.9f, 0.1f, // 11

    2.0f, 0.0f, 0.0f,                                                                         // 12
    2.0f, 1.0f, 0.0f, 1.9f, 0.9f, -.1f, 1.9f, 0.9f, 0.1f, 2.1f, 0.9f, -.1f, 2.1f, 0.9f, 0.1f, // 17
};

// 一个箭头由两个三角形和一条线段构成
std::array<long long, 3 * 2> linesIndices {
    0,
    1,
    6,
    7,
    12,
    13,
};

std::array<long long, 3 * 6> triangleIndices { 1, 2, 3, 1, 4, 5,

    7, 8, 9, 7, 10, 11,

    13, 14, 15, 13, 16, 17 };

class MyInteractorStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyInteractorStyle* New();
    vtkTypeMacro(MyInteractorStyle, vtkInteractorStyleRubberBand3D);

    virtual void OnLeftButtonUp() override
    {
        std::cout << "------------------------------------\n";

        if (this->Interactor)
        {
            auto posX = this->Interactor->GetEventPosition()[0];
            auto posY = this->Interactor->GetEventPosition()[1];
            std::cout << "mouse X : " << posX << "\tY : " << posY << '\n';

            vtkNew<vtkPropPicker> propPicker;
            this->Interactor->SetPicker(propPicker);

            if (propPicker->Pick(posX, posY, 0, this->CurrentRenderer) != 0)
            {
                auto pickPosition = propPicker->GetPickPosition();
                auto pickActor    = propPicker->GetActor();
                std::cout << "pickPostion : " << pickPosition[0] << '\t' << pickPosition[1] << '\t' << pickPosition[2] << '\n';

                auto dataSet = pickActor->GetMapper()->GetInput();

                std::array<double, 3> tempPt;
                double tempDis { 0. };
                int tempSubId { 0 };
                vtkIdType tempCellId { 0 };

                vtkNew<vtkGenericCell> genericCell;
                vtkNew<vtkCellLocator> cellLocator;
                cellLocator->SetDataSet(dataSet);
                cellLocator->BuildLocator();

                // 查找距离点pickPosition距离最近的单元
                cellLocator->FindClosestPoint(pickPosition, tempPt.data(), tempCellId, tempSubId, tempDis);
                std::cout << tempCellId << '\t' << tempSubId << '\t' << tempDis << '\n';

                // 使用vtkGenericCell要比不使用速度快
                cellLocator->FindClosestPoint(pickPosition, tempPt.data(), genericCell, tempCellId, tempSubId, tempDis);
                std::cout << tempCellId << '\t' << tempSubId << '\t' << tempDis << '\n';

                // 获取构成该单元的所有顶点
                vtkNew<vtkIdList> pts;
                dataSet->GetCellPoints(tempCellId, pts);
                double distance { VTK_DOUBLE_MAX };
                int pointID { -1 };

                for (auto i = 0; i < pts->GetNumberOfIds(); ++i)
                {
                    double pt[3] { 0. };
                    dataSet->GetPoint(pts->GetId(i), pt);

                    auto _distance_ = std::hypot(pt[0] - pickPosition[0], pt[1] - pickPosition[1], pt[2] - pickPosition[2]);
                    if (_distance_ < distance)
                    {
                        // 距离pickPosition最近的顶点
                        distance = _distance_;
                        pointID  = pts->GetId(i);
                    }
                }
                std::cout << "point id: " << pointID << '\n';
            }
        }

        this->Interactor->Render();
        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyInteractorStyle);
} // namespace

int main()
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cellsTriangles;
    vtkNew<vtkCellArray> cellsLines;

    vtkNew<vtkFloatArray> scalars;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
    for (size_t i = 0; i < linesIndices.size(); i += 2)
    {
        cellsLines->InsertNextCell({ linesIndices[i], linesIndices[i + 1] });
    }
    for (size_t i = 0; i < triangleIndices.size(); i += 3)
    {
        cellsTriangles->InsertNextCell({ triangleIndices[i], triangleIndices[i + 1], triangleIndices[i + 2] });
    }

    scalars->SetNumberOfValues(9);
    for (size_t i = 0; i < 3; i++)
    {
        scalars->SetValue(i, i);
        scalars->SetValue(2 * i + 3, i);
        scalars->SetValue(2 * i + 3 + 1, i);
    }

    polyData->SetPoints(points);
    polyData->SetPolys(cellsTriangles);
    polyData->SetLines(cellsLines);
    polyData->GetCellData()->SetScalars(scalars);

    // lookupTable
    vtkNew<vtkLookupTable> lut;
    lut->SetRange(scalars->GetRange());
    lut->SetHueRange(0.67, 0.);
    lut->Build();

    // mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);
    mapper->SetScalarRange(scalars->GetRange());
    mapper->SetLookupTable(lut);
    mapper->ScalarVisibilityOn();

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

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

    // interactor syle
    vtkNew<MyInteractorStyle> style;
    iren->SetInteractorStyle(style);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST39

#ifdef TEST40

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkCellPicker.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

namespace {
vtkNew<vtkActor> sphereActor;
vtkNew<vtkActor> cubeActor;
vtkNew<vtkActor> coneActor;

class InteractorStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static InteractorStyle* New();
    vtkTypeMacro(InteractorStyle, vtkInteractorStyleRubberBand3D);

    void OnLeftButtonUp() override
    {
        if (this->Interactor)
        {
            auto eventPos = this->Interactor->GetEventPosition();
            this->FindPokedRenderer(eventPos[0], eventPos[1]);
            if (this->CurrentRenderer)
            {
                // 使用vtkPropPicker拾取
                {
                    vtkNew<vtkPropPicker> picker;
                    picker->PickFromListOn();
                    picker->AddPickList(m_assembly);
                    this->Interactor->SetPicker(picker);

                    if (auto numPick = picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer))
                    {
                        // assembly prop3D viewProp都是同一个对象，即assembly
                        auto actor        = picker->GetActor(); // null
                        auto assembly     = picker->GetAssembly();
                        auto prop3D       = picker->GetProp3D();
                        auto propAssembly = picker->GetPropAssembly(); // null
                        auto viewProp     = picker->GetViewProp();
                        auto pickList     = picker->GetPickList(); // number = 0

                        printf("---------------------------- prop pick\n");
                        printf("actor\t\t%p\n", actor);
                        printf("assembly\t%p\n", assembly);
                        printf("prop3D\t\t%p\n", prop3D);
                        printf("propAssembly\t%p\n", propAssembly);
                        printf("viewProp\t%p\n", viewProp);
                        printf("pickList\t%p\n", pickList);

                        if (auto path = picker->GetPath())
                        {
                            printf("numOfItems\t%d\n", path->GetNumberOfItems());
                            if (auto actor = vtkActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
                            {
                                // 始终都是同一个actor
                                // 所以不能使用vtkPropPicker来拾取vtkAssembly
                                printf("picked\t%p\n", actor);
                            }
                        }
                    }
                }

                // 使用vtkCellPicker拾取
                {
                    vtkNew<vtkCellPicker> picker;
                    this->Interactor->SetPicker(picker);

                    if (auto numPick = picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer))
                    {
                        // assembly prop3D viewProp都是同一个对象，即assembly
                        auto actor        = picker->GetActor(); // null
                        auto assembly     = picker->GetAssembly();
                        auto prop3D       = picker->GetProp3D();
                        auto propAssembly = picker->GetPropAssembly(); // null
                        auto viewProp     = picker->GetViewProp();
                        auto pickList     = picker->GetPickList(); // number = 0

                        printf("---------------------------- cell pick\n");
                        printf("actor\t\t%p\n", actor);
                        printf("assembly\t%p\n", assembly);
                        printf("prop3D\t\t%p\n", prop3D);
                        printf("propAssembly\t%p\n", propAssembly);
                        printf("viewProp\t%p\n", viewProp);
                        printf("pickList\t%p\n", pickList);

                        if (auto path = picker->GetPath())
                        {
                            printf("numOfItems\t%d\n", path->GetNumberOfItems());
                            // 鼠标点击那个图元拾取的就是那个图元
                            if (auto actor = vtkActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
                            {
                                printf("picked\t%p\n", actor);
                            }
                        }
                    }
                }
            }
        }

        Superclass::OnLeftButtonUp();
        this->Interactor->Render();
    }

    void OnRightButtonUp() override
    {
        if (m_assembly)
        {
            // 从assembly中获取每个actor
            vtkNew<vtkPropCollection> collection;
            m_assembly->GetActors(collection);
            collection->InitTraversal();
            std::cout << "number of item: " << collection->GetNumberOfItems() << '\n';

            for (vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
            {
                dynamic_cast<vtkActor*>(collection->GetNextProp())->GetProperty()->SetOpacity(0.5);
            }

            this->Interactor->Render();
        }

        Superclass::OnRightButtonUp();
    }

    void SetAssembly(const vtkSmartPointer<vtkAssembly>& assembly)
    {
        m_assembly = assembly;
    }

private:
    vtkSmartPointer<vtkAssembly> m_assembly { nullptr };
};

vtkStandardNewMacro(InteractorStyle);
} // namespace

int main(int, char*[])
{
    {
        // Create a sphere
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->Update();

        vtkNew<vtkPolyDataMapper> sphereMapper;
        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
        sphereActor->SetMapper(sphereMapper);
        sphereActor->GetProperty()->SetColor(1, 0, 0);

        // Create a cube
        vtkNew<vtkCubeSource> cubeSource;
        cubeSource->SetCenter(5.0, 0.0, 0.0);
        cubeSource->Update();

        vtkNew<vtkPolyDataMapper> cubeMapper;
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
        cubeActor->SetMapper(cubeMapper);
        cubeActor->GetProperty()->SetColor(0, 1, 0);

        // Create a cone
        vtkNew<vtkConeSource> coneSource;
        coneSource->SetCenter(2.5, 0, 0);
        coneSource->Update();

        vtkNew<vtkPolyDataMapper> coneMapper;
        coneMapper->SetInputConnection(coneSource->GetOutputPort());
        coneActor->SetMapper(coneMapper);
        coneActor->GetProperty()->SetColor(0, 1, 1);
    }

    // Combine the sphere and cube into an assembly
    vtkNew<vtkAssembly> assembly;
    assembly->AddPart(sphereActor);
    assembly->AddPart(cubeActor);

    // 给整个vtkAssembly设置变换矩阵，会对vtkAssembly中所有的vtkActor生效
    // vtkNew<vtkTransform> transform;
    // transform->PostMultiply(); // this is the key line
    // transform->Translate(5.0, 0, 0);
    // assembly->SetUserTransform(transform);

    // Visualization
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(assembly);
    renderer->AddActor(coneActor);
    // renderer->AddActor(cubeActor);
    // renderer->AddActor(sphereActor);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    printf("cubeActor:\t%p\n", cubeActor.GetPointer());
    printf("sphereActor:\t%p\n", sphereActor.GetPointer());
    printf("assembly:\t%p\n", assembly.GetPointer());
    printf("coneActor:\t%p\n", coneActor.GetPointer());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("Assembly");
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<InteractorStyle> style;
    style->SetAssembly(assembly);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST40

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
        this->SelectedActor  = vtkSmartPointer<vtkActor>::New();
        this->SelectedActor->SetMapper(SelectedMapper);
    }

    virtual void OnLeftButtonUp() override
    {
        vtkNew<vtkNamedColors> colors;

        // Forward events
        vtkInteractorStyleRubberBandPick::OnLeftButtonUp();

        vtkPlanes* frustum = static_cast<vtkAreaPicker*>(this->GetInteractor()->GetPicker())->GetFrustum();

        vtkNew<vtkExtractGeometry> extractGeometry;
        extractGeometry->SetImplicitFunction(frustum);
        extractGeometry->SetInputData(this->Points);
        extractGeometry->Update();

        vtkNew<vtkVertexGlyphFilter> glyphFilter;
        glyphFilter->SetInputConnection(extractGeometry->GetOutputPort());
        glyphFilter->Update();

        vtkPolyData* selected = glyphFilter->GetOutput();
        std::cout << "Selected " << selected->GetNumberOfPoints() << " points." << std::endl;
        std::cout << "Selected " << selected->GetNumberOfCells() << " cells." << std::endl;
        this->SelectedMapper->SetInputData(selected);
        this->SelectedMapper->ScalarVisibilityOff();

        vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(selected->GetPointData()->GetArray("OriginalIds"));
        for (vtkIdType i = 0; i < ids->GetNumberOfTuples(); i++)
        {
            std::cout << "Id " << i << " : " << ids->GetValue(i) << std::endl;
        }

        this->SelectedActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
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

int main(int, char*[])
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

#ifdef TEST19

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

namespace {

// Catch mouse events
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MouseInteractorStyle* New();

    virtual void OnLeftButtonDown() override
    {
        int* pos = this->GetInteractor()->GetEventPosition();

        vtkNew<vtkCellPicker> picker;
        picker->SetTolerance(0.005); // 误差范围

        // cpu拾取
        auto picked = picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

        double* worldPosition = picker->GetPickPosition();
        std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

        // -1表示没有拾取到
        if (picker->GetCellId() != -1)
        {
        }
    }
};

vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkPolyData> polyData;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> cells;

    constexpr int col = 5;
    constexpr int row = 5;

    points->InsertNextPoint(0.0, 0.0, 0.0);
    points->InsertNextPoint(1.0, 0.0, 0.0);
    points->InsertNextPoint(2.0, 0.0, 0.0);

    points->InsertNextPoint(0.0, 1.0, 0.0);
    points->InsertNextPoint(1.0, 1.0, 0.0);
    points->InsertNextPoint(2.0, 1.0, 0.0);

    points->InsertNextPoint(0.0, 2.0, 0.0);
    points->InsertNextPoint(1.0, 2.0, 0.0);
    points->InsertNextPoint(2.0, 2.0, 0.0);

    cells->InsertNextCell({ 0, 1, 4, 3 });
    cells->InsertNextCell({ 1, 2, 5, 4 });
    cells->InsertNextCell({ 3, 4, 7, 6 });
    cells->InsertNextCell({ 4, 5, 8, 7 });

    polyData->SetPoints(points);
    polyData->SetPolys(cells);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    vtkNew<vtkActor> actor;
    actor->GetProperty()->SetColor(0, 1, 0);
    actor->GetProperty()->EdgeVisibilityOn();
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CellPicking");
    renderWindow->SetSize(800, 600);

    vtkNew<MouseInteractorStyle> style;
    style->SetDefaultRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);
    renderWindowInteractor->Initialize();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST19

#ifdef TEST35

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkCellPicker.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

#include <string>

class DesignInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static DesignInteractorStyle* New()
    {
        return new DesignInteractorStyle;
    }

    vtkTypeMacro(DesignInteractorStyle, vtkInteractorStyleTrackballCamera);

    DesignInteractorStyle()
    {
        sphereActor = vtkSmartPointer<vtkActor>::New();
    }

    virtual void OnLeftButtonDown()
    {
        if (!this->Interactor)
        {
            return;
        }

        Press         = 1;
        int PressFlag = 1;

        int* pEvtPos = this->Interactor->GetEventPosition();
        this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

        vtkNew<vtkCellPicker> vtkCurPicker;
        this->Interactor->SetPicker(vtkCurPicker);
        this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
        double Position[3];
        vtkCurPicker->GetPickPosition(Position);
        int TriID = vtkCurPicker->GetCellId();

        m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
        m_polyData->GetCellData()->Modified();
        m_polyData->GetCellData()->GetScalars()->Modified();

        this->Interactor->Render();
    }

    virtual void OnLeftButtonUp()
    {
    }

    virtual void OnRightButtonDown()
    {
        this->StartRotate();
    } // 避免vtk的GrabFocus接口占用交互命令

    virtual void OnRightButtonUp()
    {
        this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
    }

    virtual void OnMouseMove()
    {
        if (!this->Interactor)
        {
            return;
        }
        if (EditMode == 0)
        {

            int* pEvtPos = this->Interactor->GetEventPosition();
            this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

            vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
            this->Interactor->SetPicker(vtkCurPicker);
            this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
            double Position[3];
            vtkCurPicker->GetPickPosition(Position);
            int TriID = vtkCurPicker->GetCellId();
            if (TriID == -1)
                return;

            vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
            sphere->SetCenter(Position);
            sphere->SetRadius(Radius);
            sphere->SetPhiResolution(36);
            sphere->SetThetaResolution(36);
            sphere->Update();

            vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            spheremapper->SetInputConnection(sphere->GetOutputPort());
            sphereActor->SetMapper(spheremapper);
            sphereActor->GetProperty()->SetOpacity(0.2);
            sphereActor->GetProperty()->SetColor(lut->GetTableValue(PressFlag));
            sphereActor->PickableOff();
            this->CurrentRenderer->AddActor(sphereActor);
            this->Interactor->Render();

            if (Press == 0)
                return;

            BFS(Position, TriID);

            m_polyData->GetCellData()->Modified();
            m_polyData->GetCellData()->GetScalars()->Modified();
            this->Interactor->Render();
        }
        if (EditMode == 1)
        {
            int* pEvtPos = this->Interactor->GetEventPosition();
            this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

            vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
            this->Interactor->SetPicker(vtkCurPicker);
            this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
            int TriID = vtkCurPicker->GetCellId();
            if (TriID == -1)
                return;
            if (Press == 0)
                return;
            m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
            m_polyData->GetCellData()->GetScalars()->Modified();
            this->Interactor->Render();
        }
    }

    virtual void OnMouseWheelForward()
    {
        if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey())
            this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
    }

    virtual void OnMouseWheelBackward()
    {
        if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey())
            this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
    }

    void SetPolyData(vtkSmartPointer<vtkPolyData> _polyData)
    {
        m_polyData = _polyData;
    }

    void SetLookUpTable(vtkSmartPointer<vtkLookupTable> _lut)
    {
        lut = _lut;
    }

private:
    void BFS(double* Position, int TriID)
    {
        if (CellInSphere(Position, TriID) == 0)
            return;
        if (static_cast<int>(m_polyData->GetCellData()->GetScalars()->GetTuple1(TriID)) == PressFlag)
            return;
        m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
        auto id0 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(0);
        auto id1 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(1);
        auto id2 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(2);
        vtkNew<vtkIdList> idlist0;
        m_polyData->GetPointCells(id0, idlist0);
        vtkNew<vtkIdList> idlist1;
        m_polyData->GetPointCells(id1, idlist1);
        vtkNew<vtkIdList> idlist2;
        m_polyData->GetPointCells(id2, idlist2);
        for (int i = 0; i < idlist0->GetNumberOfIds(); i++)
            BFS(Position, idlist0->GetId(i));
        for (int i = 0; i < idlist1->GetNumberOfIds(); i++)
            BFS(Position, idlist1->GetId(i));
        for (int i = 0; i < idlist2->GetNumberOfIds(); i++)
            BFS(Position, idlist2->GetId(i));
        return;
    }

    bool CellInSphere(double* Position, int TriID)
    {
        auto pt0 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(0);
        auto pt1 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(1);
        auto pt2 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(2);
        // auto f0 = vtkMath::Distance2BetweenPoints(Position, pt0) < Radius ? 1 : 0;
        if (vtkMath::Distance2BetweenPoints(Position, pt0) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt1) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt2) < Radius)
            return 1;
        // auto f1 = vtkMath::Distance2BetweenPoints(Position, pt1) > Radius ? 0 : 1;
        // auto f2 = vtkMath::Distance2BetweenPoints(Position, pt2) > Radius ? 0 : 1;
        return 0;
    }

private:
    vtkSmartPointer<vtkPolyData> m_polyData { nullptr };
    vtkSmartPointer<vtkActor> sphereActor { nullptr };
    vtkSmartPointer<vtkLookupTable> lut;
    int Press { 0 };
    int EditMode { 0 };
    int PressFlag { 0 };
    double Radius = 2.5;
};

int main()
{
    std::string inputFileName = "resource/test2.stl";

    vtkNew<vtkSTLReader> STLReader;
    STLReader->SetFileName(inputFileName.c_str());
    STLReader->Update();

    auto polydata = STLReader->GetOutput();

    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputData(polydata);
    featureEdges->BoundaryEdgesOff();
    featureEdges->FeatureEdgesOn();
    featureEdges->SetFeatureAngle(20);
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOff();
    featureEdges->Update();

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(1, 0, 0);
    edgeActor->GetProperty()->SetLineWidth(3.0);
    edgeActor->GetProperty()->SetRenderLinesAsTubes(0.5);
    edgeActor->PickableOff();

    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(20);
    lut->Build();

    double white[4] = { 1.0, 1.0, 1.0, 1.0 };
    lut->SetTableValue(0, white);
    vtkNew<vtkFloatArray> cellData;
    for (int i = 0; i < polydata->GetNumberOfCells(); i++)
        cellData->InsertTuple1(i, 0);
    polydata->GetCellData()->SetScalars(cellData);
    polydata->GetCellData()->GetScalars()->Print(std::cout);
    polydata->BuildLinks();

    vtkNew<vtkPolyDataMapper> polydataMapper;
    polydataMapper->SetInputData(polydata);
    polydataMapper->SetScalarRange(0, 19);
    polydataMapper->SetLookupTable(lut);
    polydataMapper->Update();

    vtkNew<vtkActor> polydataActor;
    polydataActor->GetProperty()->EdgeVisibilityOff();
    polydataActor->SetMapper(polydataMapper);
    polydataActor->GetProperty()->SetOpacity(1);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->AddActor(edgeActor);
    renderer->AddActor(polydataActor);

    vtkNew<vtkRenderWindow> window;
    window->SetSize(800, 600);
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> interactor;
    vtkNew<DesignInteractorStyle> vtkStyle;
    interactor->SetInteractorStyle(vtkStyle);
    interactor->SetRenderWindow(window);

    // zxk::initCallbackCommand();

    // vtkInter->AddObserver(vtkCommand::LeftButtonPressEvent, LeftButtonPressCallback);
    // vtkInter->AddObserver(vtkCommand::LeftButtonReleaseEvent, LeftButtonReleaseCallback);
    // vtkInter->AddObserver(vtkCommand::MiddleButtonPressEvent, MiddleButtonPressCallback);
    // vtkInter->AddObserver(vtkCommand::MiddleButtonReleaseEvent, MiddleButtonReleaseCallback);
    // vtkInter->AddObserver(vtkCommand::RightButtonPressEvent, RightButtonPressCallback);
    // vtkInter->AddObserver(vtkCommand::RightButtonReleaseEvent, RightButtonReleaseCallback);
    // vtkInter->AddObserver(vtkCommand::KeyPressEvent, KeyPressCallback);
    // vtkInter->AddObserver(vtkCommand::KeyReleaseEvent, KeyReleaseCallback);
    // vtkInter->AddObserver(vtkCommand::MouseMoveEvent, MouseMoveCallback);
    // vtkInter->AddObserver(vtkCommand::MouseWheelForwardEvent, MouseWheelForwardCallback);
    // vtkInter->AddObserver(vtkCommand::MouseWheelBackwardEvent, MouseWheelBackwardCallback);

    // vtkInter->AddObserver(vtkCommand::KeyPressEvent, );

    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST35

#ifdef TEST33

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPickingManager.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
vtkNew<vtkAxesActor> axes;

class MyStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBand3D);

    void OnChar() override
    {
        Superclass::OnChar();
    }

    void OnLeftButtonDown() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        std::cout << "------------------------------------------\n";
        std::cout << X << '\t' << Y << '\n';

        this->FindPokedRenderer(X, Y);

        vtkNew<vtkPropCollection> props;
        axes->GetActors(props);

        // 使用vtkPropPicker vtkCellPicker都拾取不到三个轴
        // vtkPropPicker只能拾取到整个坐标轴
        // 不能使用PickProp(X,Y)，拾取不到vtkProp
        if (auto path = this->CurrentRenderer->PickPropFrom(X, Y, props))
        {
            printf("numOfItems\t%d\t", path->GetNumberOfItems());
            if (auto actor = vtkActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
            {
                printf("picked\t%p\n", actor);
            }
        }

        Superclass::OnLeftButtonDown();
        this->Interactor->Render();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetWindowName("DisplayCoordinateAxes");
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<MyStyle> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    axes->SetTipTypeToCone();
    axes->SetShaftTypeToCylinder();
    axes->SetConeRadius(.5);
    axes->SetCylinderRadius(0.05);

    // 打印axes的信息
    printf("%p\n", axes.GetPointer());
    vtkNew<vtkPropCollection> props;
    axes->GetActors(props);
    props->InitTraversal();
    while (auto prop = props->GetNextProp())
    {
        printf("%p\n", prop);
    }

    renderer->AddActor(axes);
    renderer->ResetCamera();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST33

#ifdef TEST34

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPickingManager.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
class MyWidget : public vtkOrientationMarkerWidget
{
public:
    static MyWidget* New();
    vtkTypeMacro(MyWidget, vtkOrientationMarkerWidget);

protected:
    void OnLeftButtonDown() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        std::cout << "------------------------------------------\n";
        std::cout << X << '\t' << Y << '\n';

        // vtkOrientationMarkerWidget内部会创建一个vtkRenderer，它的图层被设置为1，即始终在最上面
        // this->CurrentRenderer是原始的renderer
        // this->Renderer才是vtkOrientationMarkerWidget内部创建的renderer
        printf("widget current render: %p\n", this->CurrentRenderer);
        printf("widget render: %p\n", this->Renderer);

        vtkNew<vtkPropCollection> props;
        this->GetOrientationMarker()->GetActors(props);

        if (auto path = this->Renderer->PickPropFrom(X, Y, props))
        {
            printf("numOfItems\t%d\t", path->GetNumberOfItems());
            if (auto actor = vtkActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
            {
                printf("picked\t%p\n", actor);
            }
        }

        Superclass::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MyWidget);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1.0);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(sphereSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->SetLayer(0);
    renderer->AddActor(actor);
    printf("default render: %p\n", renderer.GetPointer());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetNumberOfLayers(2);
    renderWindow->SetWindowName("DisplayCoordinateAxes");
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkInteractorStyleRubberBand3D> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    vtkNew<vtkAxesActor> axes;
    printf("exes: %p\n", axes.GetPointer());

    vtkNew<vtkPropCollection> props;
    axes->GetActors(props);
    props->InitTraversal();
    while (auto prop = props->GetNextProp())
    {
        // 打印坐标轴的三条轴和三个圆锥
        printf("axes: %p\n", prop);
    }

    vtkNew<MyWidget> widget;
    widget->SetOutlineColor(0, 0, 1);
    widget->SetOrientationMarker(axes);
    widget->SetInteractor(renderWindowInteractor);
    widget->SetViewport(0.0, 0.0, 0.4, 0.4);
    widget->SetEnabled(1);
    widget->InteractiveOn();

    renderer->GetActiveCamera()->Azimuth(50);
    renderer->GetActiveCamera()->Elevation(-30);
    renderer->ResetCamera();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST34

#ifdef TEST36

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPickingManager.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {
class MyWidget : public vtkOrientationMarkerWidget
{
public:
    static MyWidget* New();
    vtkTypeMacro(MyWidget, vtkOrientationMarkerWidget);

    // 鼠标是否处于widget内部
    bool GetPosInWidget(int X, int Y) const
    {
        double vp[4];
        this->Renderer->GetViewport(vp);

        // compute display bounds of the widget to see if we are inside or outside
        this->Renderer->NormalizedDisplayToDisplay(vp[0], vp[1]);
        this->Renderer->NormalizedDisplayToDisplay(vp[2], vp[3]);

        int pos1[2] = { static_cast<int>(vp[0]), static_cast<int>(vp[1]) };
        int pos2[2] = { static_cast<int>(vp[2]), static_cast<int>(vp[3]) };

        if (X < (pos1[0] - this->Tolerance) || (pos2[0] + this->Tolerance) < X || Y < (pos1[1] - this->Tolerance) || (pos2[1] + this->Tolerance) < Y)
        {
            return false;
        }

        return true;
    }

    // 获取widget的中心
    std::array<double, 2> GetCenter() const
    {
        auto center = this->Renderer->GetCenter();
        return { center[0], center[1] };
    }

    // 拾取坐标轴
    bool PickAxis(int X, int Y)
    {
        vtkNew<vtkPropCollection> props;
        this->GetOrientationMarker()->GetActors(props);

        if (auto path = this->Renderer->PickPropFrom(X, Y, props))
        {
            printf("numOfItems\t%d\t", path->GetNumberOfItems());
            if (auto actor = vtkActor::SafeDownCast(path->GetLastNode()->GetViewProp()))
            {
                printf("picked\t%p\n", actor);
                return true;
            }
        }

        return false;
    }

protected:
    MyWidget()
    {
        this->SetOutlineColor(1, 0, 0);
        this->OutlineActor->VisibilityOn();
    }

    // 屏蔽鼠标左键对widget的响应
    void OnLeftButtonDown() override
    {
        this->Interactor->Render();
    }

    virtual void OnLeftButtonUp() override
    {
        this->Interactor->Render();
    }

    virtual void OnMouseMove() override
    {
        this->Interactor->Render();
    }
};

vtkStandardNewMacro(MyWidget);

class MyStyle : public vtkInteractorStyleRubberBand3D
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBand3D);

    void SetWidget(const vtkSmartPointer<MyWidget>& w)
    {
        m_widget = w;
    }

protected:
    MyStyle()
    {
        // 开启定时器
        // 鼠标左键按下旋转的时候通过定时器调用
        this->UseTimers = 1;
    }

    void OnLeftButtonDown() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        if (m_widget && m_widget->GetPosInWidget(X, Y))
        {
            if (m_widget->PickAxis(X, Y))
            {
                // do some thing
            }
            else
            {
                // 鼠标处于坐标轴所在区域则开始旋转
                MyRotateAxesStart();
            }
            this->Interactor->Render();
            return;
        }
        else
        {
            Superclass::OnLeftButtonDown();
        }
    }

    void OnLeftButtonUp() override
    {
        MyRotateAxesStop();
        Superclass::OnLeftButtonUp();
    }

    void Rotate() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        // 鼠标移出坐标轴区域则停止旋转
        if (!m_widget || !m_widget->GetPosInWidget(X, Y))
        {
            std::cout << "return\n";
            return;
        }

        if (this->CurrentRenderer == nullptr)
        {
            std::cout << "nullptr\n";
            return;
        }

        std::cout << "rotate\n";

        vtkRenderWindowInteractor* rwi = this->Interactor;

        auto center = m_widget->GetCenter();

        double dx = rwi->GetEventPosition()[0] - center[0];
        double dy = rwi->GetEventPosition()[1] - center[1];

        double* vp      = this->CurrentRenderer->GetViewport();
        const int* size = rwi->GetSize();

        double delta_elevation = -20.0 / ((vp[3] - vp[1]) * size[1]);
        double delta_azimuth   = -20.0 / ((vp[2] - vp[0]) * size[0]);

        double rxf = dx * delta_azimuth;
        double ryf = dy * delta_elevation;

        vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
        camera->Azimuth(rxf);
        camera->Elevation(ryf);
        camera->OrthogonalizeViewUp();

        if (this->AutoAdjustCameraClippingRange)
        {
            this->CurrentRenderer->ResetCameraClippingRange();
        }

        if (rwi->GetLightFollowCamera())
        {
            this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
        }

        rwi->Render();
    }

private:
    void MyRotateAxesStart()
    {
        auto pos = this->Interactor->GetEventPosition();
        this->FindPokedRenderer(pos[0], pos[1]);
        this->StartRotate();
    }

    void MyRotateAxesStop()
    {
        this->EndRotate();
    }

private:
    vtkSmartPointer<MyWidget> m_widget { nullptr };
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetCenter(0.0, 0.0, 0.0);
    sphereSource->SetRadius(1.0);
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(sphereSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(.1, .2, .3);
    renderer->SetLayer(0);
    renderer->AddActor(actor);
    printf("default render: %p\n", renderer.GetPointer());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetNumberOfLayers(2);
    renderWindow->SetWindowName("DisplayCoordinateAxes");
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<MyStyle> style;

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(style);

    vtkNew<vtkAxesActor> axes;
    printf("exes: %p\n", axes.GetPointer());

    vtkNew<vtkPropCollection> props;
    axes->GetActors(props);
    props->InitTraversal();
    while (auto prop = props->GetNextProp())
    {
        // 打印坐标轴的三条轴和三个圆锥
        printf("axes: %p\n", prop);
    }

    vtkNew<MyWidget> widget;
    widget->SetOutlineColor(0, 0, 1);
    widget->SetOrientationMarker(axes);
    widget->SetInteractor(renderWindowInteractor);
    widget->SetViewport(0.0, 0.0, 0.4, 0.4);
    widget->SetEnabled(1);
    widget->InteractiveOn();

    style->SetWidget(widget);

    renderer->GetActiveCamera()->Azimuth(50);
    renderer->GetActiveCamera()->Elevation(-30);
    renderer->ResetCamera();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST36

#ifdef TEST37

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

/*
 * https://blog.csdn.net/qq_37366618/article/details/127315624
 * 定义命令：
 * 使用vtkInteractorStyleRubberBandPick交互方式时，会自动触发vtkAreaPicker
 * 交互时框选的信息会存储在vtkAreaPicker中
 * 将vtkAreaPicker的结束拾取事件绑定vtkPickerCallback
 * 类似QT中信号与槽的机制
 * vtkAreaPicker触发结束拾取信号，然后执行vtkPickerCallback中的Execute函数
 */
class vtkPickerCallback : public vtkCommand
{
public:
    //@brief 定义New函数(固定格式)
    static vtkPickerCallback* New()
    {
        return new vtkPickerCallback;
    }

    //@brief 定义Execute函数(vtkCommand中的Execute为纯虚函数，必须要实现)
    virtual void Execute(vtkObject* caller, unsigned long, void*);
    void SetPolyData(vtkPolyData* input);
    void SetRenderer(vtkRenderer* input);

private:
    vtkPolyData* polyData; // 需要处理的几何数据
    vtkRenderer* renderer; // 需要调用的渲染器

    vtkDataSetMapper* mapper; // 用于显示框选面片的mapper
    vtkActor* actor;          // 用于显示框选面片的actor
};

int main()
{
    //****************创建球体*****************
    vtkSphereSource* sphere = vtkSphereSource::New();
    sphere->SetThetaResolution(18);
    sphere->SetPhiResolution(18);
    sphere->SetRadius(10);
    sphere->SetCenter(0, 0, 0);
    sphere->Update();

    //****************创建Mapper***************
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(sphere->GetOutputPort());
    // 换成此语句效果一致：mapper->SetInputData(sphere->GetOutput());

    //****************创建Actor****************
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);

    //****************创建渲染器***************
    vtkRenderer* renderer = vtkRenderer::New();
    renderer->AddActor(actor);

    //****************创建渲染窗口*************
    vtkRenderWindow* renderWindow = vtkRenderWindow::New();
    renderWindow->AddRenderer(renderer);

    //****************创建交互器***************
    vtkRenderWindowInteractor* renderWindowInteractor = vtkRenderWindowInteractor::New();

    //****************创建交互方式*************
    vtkInteractorStyleRubberBandPick* interactorStyle = vtkInteractorStyleRubberBandPick::New();

    //****************创建拾取回调函数*********
    vtkPickerCallback* callback = vtkPickerCallback::New();
    callback->SetPolyData(sphere->GetOutput());
    callback->SetRenderer(renderer);

    //****************创建区域拾取器***********
    vtkAreaPicker* areaPicker = vtkAreaPicker::New();
    areaPicker->AddObserver(vtkCommand::EndPickEvent, callback); // 为“结束拾取”事件绑定“拾取回调函数”

    renderWindowInteractor->SetRenderWindow(renderWindow);       // 为交互器设置渲染窗口
    renderWindowInteractor->SetInteractorStyle(interactorStyle); // 为交互器设置交互方式
    renderWindowInteractor->SetPicker(areaPicker);               // 为交互器设置拾取器

    renderWindowInteractor->Start();

    return 0;
}

void vtkPickerCallback::Execute(vtkObject* caller, unsigned long, void*)
{
    // 通过反射获取调用者
    vtkAreaPicker* areaPicker = static_cast<vtkAreaPicker*>(caller);
    // 获取框选的视锥体（由六个面组成）
    vtkPlanes* frustum = areaPicker->GetFrustum();
    // 提前标记几何数据的CellId
    vtkIdFilter* idFilter = vtkIdFilter::New();
    idFilter->SetInputData(polyData);
    idFilter->SetCellIdsArrayName("OriginalCellId");
    idFilter->Update();
    // 提取视锥体内的模型
    vtkExtractPolyDataGeometry* extract = vtkExtractPolyDataGeometry::New();
    extract->SetInputConnection(idFilter->GetOutputPort());
    extract->SetImplicitFunction(frustum);
    extract->Update();
    if (!extract->GetOutput()->GetPolys())
    {
        return;
    }
    // 创建面片定位器
    vtkCellLocator* locator = vtkCellLocator::New();
    locator->SetDataSet(extract->GetOutput());
    locator->BuildLocator();
    //----------利用光线投射的方法寻找更靠近摄像机的面片------------
    double rayStart[3];     // 光线起点坐标：设置为摄像机位置
    double rayDirection[3]; // 光线方向向量：设置为框选数据包围盒的中心
    renderer->GetActiveCamera()->GetPosition(rayStart);
    extract->GetOutput()->GetCenter(rayDirection);
    double xyz[3];
    double t;
    double pcoords[3];
    int subId;
    vtkIdType cellId = -1; // 记录光线击中的面片Id号
    locator->IntersectWithLine(rayStart, rayDirection, 0.0001, t, xyz, pcoords, subId, cellId);
    //-----------利用找到的面片获取相连的面
    vtkPolyDataConnectivityFilter* connectivity = vtkPolyDataConnectivityFilter::New();
    connectivity->SetInputConnection(extract->GetOutputPort());
    connectivity->SetExtractionModeToCellSeededRegions();
    connectivity->InitializeSeedList();
    connectivity->AddSeed(cellId);
    connectivity->Update();

    //--------删除框选面片----------
    // 提取框选面片的原始面片ID
    vtkIdTypeArray* ids = dynamic_cast<vtkIdTypeArray*>(connectivity->GetOutput()->GetCellData()->GetArray("OriginalCellId"));
    // 要删除面片前必须先执行建立拓扑链接
    polyData->BuildLinks();
    if (!ids)
    {
        return;
    }
    for (int i = 0; i < ids->GetNumberOfValues(); i++)
    {
        vtkIdType id = ids->GetValue(i);
        polyData->DeleteCell(id);
    }
    // 提交删除面片操作
    polyData->RemoveDeletedCells();
    polyData->Modified();
    // 刷新渲染器
    renderer->Render();
}

void vtkPickerCallback::SetPolyData(vtkPolyData* input)
{
    polyData = input;
}

void vtkPickerCallback::SetRenderer(vtkRenderer* input)
{
    renderer = input;
    // 初始化用于显示框选面片的mapper和actor
    mapper = vtkDataSetMapper::New();
    actor  = vtkActor::New();
    actor->SetMapper(mapper);
}

#endif // TEST37
