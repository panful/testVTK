/**
 * 01. vtkCellPicker 拾取图元的单元，CPU拾取
 * 02. vtkAreaPicker 框选拾取，CPU拾取，通过构建一个视锥体，然后用这个视锥体对需要拾取的几何体进行提取操作
 * 03. vtkPropPicker 拾取vtkProp，GPU拾取
 * 04. vtkPointPicker 拾取图元的顶点坐标，CPU拾取
 * 05. vtkWorldPointPicker 仅仅将输入的屏幕坐标转换为世界坐标，并不拾取任何图元
 * 06. vtkRenderedAreaPicker GPU拾取，和vtkAreaPicker相比，不能拾取被遮挡的vtkProp
 * 07. vtkHardwareSelector GPU拾取顶点、单元等
 *
 * 21. 拾取坐标轴vtkAxesActor的某一个轴  vtkRenderer->PickPropFrom()
 * 22. 拾取 vtkOrientationMarkerWidget 中的 vtkAxesActor，多图层拾取
 *
 * 31. 拾取vtkAssembly的指定Actor
 *
 * 41. 拾取事件回调函数 vtkCommand::EndPickEvent
 * 42. 高亮拾取到的图元 HighlightProp
 *
 * vtkScenePicker  vtkHardwarePicker
 * vtkResliceCursorPicker vtkVolumePicker vtkZSpaceHardwarePicker vtkVRHardwarePicker
 *
 * 多图层拾取 01_03_TEST45
 */

#define TEST42

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

namespace {

class Style : public vtkInteractorStyleTrackballCamera
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        if (this->CurrentRenderer)
        {
            int pos[2] {};
            this->GetInteractor()->GetEventPosition(pos);

            std::cout << "------------------------------------------------------\n";
            std::cout << "event pos: " << pos[0] << '\t' << pos[1] << '\n';

            vtkNew<vtkCellPicker> picker; // cpu拾取
            picker->SetTolerance(0.05);   // 误差范围

            if (0 != picker->Pick(pos[0], pos[1], 0, this->CurrentRenderer))
            {
                auto cellID   = picker->GetCellId();          // 单元ID，-1表示没有拾取到(拾取到的第一个图元)
                auto position = picker->GetPickPosition();    // 拾取点与拾取到的第一个图元交点
                auto points   = picker->GetPickedPositions(); // 拾取点与拾取到的所有图元交点
                auto cellIJK  = picker->GetCellIJK();         // 只对体网格（image/volumes）有效
                auto pointID  = picker->GetPointId();         // 在拾取到的单元上，距离拾取点最近的顶点ID
                auto selPoint = picker->GetSelectionPoint();  // 拾取点的屏幕坐标
                auto prop3Ds  = picker->GetProp3Ds();         // 拾取到的所有vtkProp
                auto actor    = picker->GetActor();           // 拾取到的第一个图元

                std::cout << "cell id: " << cellID << "\npick position: " << position[0] << '\t' << position[1] << '\t' << position[2]
                          << "\npoint id: " << pointID << "\nselection point: " << selPoint[0] << '\t' << selPoint[1] << '\t' << selPoint[2]
                          << "\nactor: " << actor << '\n';

                for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
                {
                    double pt[3] {};
                    points->GetPoint(i, pt);
                    std::cout << "picked position " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
                }

                for (prop3Ds->InitTraversal(); auto prop = prop3Ds->GetNextProp3D();)
                {
                    std::cout << "prop3Ds:" << prop << '\n';
                }
            }
        }

        Superclass::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(Style);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkRenderer> renderer;

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0, 0.0, 1.0);
        points->InsertNextPoint(1.0, 0.0, 1.0);
        points->InsertNextPoint(2.0, 0.0, 1.0);

        points->InsertNextPoint(0.0, 1.0, 1.0);
        points->InsertNextPoint(1.0, 1.0, 1.0);
        points->InsertNextPoint(2.0, 1.0, 1.0);

        points->InsertNextPoint(0.0, 2.0, 1.0);
        points->InsertNextPoint(1.0, 2.0, 1.0);
        points->InsertNextPoint(2.0, 2.0, 1.0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 4, 3 });
        cells->InsertNextCell({ 1, 2, 5, 4 });
        cells->InsertNextCell({ 3, 4, 7, 6 });
        cells->InsertNextCell({ 4, 5, 8, 7 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->GetProperty()->SetColor(0, 1, 0);
        actor->GetProperty()->EdgeVisibilityOn();
        actor->GetProperty()->SetEdgeColor(1, 0, 0);
        actor->SetMapper(mapper);

        std::cout << "actor1\t" << actor << '\n';

        renderer->AddActor(actor);
    }

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0, 0.0, 0.0);
        points->InsertNextPoint(2.0, 0.0, 0.0);
        points->InsertNextPoint(2.0, 2.0, 0.0);
        points->InsertNextPoint(0.0, 2.0, 0.0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->GetProperty()->SetColor(0, 0, 1);
        actor->GetProperty()->EdgeVisibilityOn();
        actor->GetProperty()->SetEdgeColor(1, 0, 0);
        actor->SetMapper(mapper);

        std::cout << "actor2\t" << actor << '\n';

        renderer->AddActor(actor);
    }

    renderer->ResetCamera();
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<Style> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkObjectFactory.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

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
            if (0 != picker->AreaPick(x0, y0, x1, y1, this->CurrentRenderer))
            {
                auto planes     = picker->GetFrustum();        // 6个面构成的视锥体
                auto prop3Ds    = picker->GetProp3Ds();        // 视锥体内的所有vtkProp，即所有被拾取到的vtkProp
                auto prop3D     = picker->GetProp3D();         // 视锥体内距离相机最近的vtkProp
                auto actor      = picker->GetActor();          // 同上
                auto viewProp   = picker->GetViewProp();       // 同上
                auto clipPoints = picker->GetClipPoints();     // 8个顶点
                auto pickPos    = picker->GetPickPosition();   // 永远都是{0,0,0}
                auto selPoint   = picker->GetSelectionPoint(); // 永远都是{0,0,0}

                std::cout << "prop3Ds count\t" << prop3Ds->GetNumberOfItems() << '\n';
                prop3Ds->InitTraversal();
                while (auto prop = prop3Ds->GetNextProp3D())
                {
                    std::cout << "props\t\t" << prop << '\n';
                }
                std::cout << "prop3D\t\t" << prop3D << "\nactor\t\t" << actor << "\nviewProp\t" << viewProp << "\n";

                // for (vtkIdType i = 0; i < clipPoints->GetNumberOfPoints(); ++i)
                // {
                //     double pt[3] {};
                //     clipPoints->GetPoint(i, pt);
                //     std::cout << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
                // }
                // std::cout << "pickpos: " << pickPos[0] << '\t' << pickPos[1] << '\t' << pickPos[2] << '\n';
                // std::cout << "selPoint: " << selPoint[0] << '\t' << selPoint[1] << '\t' << selPoint[2] << '\n';
            }
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkRenderer> renderer;

    for (int i = 0; i < 10; ++i)
    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(2.0 * i, 2.0 * i, 2.0 * i);
        source->SetRadius(1.0);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());
        mapper->ScalarVisibilityOff();

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0., 1., 0.);

        std::cout << actor << '\n';

        renderer->AddActor(actor);
    }

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <map>

namespace {
std::map<vtkSmartPointer<vtkActor>, int> theActors; // actor标识

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        int clickPos[2] {};
        this->GetInteractor()->GetEventPosition(clickPos);
        std::cout << "--------------------------------------------------\n";
        std::cout << "event pos: " << clickPos[0] << '\t' << clickPos[1] << '\n';

        vtkNew<vtkPropPicker> picker; // 多个actor重叠时，拾取距离相机最近的actor
        if (0 != picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer()))
        {
            auto actor    = picker->GetActor();          // 拾取到的actor
            auto pickPos  = picker->GetPickPosition();   // 拾取点在世界坐标系的坐标
            auto selPoint = picker->GetSelectionPoint(); // 拾取点的屏幕坐标，和eventpos相同

            std::cout << "picked actor: " << theActors[actor] << "\npick position: " << pickPos[0] << '\t' << pickPos[1] << '\t' << pickPos[2]
                      << "\nselection point: " << selPoint[0] << '\t' << selPoint[1] << '\t' << selPoint[2] << '\n';
        }

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;

    constexpr int numOfActors { 3 };

    for (int i = 0; i < numOfActors; ++i)
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0 + i * 1.0, 0.0, i * 0.5);
        points->InsertNextPoint(1.0 + i * 1.0, 0.0, i * 0.5);
        points->InsertNextPoint(1.0 + i * 1.0, 1.0, i * 0.5);
        points->InsertNextPoint(0.0 + i * 1.0, 1.0, i * 0.5);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(i % 2, (i + 1) % 2, 0);

        renderer->AddActor(actor);
        theActors.try_emplace(actor, i);
    }

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    style->SetDefaultRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

        int eventPos[2] {};
        this->GetInteractor()->GetEventPosition(eventPos);

        std::cout << "-------------------------------------\n";
        std::cout << "Event Pos: " << eventPos[0] << '\t' << eventPos[1] << '\n';

        if (this->CurrentRenderer)
        {
            vtkNew<vtkPointPicker> picker;
            picker->SetTolerance(0.05); // 误差
            this->Interactor->SetPicker(picker);

            // 只有当actor的顶点处于拾取范围内时，才算拾取到，不论拾取点是否在actor上
            if (0 != picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer))
            {
                auto actor   = picker->GetActor();           // 拾取到的第一个Actor
                auto pointId = picker->GetPointId();         // 拾取到的顶点ID（第一个actor的顶点ID）
                auto pickpos = picker->GetPickPosition();    // 拾取到的vtkPolyData顶点坐标
                auto dataSet = picker->GetDataSet();         // 拾取到的vtkPolyData
                auto points  = picker->GetPickedPositions(); // 拾取点与拾取到的图元相交的点
                auto prop3Ds = picker->GetProp3Ds(); // 拾取到的所有vtkProp，可以拾取到误差范围内所有顶点，包括被遮挡的顶点

                std::cout << "actor:" << actor << "\npoint id: " << pointId << "\npick position: " << pickpos[0] << '\t' << pickpos[1] << '\t'
                          << pickpos[2] << '\n';

                for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
                {
                    double pt[3] {};
                    points->GetPoint(i, pt);
                    std::cout << "picked position " << i << " : " << pt[0] << '\t' << pt[1] << '\t' << pt[2] << '\n';
                }

                for (prop3Ds->InitTraversal(); auto prop = prop3Ds->GetNextProp3D();)
                {
                    std::cout << "prop3Ds:" << prop << '\n';
                }
            }
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
{
    vtkNew<vtkActor> actor1;
    vtkNew<vtkActor> actor2;

    std::cout << "Actor1: " << actor1.GetPointer() << "\nActor2: " << actor2.GetPointer() << '\n';

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0, 0.0, 0.0);
        points->InsertNextPoint(2.0, 0.0, 0.0);
        points->InsertNextPoint(2.0, 2.0, 0.0);
        points->InsertNextPoint(0.0, 2.0, 0.0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2 });
        cells->InsertNextCell({ 0, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        actor1->SetMapper(mapper);
        actor1->GetProperty()->SetColor(0, 1, 0);
        actor1->GetProperty()->EdgeVisibilityOn();
        actor1->GetProperty()->SetEdgeColor(1, 1, 1);
    }

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0, 0.0, 1.0);
        points->InsertNextPoint(2.0, 0.0, 1.0);
        points->InsertNextPoint(2.0, 2.0, 1.0);
        points->InsertNextPoint(0.0, 2.0, 1.0);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        actor2->SetMapper(mapper);
        actor2->GetProperty()->SetColor(1, 0, 0);
        actor2->GetProperty()->EdgeVisibilityOn();
        actor2->GetProperty()->SetEdgeColor(1, 1, 1);
    }

    //-------------------------------------------------------------
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor1);
    renderer->AddActor(actor2);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSphereSource.h>
#include <vtkWorldPointPicker.h>

namespace {
class MouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MouseInteractorStyle* New();
    vtkTypeMacro(MouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        auto eventPos = this->Interactor->GetEventPosition();
        std::cout << "------------------------------------------\nPicking pixel:\t" << eventPos[0] << '\t' << eventPos[1] << '\n';

        auto picker = vtkWorldPointPicker::SafeDownCast(this->Interactor->GetPicker());
        if (picker && this->CurrentRenderer)
        {
            // 永远返回0，因为不需要拾取任何图元，仅仅将输入的屏幕坐标转换为世界坐标
            picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer);

            auto pickPos  = picker->GetPickPosition();   // 输入的屏幕坐标转换的世界坐标
            auto selPoint = picker->GetSelectionPoint(); // 屏幕坐标

            std::cout << "pickPos\t\t" << pickPos[0] << '\t' << pickPos[1] << '\t' << pickPos[2] << '\n';
            std::cout << "selPoint\t" << selPoint[0] << '\t' << selPoint[1] << '\t' << selPoint[2] << '\n';
        }

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MouseInteractorStyle);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkWorldPointPicker> worldPointPicker;
    renderWindowInteractor->SetPicker(worldPointPicker);

    vtkNew<MouseInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST5

#ifdef TEST6

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

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

            vtkNew<vtkRenderedAreaPicker> picker;
            if (0 != picker->AreaPick(x0, y0, x1, y1, this->CurrentRenderer))
            {
                auto planes     = picker->GetFrustum();        // 6个面构成的视锥体
                auto prop3Ds    = picker->GetProp3Ds();        // 视锥体内可以看到的所有vtkProp，不包含被遮挡的vtkProp
                auto prop3D     = picker->GetProp3D();         // 视锥体内距离相机最近的vtkProp
                auto actor      = picker->GetActor();          // 同上
                auto viewProp   = picker->GetViewProp();       // 同上
                auto clipPoints = picker->GetClipPoints();     // 8个顶点
                auto pickPos    = picker->GetPickPosition();   // 视锥体（世界坐标）的中心
                auto selPoint   = picker->GetSelectionPoint(); // 拾取框（屏幕坐标）的中心

                std::cout << "prop3Ds count\t" << prop3Ds->GetNumberOfItems() << '\n';
                for (prop3Ds->InitTraversal(); auto prop = prop3Ds->GetNextProp3D();)
                {
                    std::cout << "props\t\t" << prop << '\n';
                }

                std::cout << "prop3D\t\t" << prop3D << "\nactor\t\t" << actor << "\nviewProp\t" << viewProp << "\n";
            }

            this->Interactor->Render();
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;

    constexpr int numOfActors { 3 };

    for (int i = 0; i < numOfActors; ++i)
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0.0 + i * 1.0, 0.0, i * 0.5);
        points->InsertNextPoint(1.0 + i * 1.0, 0.0, i * 0.5);
        points->InsertNextPoint(1.0 + i * 1.0, 1.0, i * 0.5);
        points->InsertNextPoint(0.0 + i * 1.0, 1.0, i * 0.5);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(i % 2, (i + 1) % 2, 0);
        std::cout << "actor " << i << " : " << actor << '\n';

        renderer->AddActor(actor);
    }

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    style->SetDefaultRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkHardwareSelector.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkInteractorStyleRubberBand3D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkRenderer.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

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

            vtkNew<vtkRenderedAreaPicker> picker; // 先用 vtkRenderedAreaPicker 快速拾取确定有图元被拾取到
            if (0 != picker->AreaPick(x0, y0, x1, y1, this->CurrentRenderer))
            {
                auto ux0 = std::min(x0, x1);
                auto uy0 = std::min(y0, y1);
                auto ux1 = std::max(x0, x1);
                auto uy1 = std::max(y0, y1);

                vtkNew<vtkHardwareSelector> selector; // 再用硬件拾取顶点（可能拾取到了图元，但并没有拾取到顶点）
                selector->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_POINTS); // 可以设置拾取顶点、单元等类型
                selector->SetRenderer(this->CurrentRenderer);
                selector->SetArea(static_cast<uint32_t>(ux0), static_cast<uint32_t>(uy0), static_cast<uint32_t>(ux1), static_cast<uint32_t>(uy1));

                vtkSelection* result     = selector->Select();         // 必须手动释放
                const vtkIdType numNodes = result->GetNumberOfNodes(); // 拾取到的图元个数
                std::cout << "Number of nodes:\t" << numNodes << '\n';

                for (vtkIdType nodeId = 0; nodeId < numNodes; ++nodeId)
                {
                    vtkSelectionNode* node = result->GetNode(nodeId);
                    vtkActor* selActor     = vtkActor::SafeDownCast(node->GetProperties()->Get(vtkSelectionNode::PROP()));
                    std::cout << "Actor\t" << selActor << '\n';

                    if (selActor)
                    {
                        if (vtkIdTypeArray* selIds = vtkArrayDownCast<vtkIdTypeArray>(node->GetSelectionList()))
                        {
                            vtkIdType numIds = selIds->GetNumberOfTuples(); // 拾取到的顶点个数，被遮挡的顶点不会被拾取
                            std::cout << "Number of tuples:\t" << numIds << "\nID:\t";

                            for (vtkIdType i = 0; i < numIds; ++i)
                            {
                                vtkIdType curId = selIds->GetValue(i); // 图元的顶点ID
                                std::cout << curId << '\t';
                            }
                            std::cout << '\n';
                        }
                    }
                }

                result->Delete();
            }

            this->Interactor->Render();
        }
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int argc, char* argv[])
{
    vtkNew<vtkRenderer> ren;

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(0, 0, 0);
        points->InsertNextPoint(1, 0, 0);
        points->InsertNextPoint(1, 1, 0);
        points->InsertNextPoint(0, 1, 0);
        points->InsertNextPoint(0, 0, 1);
        points->InsertNextPoint(1, 0, 1);
        points->InsertNextPoint(1, 1, 1);
        points->InsertNextPoint(0, 1, 1);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });
        cells->InsertNextCell({ 1, 5, 6, 2 });
        cells->InsertNextCell({ 5, 4, 7, 6 });
        cells->InsertNextCell({ 4, 0, 3, 7 });
        cells->InsertNextCell({ 3, 2, 6, 7 });
        cells->InsertNextCell({ 4, 5, 1, 0 });

        vtkNew<vtkDoubleArray> scalars;
        scalars->InsertNextValue(0);
        scalars->InsertNextValue(1);
        scalars->InsertNextValue(2);
        scalars->InsertNextValue(3);
        scalars->InsertNextValue(4);
        scalars->InsertNextValue(5);

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        polyData->GetCellData()->SetScalars(scalars);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        mapper->SetScalarRange(0, 5);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        std::cout << "actor 1 \t" << actor << '\n';
        ren->AddActor(actor);
    }

    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(2, 0, 0);
        points->InsertNextPoint(3, 0, 0);
        points->InsertNextPoint(3, 1, 0);
        points->InsertNextPoint(2, 1, 0);
        points->InsertNextPoint(2, 0, 1);
        points->InsertNextPoint(3, 0, 1);
        points->InsertNextPoint(3, 1, 1);
        points->InsertNextPoint(2, 1, 1);

        vtkNew<vtkCellArray> cells;
        cells->InsertNextCell({ 0, 1, 2, 3 });
        cells->InsertNextCell({ 1, 5, 6, 2 });
        cells->InsertNextCell({ 5, 4, 7, 6 });
        cells->InsertNextCell({ 4, 0, 3, 7 });
        cells->InsertNextCell({ 3, 2, 6, 7 });
        cells->InsertNextCell({ 4, 5, 1, 0 });

        vtkNew<vtkDoubleArray> scalars;
        scalars->InsertNextValue(0);
        scalars->InsertNextValue(1);
        scalars->InsertNextValue(2);
        scalars->InsertNextValue(3);
        scalars->InsertNextValue(4);
        scalars->InsertNextValue(5);

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(points);
        polyData->SetPolys(cells);
        polyData->GetCellData()->SetScalars(scalars);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);
        mapper->SetScalarRange(0, 5);

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);

        std::cout << "actor 2 \t" << actor << '\n';
        ren->AddActor(actor);
    }

    vtkNew<vtkRenderWindow> win;
    win->SetMultiSamples(0);
    win->AddRenderer(ren);
    win->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(win);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    win->Render();
    iren->Start();

    return 0;
}

#endif // TEST7

#ifdef TEST21

#include <map>
#include <vtkActor.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
std::map<vtkProp*, std::string> axesNames;

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        if (this->CurrentRenderer)
        {
            int eventPos[2] {};
            this->Interactor->GetEventPosition(eventPos);

            std::cout << "------------------------------------------\n";
            std::cout << "event pos: " << eventPos[0] << '\t' << eventPos[1] << '\n';

            // 使用vtkPropPicker vtkCellPicker都拾取不到三个轴，只能拾取到整个坐标轴
            // 可以使用PickPropFrom()拾取具体的某一个轴，不能使用PickProp(X,Y)，拾取不到vtkProp
            if (auto path = this->CurrentRenderer->PickPropFrom(eventPos[0], eventPos[1], m_props))
            {
                if (auto prop = path->GetLastNode()->GetViewProp())
                {
                    std::cout << axesNames[prop] << '\n';
                }
            }
        }

        Superclass::OnLeftButtonDown();
    }

    void SetAxesProps(const vtkSmartPointer<vtkPropCollection>& props)
    {
        m_props = props;
    }

private:
    vtkSmartPointer<vtkPropCollection> m_props {};
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkAxesActor> axes;
    axes->SetTipTypeToCone();
    axes->SetShaftTypeToCylinder();
    axes->SetConeRadius(.5);
    axes->SetCylinderRadius(0.05);

    vtkNew<vtkPropCollection> props;
    axes->GetActors(props);
    props->InitTraversal();

    // vtkAxesActor前三个是轴，后三个是轴上的圆锥
    axesNames.try_emplace(props->GetNextProp(), "axis X");
    axesNames.try_emplace(props->GetNextProp(), "axis Y");
    axesNames.try_emplace(props->GetNextProp(), "axis Z");
    axesNames.try_emplace(props->GetNextProp(), "cone X");
    axesNames.try_emplace(props->GetNextProp(), "cone Y");
    axesNames.try_emplace(props->GetNextProp(), "cone Z");

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(axes);
    renderer->SetBackground(.1, .2, .3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetAxesProps(props);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST21

#ifdef TEST22

#include <vtkActor.h>
#include <vtkAssemblyPath.h>
#include <vtkAxesActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
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
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
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

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST22

#ifdef TEST31

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkAssemblyPath.h>
#include <vtkCellPicker.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <map>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonUp() override
    {
        if (this->Interactor)
        {
            auto eventPos = this->Interactor->GetEventPosition();
            this->FindPokedRenderer(eventPos[0], eventPos[1]);

            if (this->CurrentRenderer)
            {
                // 使用vtkPropPicker拾取，拾取不到当前的actor
                {
                    vtkNew<vtkPropPicker> picker;
                    this->Interactor->SetPicker(picker);

                    if (0 != picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer))
                    {
                        // assembly prop3D viewProp都是同一个对象，即assembly
                        auto assembly     = picker->GetAssembly();
                        auto prop3D       = picker->GetProp3D();
                        auto viewProp     = picker->GetViewProp();
                        auto actor        = picker->GetActor();        // nullptr
                        auto propAssembly = picker->GetPropAssembly(); // nullptr

                        std::cout << "----------------------- prop pick\nactor\t\t" << actor << "\npropAssembly\t" << propAssembly << "\nassembly\t"
                                  << assembly << "\nprop3D\t\t" << prop3D << "\nviewProp\t" << viewProp << '\n';

                        if (auto&& path = picker->GetPath())
                        {
                            path->InitTraversal();
                            while (auto&& node = path->GetNextNode())
                            {
                                // 总共两个Node，第一个是assembly，第二个是第一次插入assembly的actor
                                std::cout << "path\t\t" << node->GetViewProp() << '\n';
                            }
                        }
                    }
                }

                // 使用vtkCellPicker拾取，可以拾取到指定的actor
                {
                    vtkNew<vtkCellPicker> picker;
                    this->Interactor->SetPicker(picker);

                    if (0 != picker->Pick(eventPos[0], eventPos[1], 0, this->CurrentRenderer))
                    {
                        // assembly prop3D viewProp都是同一个对象，即assembly
                        auto assembly     = picker->GetAssembly();
                        auto prop3D       = picker->GetProp3D();
                        auto viewProp     = picker->GetViewProp();
                        auto actor        = picker->GetActor();        // nullptr
                        auto propAssembly = picker->GetPropAssembly(); // nullptr

                        std::cout << "----------------------- cell pick\nactor\t\t" << actor << "\npropAssembly\t" << propAssembly << "\nassembly\t"
                                  << assembly << "\nprop3D\t\t" << prop3D << "\nviewProp\t" << viewProp << '\n';

                        // 拾取Assembly的指定Actor
                        if (auto path = picker->GetPath())
                        {
                            // 方式1，必须初始化遍历列表
                            // path->InitTraversal();
                            // while (auto&& node = path->GetNextNode())
                            // {
                            //     std::cout << "path\t\t" << node->GetViewProp() << '\n';
                            // }

                            // 方式2，直接访问最后一个Node
                            std::cout << "path\t\t" << path->GetLastNode()->GetViewProp() << '\n';
                        }
                    }
                }

                // vtkPointPicker vtkAreaPicker... 没有测试是否可以拾取指定Assembly的Actor
            }
        }

        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkAssembly> assembly;
    std::cout << "assembly\t" << assembly.Get() << '\n';

    {
        vtkNew<vtkSphereSource> source;
        source->SetCenter(0, 0, 0);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 0, 0);

        assembly->AddPart(actor);
        std::cout << "sphere\t\t" << actor.Get() << '\n';
    }

    {
        vtkNew<vtkCubeSource> source;
        source->SetCenter(2, 0, 0);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(1, 1, 0);

        assembly->AddPart(actor);
        std::cout << "cube\t\t" << actor.Get() << '\n';
    }

    {
        vtkNew<vtkConeSource> source;
        source->SetCenter(4, 0, 0);
        source->Update();

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(source->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->GetProperty()->SetColor(0, 1, 0);

        assembly->AddPart(actor);
        std::cout << "cone\t\t" << actor.Get() << '\n';
    }

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(assembly);
    renderer->SetBackground(.1, .2, .3);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<MyStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST31

#ifdef TEST41

#include <vtkActor.h>
#include <vtkAreaPicker.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

namespace {
class PickerCB : public vtkCommand
{
public:
    static PickerCB* New();
    vtkTypeMacro(PickerCB, vtkCommand);

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        if (auto picker = vtkAreaPicker::SafeDownCast(caller))
        {
            auto frustum = picker->GetFrustum();
            std::cout << "frustum: " << frustum->GetNumberOfPlanes() << '\n';
        }
    }
};

vtkStandardNewMacro(PickerCB);

class MyStyle : public vtkInteractorStyleRubberBandPick
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleRubberBandPick);

    virtual void OnLeftButtonUp() override
    {
        // 会在鼠标左键弹起时调用Pick()
        Superclass::OnLeftButtonUp();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

// 按r拾取

int main(int, char*[])
{
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // 设置拾取器
    vtkNew<vtkAreaPicker> picker;
    renderWindowInteractor->SetPicker(picker);

    // 设置拾取事件回调函数
    vtkNew<PickerCB> pickerCB;
    picker->AddObserver(vtkCommand::EndPickEvent, pickerCB);

    vtkNew<MyStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST41

#ifdef TEST42

#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>

namespace {

class Style : public vtkInteractorStyleTrackballCamera
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        if (this->CurrentRenderer)
        {
            int pos[2] {};
            this->GetInteractor()->GetEventPosition(pos);

            vtkNew<vtkPropPicker> picker;
            if (0 != picker->Pick(pos[0], pos[1], 0, this->CurrentRenderer) && picker->GetActor())
            {
                // 高亮拾取到的图元
                this->HighlightProp(picker->GetActor());
            }
            else
            {
                // 如果没有拾取到任何图元，将之前的高亮图元恢复颜色
                this->HighlightProp(nullptr);
            }
        }

        Superclass::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(Style);

} // namespace

int main(int, char*[])
{
    vtkNew<vtkRenderer> renderer;

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vtkNew<vtkSphereSource> source;
            source->SetCenter(static_cast<double>(i), static_cast<double>(j), 0.0);
            source->SetRadius(0.2);
            source->Update();

            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputData(source->GetOutput());

            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);

            renderer->AddActor(actor);
        }
    }

    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<Style> style;
    style->SetCurrentRenderer(renderer);
    style->SetPickColor(1, 0, 0); // 设置拾取标记颜色，拾取标记只能是一个包围盒边框

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST42
