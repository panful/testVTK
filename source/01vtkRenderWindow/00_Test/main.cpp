
/**
 * 1. 引用计数 主动释放vtk智能指针
 * 2. vtk智能指针的使用
 * 3. vtkNew 和 vtkSmartPointer 函数参数为智能指针
 * 4. AddObserver 使用lambda表达式和继承自vtkCommand
 */

#define TEST3

#ifdef TEST1

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <vector>

namespace {

class Test : public vtkObject
{
public:
    static Test* New();
    vtkTypeMacro(Test, vtkObject);

protected:
    Test()
    {
        std::cout << "construct\tref count:" << this->GetReferenceCount() << '\n'; // 1
    }

    ~Test() override
    {
        std::cout << "destruct\tref count:" << this->GetReferenceCount() << '\n'; // 0
    }

private:
    Test(const Test&)            = delete;
    Test& operator=(const Test&) = delete;
};

vtkStandardNewMacro(Test);
} // namespace

int main(int, char*[])
{
    {
        vtkNew<Test> t;

        t.Reset(); // 正确释放
        // t->Delete(); // 崩溃

        std::cout << "+++++\n";
    }

    std::cout << "-------------------------------\n";

    {
        vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();

        t = nullptr; // 正确释放
        // t->Delete(); // 崩溃

        std::cout << "++++++\n";
    }

    std::cout << "-------------------------------\n";

    {
        std::vector<vtkSmartPointer<Test>> ts;

        vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();

        std::cout << "t ref count:\t" << t->GetReferenceCount() << '\n'; // 1
        ts.emplace_back(t);
        std::cout << "t ref count:\t" << t->GetReferenceCount() << '\n';           // 2
        std::cout << "ts ref count:\t" << ts.front()->GetReferenceCount() << '\n'; // 2

        t = nullptr; // 引用计数减1，并不释放

        std::cout << "ts ref count:\t" << ts.front()->GetReferenceCount() << '\n'; // 1
        std::cout << "++++++\n";
    }

    std::cout << "-------------------------------\n";

    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

namespace {

class Test : public vtkObject
{
public:
    static Test* New();
    vtkTypeMacro(Test, vtkObject);

protected:
    Test()
    {
        std::cout << "construct\tref count:" << this->GetReferenceCount() << '\n'; // 1
    }

    ~Test() override
    {
        std::cout << "destruct\tref count:" << this->GetReferenceCount() << '\n'; // 0
    }

private:
    Test(const Test&)            = delete;
    Test& operator=(const Test&) = delete;
};

vtkStandardNewMacro(Test);
} // namespace

Test* Create()
{
    auto t = Test::New();
    return t;
}

vtkSmartPointer<Test> Create2()
{
    auto t = vtkSmartPointer<Test>::New();
    return t;
}

int main(int, char*[])
{
    {
        vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();
    }

    std::cout << "-------------------------\n";

    {
        vtkNew<Test> t;
    }

    std::cout << "-------------------------\n";

    {
        auto t = Test::New();
        t->Delete();
    }

    std::cout << "-------------------------\n";

    {
        std::cout << "+++\n";
        Create2();
        std::cout << "+++\n";
    }

    std::cout << "-------------------------\n";
    {
        std::cout << "+++\n";
        auto t = Create2();
        std::cout << "+++\n";
    }

    std::cout << "-------------------------\n";

    {
        // 不能正确释放
        auto t = Test::New();
    }

    std::cout << "-------------------------\n";

    {
        // 不能正确释放
        vtkSmartPointer<Test> t = Create();
    }

    std::cout << "-------------------------\n";

    {
        // 不能正确释放
        vtkSmartPointer<Test> t = Test::New();
    }

    return 0;
}
#endif // TEST2

#ifdef TEST3

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

namespace {

class Test : public vtkObject
{
public:
    static Test* New();
    vtkTypeMacro(Test, vtkObject);

protected:
    Test()
    {
        std::cout << "construct\tref count:" << this->GetReferenceCount() << '\n'; // 1
    }

    ~Test() override
    {
        std::cout << "destruct\tref count:" << this->GetReferenceCount() << '\n'; // 0
    }

private:
    Test(const Test&)            = delete;
    Test& operator=(const Test&) = delete;
};

vtkStandardNewMacro(Test);
} // namespace

void Param1(const vtkSmartPointer<Test>& t)
{
    // 传递vtkSmartPointer此处是1，传递vtkNew此处是2
    std::cout << "const vtkSmartPointer<Test>&\tref count\t" << t->GetReferenceCount() << '\n';
}

void Param2(vtkSmartPointer<Test> t)
{
    // 传递vtkSmartPointer和vtkNew此处都是2
    std::cout << "vtkSmartPointer<Test>\t\tref count\t" << t->GetReferenceCount() << '\n';
}

// vkObjectBase 有一个成员变量 std::atomic<int32_t> ReferenceCount;

// vtkNew           就好像是 std::unique_ptr
// vtkSmartPointer  就好像是 std::shared_ptr
// vtkNew 可以隐式转换为 vtkSmartPointer 
// vtkNew 更轻量

int main(int, char*[])
{
    std::cout << "-------------------------\n";
    {
        vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();
        Param1(t);
        std::cout << "+++++\n";
    }
    std::cout << "-------------------------\n";
    {
        vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();
        Param2(t);
        std::cout << "+++++\n";
    }
    std::cout << "-------------------------\n";
    {
        vtkNew<Test> t;
        Param1(t);
        std::cout << "+++++\n";
    }
    std::cout << "-------------------------\n";
    {
        vtkNew<Test> t;
        Param2(t);
        std::cout << "+++++\n";
    }
    std::cout << "-------------------------\n";

    return 0;
}
#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();
    vtkTypeMacro(MyCommand, vtkCommand);

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        std::cout << "vtkCommand\t" << caller << '\n';
    }
};

vtkStandardNewMacro(MyCommand);
} // namespace

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

    // renderer
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(cubeActor);
    renderer->ResetCamera();

    // RenderWindow
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(600, 600); // 设置window大小

    // RenderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    std::cout << "vtkRenderWindowInteractor\t" << iren << '\n';

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    // 继承vtkCommand设置回调函数
    vtkNew<MyCommand> cb1;
    iren->AddObserver(vtkCommand::RightButtonPressEvent, cb1);

    // 用lambda设置VTK的回调函数
    vtkNew<vtkCallbackCommand> cb2;
    cb2->SetCallback([](vtkObject* caller, unsigned long, void*, void*) { std::cout << "lambda\t\t" << caller << '\n'; });
    iren->AddObserver(vtkCommand::LeftButtonPressEvent, cb2);

    // 数据交互
    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST4
