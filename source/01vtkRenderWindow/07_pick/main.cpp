/*
* 27.框选拾取 vtk拾取相关的类 https://blog.csdn.net/lovelanjuan/article/details/7301773
* 28.actor拾取 vtkPropPicker 基于硬件拾取
* 29.vtk拾取汇总，框选拾取，范围拾取，点拾取，单元拾取
* 30.范围拾取 vtkAreaPicker 点到actor的最短距离 vtkCellLocator
* 62 vtkInteractorStyleRubberBandPick 框选拾取
* 19 单元拾取 cellpick
* 35 拾取并标记
*  多图层拾取
*/

#define TEST27

#ifdef TEST27

// https://kitware.github.io/vtk-examples/site/Cxx/Picking/HighlightSelectedPoints/

// 按下R开始框选拾取

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

#ifdef TEST40

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAxesActor.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

int main(int, char* [])
{
    // Create a sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
    vtkNew<vtkActor> sphereActor;
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(1, 0, 0);

    // Create a cube
    vtkNew<vtkCubeSource> cubeSource;
    cubeSource->SetCenter(5.0, 0.0, 0.0);
    cubeSource->Update();

    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cubeSource->GetOutputPort());
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);


    // Combine the sphere and cube into an assembly
    vtkNew<vtkAssembly> assembly;
    assembly->AddPart(sphereActor);
    assembly->AddPart(cubeActor);

    // Apply a transform to the whole assembly
    //vtkNew<vtkTransform> transform;
    //transform->PostMultiply(); // this is the key line
    //transform->Translate(5.0, 0, 0);

    //assembly->SetUserTransform(transform);

    // Extract each actor from the assembly and change its opacity
    //vtkNew<vtkPropCollection> collection;

    //assembly->GetActors(collection);
    //collection->InitTraversal();
    //for (vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
    //{
    //    dynamic_cast<vtkActor*>(collection->GetNextProp())
    //        ->GetProperty()
    //        ->SetOpacity(0.5);
    //}

    // Visualization
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(assembly);
    renderer->SetBackground(.1, .2, .3);

    renderer->ResetCamera();
    renderWindow->SetWindowName("Assembly");
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

#ifdef TEST19

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractSelection.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
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
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkUnstructuredGrid.h>

namespace {

    // Catch mouse events
    class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static MouseInteractorStyle* New();

        MouseInteractorStyle()
        {
            selectedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
            selectedActor = vtkSmartPointer<vtkActor>::New();
        }

        virtual void OnLeftButtonDown() override
        {
            vtkNew<vtkNamedColors> colors;

            // Get the location of the click (in window coordinates)
            int* pos = this->GetInteractor()->GetEventPosition();

            vtkNew<vtkCellPicker> picker;
            picker->SetTolerance(0.0005);

            // Pick from this location.
            picker->Pick(pos[0], pos[1], 0, this->GetDefaultRenderer());

            double* worldPosition = picker->GetPickPosition();
            std::cout << "Cell id is: " << picker->GetCellId() << std::endl;

            if (picker->GetCellId() != -1)
            {

                std::cout << "Pick position is: (" << worldPosition[0] << ", "
                    << worldPosition[1] << ", " << worldPosition[2] << ")" << endl;

                vtkNew<vtkIdTypeArray> ids;
                ids->SetNumberOfComponents(1);
                ids->InsertNextValue(picker->GetCellId());

                vtkNew<vtkSelectionNode> selectionNode;
                selectionNode->SetFieldType(vtkSelectionNode::CELL);
                selectionNode->SetContentType(vtkSelectionNode::INDICES);
                selectionNode->SetSelectionList(ids);

                vtkNew<vtkSelection> selection;
                selection->AddNode(selectionNode);

                vtkNew<vtkExtractSelection> extractSelection;
                extractSelection->SetInputData(0, this->Data);
                extractSelection->SetInputData(1, selection);
                extractSelection->Update();

                // In selection
                vtkNew<vtkUnstructuredGrid> selected;
                selected->ShallowCopy(extractSelection->GetOutput());

                std::cout << "Number of points in the selection: "
                    << selected->GetNumberOfPoints() << std::endl;
                std::cout << "Number of cells in the selection : "
                    << selected->GetNumberOfCells() << std::endl;
                selectedMapper->SetInputData(selected);
                selectedActor->SetMapper(selectedMapper);
                selectedActor->GetProperty()->EdgeVisibilityOn();
                selectedActor->GetProperty()->SetColor(0., 1., 0.);

                selectedActor->GetProperty()->SetLineWidth(3);

                this->Interactor->GetRenderWindow()
                    ->GetRenderers()
                    ->GetFirstRenderer()
                    ->AddActor(selectedActor);
            }
            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        }

        vtkSmartPointer<vtkPolyData> Data;
        vtkSmartPointer<vtkDataSetMapper> selectedMapper;
        vtkSmartPointer<vtkActor> selectedActor;
    };

    vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char* [])
{
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkPlaneSource> planeSource;
    planeSource->Update();

    vtkNew<vtkTriangleFilter> triangleFilter;
    triangleFilter->SetInputConnection(planeSource->GetOutputPort());
    triangleFilter->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(triangleFilter->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("CellPicking");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    // Set the custom stype to use for interaction.
    vtkNew<MouseInteractorStyle> style;
    style->SetDefaultRenderer(renderer);
    style->Data = triangleFilter->GetOutput();

    renderWindowInteractor->SetInteractorStyle(style);

    renderer->AddActor(actor);
    renderer->ResetCamera();

    renderer->SetBackground(colors->GetColor3d("PaleTurquoise").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST19


#ifdef TEST35

#include <vtkSmartPointer.h>
#include <vtkFeatureEdges.h>
#include <vtkRenderWindow.h>
#include <vtkSTLReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCellPicker.h>
#include <vtkSphereSource.h>

#include <string>



class DesignInteractorStyle :
    public vtkInteractorStyleTrackballCamera
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

    virtual void OnLeftButtonDown() {
        if (!this->Interactor)
        {
            return;
        }

        Press = 1;
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
    virtual void OnLeftButtonUp() {}
    virtual void OnRightButtonDown() { this->StartRotate(); } // 避免vtk的GrabFocus接口占用交互命令
    virtual void OnRightButtonUp() { this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp(); }

    virtual void OnMouseMove() {
        if (!this->Interactor)
        {
            return;
        }
        if (EditMode == 0) {

            int* pEvtPos = this->Interactor->GetEventPosition();
            this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

            vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
            this->Interactor->SetPicker(vtkCurPicker);
            this->Interactor->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, this->CurrentRenderer);
            double Position[3];
            vtkCurPicker->GetPickPosition(Position);
            int TriID = vtkCurPicker->GetCellId();
            if (TriID == -1) return;

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

            if (Press == 0)return;

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
            if (TriID == -1) return;
            if (Press == 0)return;
            m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
            m_polyData->GetCellData()->GetScalars()->Modified();
            this->Interactor->Render();
        }
    }
    virtual void OnMouseWheelForward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward(); }
    virtual void OnMouseWheelBackward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward(); }

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
        if (CellInSphere(Position, TriID) == 0) return;
        if (static_cast<int>(m_polyData->GetCellData()->GetScalars()->GetTuple1(TriID)) == PressFlag) return;
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
        //auto f0 = vtkMath::Distance2BetweenPoints(Position, pt0) < Radius ? 1 : 0;
        if (vtkMath::Distance2BetweenPoints(Position, pt0) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt1) < Radius)
            return 1;
        if (vtkMath::Distance2BetweenPoints(Position, pt2) < Radius)
            return 1;
        //auto f1 = vtkMath::Distance2BetweenPoints(Position, pt1) > Radius ? 0 : 1;
        //auto f2 = vtkMath::Distance2BetweenPoints(Position, pt2) > Radius ? 0 : 1;
        return 0;
    }
private:
    vtkSmartPointer<vtkPolyData> m_polyData{ nullptr };
    vtkSmartPointer<vtkActor> sphereActor{ nullptr };
    vtkSmartPointer<vtkLookupTable> lut;
    int Press{ 0 };
    int EditMode{ 0 };
    int PressFlag{ 0 };
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

    double white[4] = { 1.0,1.0,1.0,1.0 };
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

    //zxk::initCallbackCommand();


    //vtkInter->AddObserver(vtkCommand::LeftButtonPressEvent, LeftButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::LeftButtonReleaseEvent, LeftButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::MiddleButtonPressEvent, MiddleButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::MiddleButtonReleaseEvent, MiddleButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::RightButtonPressEvent, RightButtonPressCallback);
    //vtkInter->AddObserver(vtkCommand::RightButtonReleaseEvent, RightButtonReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::KeyPressEvent, KeyPressCallback);
    //vtkInter->AddObserver(vtkCommand::KeyReleaseEvent, KeyReleaseCallback);
    //vtkInter->AddObserver(vtkCommand::MouseMoveEvent, MouseMoveCallback);
    //vtkInter->AddObserver(vtkCommand::MouseWheelForwardEvent, MouseWheelForwardCallback);
    //vtkInter->AddObserver(vtkCommand::MouseWheelBackwardEvent, MouseWheelBackwardCallback);

    //vtkInter->AddObserver(vtkCommand::KeyPressEvent, );


    window->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}
#endif // TEST35