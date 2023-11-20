
/**
 * 1. 引用计数 主动释放vtk智能指针
 * 2. vtk智能指针的使用
 * 3. vtkNew 和 vtkSmartPointer 函数参数为智能指针
 * 4. AddObserver 使用lambda表达式和继承自vtkCommand
 * 5. vtk的观察者模式 AddObserver 实现方法
 * 6. vtkTimeStamp 记录数据修改的时间
 * 7. New基类，调用子类的函数
 * 8. class中的成员变量是裸指针，引用计数仍会加1，并不会被析构
 *
 * vtkSmartPointer vtkObject vtkCommand vtkTimeStamp vtkDataArray vtkDataObject vtkAlgorithm
 */

#define TEST2

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

#include <vector>
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

Test* Create3()
{
    vtkNew<Test> t;
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

    std::cout << "-------------------------\n";

    {
        std::cout << "+++\n";
        auto t = Create3();
        std::cout << "+++\n";
        // std::vector<vtkSmartPointer<Test>> ts;
        // ts.emplace_back(t); // 崩溃，Create3返回的裸指针已经被析构了
        std::cout << "+++\n";
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

#ifdef TEST5

#include <iostream>
#include <map>

enum class Event
{
    Event_0,
    Event_1,
    Event_2,
};

class Command
{
public:
    void Execute(void* data)
    {
        std::cout << "Execute: " << *reinterpret_cast<int*>(data) << '\n';
    }
};

class Object
{
public:
    void AddObserve(Event e, Command* cmd)
    {
        m_observes.try_emplace(e, cmd);
    }

    void Update()
    {
        std::cout << "Invoke Event 0\n";
        InvokeEvent(Event::Event_0);
        std::cout << "Invoke Event 1\n";
        InvokeEvent(Event::Event_1);
        std::cout << "Invoke Event 2\n";
        InvokeEvent(Event::Event_2);
    }

private:
    void InvokeEvent(Event e)
    {
        m_observes[e]->Execute(&e);
    }

private:
    std::map<Event, Command*> m_observes;
};

int main()
{
    Object obj;
    obj.AddObserve(Event::Event_0, new Command());
    obj.AddObserve(Event::Event_1, new Command());
    obj.AddObserve(Event::Event_2, new Command());

    obj.Update();
}
#endif // TEST5

#ifdef TEST6

#include <vtkCubeSource.h>

// void vtkObject::Modified() 调用修改时间
// vtkObject的成员变量 vtkTimeStamp MTime; 用来跟踪修改时间

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    std::cout << source->GetMTime() << std::endl;
    source->SetXLength(2.0);
    std::cout << source->GetMTime() << std::endl;
    std::cout << source->GetMTime() << std::endl;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    std::cout << source->GetMTime() << std::endl;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    std::cout << source->GetMTime() << std::endl;

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>

#define Log_Func std::cout << __FUNCTION__ << std::endl;

class BaseRenderer
{
public:
    static BaseRenderer* New();

    virtual void Render()
    {
        Log_Func;
    }

    void Test()
    {
        Log_Func;
        this->AddActor();
    }

private:
    virtual void AddActor()
    {
        Log_Func;
    }
};

class OpenGLRenderer : public BaseRenderer
{
public:
    void Render() override
    {
        Log_Func;
    }

    void AddActor() override
    {
        Log_Func;
    }
};

class VulkanRenderer : public BaseRenderer
{
public:
    void Render() override
    {
        Log_Func;
    }

    void AddActor() override
    {
        Log_Func;
    }
};

/**
 * vtkObjectFactory.h 定义了以下的宏
 * vtkStandardNewMacro
 * vtkObjectFactoryNewMacro
 *
 * 通过 VTK_MODULE_INIT 在 main()之前初始化VTK模块并注册方法，New()基类的时候，返回对应的子类实例
 * 用户看似New的是一个基类，实际执行过程中执行的是子类的函数
 */

#define Render_Type_OpenGL

int main()
{
    auto p = BaseRenderer::New();
    p->Render();
    p->Test();

    return 0;
}

BaseRenderer* BaseRenderer::New()
{
#ifdef Render_Type_OpenGL
    return new OpenGLRenderer();
#else
    return new VulkanRenderer();
#endif
}

#endif // TEST7

#ifdef TEST8

#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

#include <iostream>

namespace {

class Test : public vtkObject
{
public:
    static Test* New();
    vtkTypeMacro(Test, vtkObject);

protected:
    Test()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~Test() override
    {
        std::cout << __FUNCTION__ << std::endl;
    }
};

vtkStandardNewMacro(Test);

class Test2 : public vtkObject
{
public:
    static Test2* New();
    vtkTypeMacro(Test2, vtkObject);

    void SetTest(Test* t)
    {
        // 详见 vtkSetGet.h => vtkSetObjectBodyMacro
        if (m_test != t)
        {
            auto tmp = m_test;
            m_test   = t;
            if (m_test)
            {
                // 将当前设置的成员变量引用计数加一
                m_test->Register(this);
            }
            if (tmp)
            {
                // 将原来的成员变量引用计数减一
                tmp->UnRegister(this);
            }
            this->Modified();
        }
    }

    Test* GetTest() const noexcept
    {
        return m_test;
    }

protected:
    Test2()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    ~Test2() override
    {
        std::cout << __FUNCTION__ << std::endl;
        if (m_test)
        {
            m_test->UnRegister(this);
            m_test = nullptr;
        }
    }

private:
    Test* m_test { nullptr };
};

vtkStandardNewMacro(Test2);
} // namespace

void Func(Test2* test2)
{
    // 正常情况下，如果t离开了作用域（Func()函数执行完毕）t就会被析构
    // 但是既想让t不析构，又想让Test2中保存的Test类型成员变量是裸指针
    // 可以在SetTest()函数中调用t的Register函数，让引用计数加1
    // Test2析构的时候需要调用UnRegister函数，将引用计数减1
    vtkSmartPointer<Test> t = vtkSmartPointer<Test>::New();
    std::cout << t << '\t' << t->GetReferenceCount() << '\n';
    test2->SetTest(t);
    std::cout << t << '\t' << t->GetReferenceCount() << '\n';
}

int main()
{
    {
        vtkSmartPointer<Test2> test2 = vtkSmartPointer<Test2>::New();
        Func(test2);
        std::cout << test2->GetTest() << '\t' << test2->GetTest()->GetReferenceCount() << '\n';
    }
}

#endif // TESt8