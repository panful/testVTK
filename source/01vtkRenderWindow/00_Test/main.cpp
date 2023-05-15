
/*
 * 1. 引用计数
 * 2. 主动释放vtk对象

 * 4. AddObserver使用lambda，将vtk指针指向的数据置为空
 * 5. vtkSmartPointer指向的数据置空

 */

#define TEST1

#ifdef TEST1

#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <stack>
#include <vector>

class CustomWidget : public vtkOrientationMarkerWidget
{
public:
    static CustomWidget* New();
    vtkTypeMacro(CustomWidget, vtkOrientationMarkerWidget);

protected:
    CustomWidget()
    {
        std::cout << "construct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; // 1
    }
    ~CustomWidget() override
    {
        std::cout << "destruct\n";
        std::cout << "ref count:" << this->GetReferenceCount() << '\n'; // 0
    }

private:
    CustomWidget(const CustomWidget&) = delete;
    CustomWidget& operator=(const CustomWidget&) = delete;
};

vtkStandardNewMacro(CustomWidget);

int main(int, char*[])
{
    {
        std::stack<vtkSmartPointer<vtkCamera>> cameras;
        vtkNew<vtkCamera> camera1;
        cameras.emplace(camera1);
        cameras.pop();
    }

    {
        std::stack<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> widget;
        widgets.emplace(widget);
        widgets.pop();
    }

    std::cout << "11111111111\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkNew<CustomWidget> w;
        vtkSmartPointer<CustomWidget> w1 = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';

        // widgets.front()->Delete();  //崩溃
        // widgets.front()->FastDelete();//崩溃
        // w->Delete();//崩溃
        // w = nullptr;//不能赋值为nullptr

        std::cout << "==========\n";
    }

    std::cout << "2222222222222\n";
    {
        std::vector<vtkSmartPointer<CustomWidget>> widgets;
        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        widgets.emplace_back(w);
        std::cout << w->GetReferenceCount() << '\n';
        // widgets.front()->Delete();
        // widgets.front()->FastDelete();
        // w->Delete();

        // w = nullptr;
        widgets.front() = nullptr;
        w = nullptr;

        std::cout << "==========\n";
    }

    std::cout << "3333333333333\n";
    {

        vtkSmartPointer<CustomWidget> w = vtkSmartPointer<CustomWidget>::New();
        std::cout << w->GetReferenceCount() << '\n';
        // widgets.front()->Delete();
        // widgets.front()->FastDelete();
        // w->Delete();

        std::cout << "==========\n";
    }

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <vtkObject.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

class vtkMyClass : public vtkObject
{
public:
    vtkTypeMacro(vtkMyClass, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent) { }
    static vtkMyClass* New();

protected:
    vtkMyClass()
    {
        std::cerr << "constructor called\n";
    }
    ~vtkMyClass()
    {
        std::cerr << "destructor called\n";
    }
};
vtkStandardNewMacro(vtkMyClass)

    int main(int argc, char** argv)
{
    std::cerr << __LINE__ << '\n';
    vtkSmartPointer<vtkMyClass> myObject;
    std::cerr << __LINE__ << '\n';
    myObject = vtkSmartPointer<vtkMyClass>::New();
    std::cerr << __LINE__ << '\n';
    myObject = NULL; // calls destructor
    std::cerr << __LINE__ << '\n';
    return 0;
}

#endif // TEST2



#ifdef TEST4

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <iostream>

class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        std::cout << "command callback\n";
    }
};

vtkStandardNewMacro(MyCommand);

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

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // 继承vtkCommand设置回调函数
    vtkNew<MyCommand> cb1;
    iren->AddObserver(vtkCommand::RightButtonPressEvent, cb1);

    // 用lambda设置VTK的回调函数
    vtkNew<vtkCallbackCommand> cb2;
    cb2->SetCallback([](vtkObject* caller, unsigned long, void*, void*) { std::cout << "lambda\n"; });
    iren->AddObserver(vtkCommand::LeftButtonPressEvent, cb2);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <iostream>

namespace {
std::array<float, 4 * 3> vertices {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0
};

std::array<long long, 4 * 2> indices {
    0, 1,
    1, 2,
    2, 3,
    3, 0
};

class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        // 将Actor的Mapper的vtkPolyData修改，也会修改Actor
        if (!m_polyData)
        {
            vtkNew<vtkPoints> points;
            vtkNew<vtkCellArray> cells;
            vtkNew<vtkCellArray> emptyCells;

            for (size_t i = 0; i < vertices.size(); i += 3)
            {
                points->InsertNextPoint(vertices[i], vertices[i + 1], vertices[i + 2]);
            }

            cells->InsertNextCell({ 0, 1, 2, 3 });

            m_polyData->SetPoints(points);
            m_polyData->SetPolys(cells);
            m_polyData->SetLines(emptyCells); // 如果不将线段置为空，原来的polydata就包含了线段和多边形
        }

        // 将Actor置为空
        if (m_actor && m_mapper && m_polyData)
        {
            std::cout << "polyData ref count: " << m_polyData->GetReferenceCount() << '\n';
            std::cout << "mapper ref count: " << m_mapper->GetReferenceCount() << '\n';

            // 会将mapper的引用计数减一
            // 不会将polyData的引用计数减一，因为mapper还没析构
            // vtkNew<vtkPolyDataMapper> mapper;
            // m_actor->SetMapper(mapper);

            // 会将mapper等全部重新设置的actor属性的引用计数全部减一
            // 具体可以看ShallowCopy的实现
            vtkNew<vtkActor> emptyActor;
            m_actor->ShallowCopy(emptyActor);

            std::cout << "polyData ref count: " << m_polyData->GetReferenceCount() << '\n';
            std::cout << "mapper ref count: " << m_mapper->GetReferenceCount() << '\n';
        }
    }

    void SetPolyData(const vtkSmartPointer<vtkPolyData>& polydata)
    {
        m_polyData = polydata;
    }
    void SetActor(const vtkSmartPointer<vtkActor>& actor)
    {
        m_actor = actor;
    }
    void SetMapper(const vtkSmartPointer<vtkPolyDataMapper>& mapper)
    {
        m_mapper = mapper;
    }

private:
    vtkSmartPointer<vtkPolyData> m_polyData { nullptr };
    vtkSmartPointer<vtkActor> m_actor { nullptr };
    vtkSmartPointer<vtkPolyDataMapper> m_mapper { nullptr };
};

vtkStandardNewMacro(MyCommand);
}

int main(int, char*[])
{
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
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(polyData);

    // actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    // camera
    vtkNew<vtkCamera> camera;
    camera->SetPosition(1, 1, 1);   // 设置相机位置
    camera->SetFocalPoint(0, 0, 0); // 设置相机焦点

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetActiveCamera(camera);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyCommand> cb;
    cb->SetPolyData(polyData);
    cb->SetActor(actor);
    cb->SetMapper(mapper);
    iren->AddObserver(vtkCommand::LeftButtonPressEvent, cb);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST5


