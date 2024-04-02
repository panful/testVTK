
/**
 * 101. 引用计数 主动释放vtk智能指针
 * 102. vtk智能指针的使用
 * 103. vtkNew 和 vtkSmartPointer 函数参数为智能指针
 *
 * 201. AddObserver 使用lambda表达式和继承自vtkCommand
 * 202. vtk的观察者模式 AddObserver 实现方法
 * 203. vtk AddObserver 的执行流程
 *
 * 301. vtkTimeStamp 记录数据修改的时间 vtkObject::Modified()
 *
 * 401. vtkNew<vtkActor> 实际调用 vtkActor 的子类 vtkOpenGLActor
 * 402. class中的成员变量是裸指针，引用计数仍会加1，并不会被析构
 * 403. 从 vtkActor vtkPolyDataMapper... 继承重写父类函数
 *
 * 501. vtkFloatArray vtkDoubleArray 的 SetArray 函数使用
 * 502. vtkPoints 使用 SetData 函数设置顶点数据
 *
 * vtkSmartPointer vtkObject vtkCommand vtkTimeStamp vtkDataArray vtkDataObject vtkAlgorithm
 */

#define TEST502

#ifdef TEST101

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

        std::cout << "t ref count:\t" << t->GetReferenceCount() << '\n';           // 1
        ts.emplace_back(t);
        std::cout << "t ref count:\t" << t->GetReferenceCount() << '\n';           // 2
        std::cout << "ts ref count:\t" << ts.front()->GetReferenceCount() << '\n'; // 2

        t = nullptr;                                                               // 引用计数减1，并不释放

        std::cout << "ts ref count:\t" << ts.front()->GetReferenceCount() << '\n'; // 1
        std::cout << "++++++\n";
    }

    std::cout << "-------------------------------\n";

    return 0;
}
#endif // TEST101

#ifdef TEST102

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
#endif // TEST102

#ifdef TEST103

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
#endif // TEST103

#ifdef TEST201

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

#endif // TEST201

#ifdef TEST202

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
#endif // TEST202

#ifdef TEST203

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        std::cout << "Left button down\n";
        Superclass::OnLeftButtonDown(); // 内部会调用vtkCommand::StartInteractionEvent的回调函数
    }
};

vtkStandardNewMacro(MyStyle);

//-----------------------------------------------------------------
class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();
    vtkTypeMacro(MyCommand, vtkCommand);

    // 通过AddObserver()添加回调后，当对应的事件被执行，该函数就会被调用
    void Execute(vtkObject* caller, unsigned long, void*) override
    {
        std::cout << "Callback\n";
    }
};

vtkStandardNewMacro(MyCommand);
} // namespace

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetSize(800, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<MyStyle> style;
    iren->SetInteractorStyle(style);

    // 给指定事件添加一个回调
    // 子类会继承父类的所有事件，如果对象没有该事件，添加的回调就不会被调用
    vtkNew<MyCommand> callback;
    style->AddObserver(vtkCommand::StartInteractionEvent, callback);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST203

#ifdef TEST301

#include <vtkCubeSource.h>
#include <vtkObjectFactory.h>

// void vtkObject::Modified() 调用修改时间
// vtkObject的成员变量 vtkTimeStamp MTime; 用来跟踪修改时间

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    std::cout << source->GetMTime() << std::endl;
    source->SetXLength(2.0); // 内部会调用 Modify()
    std::cout << source->GetMTime() << std::endl;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    source->SetYLength(2.0);
    source->Update();
    std::cout << source->GetMTime() << std::endl;
    source->SetYLength(2.0); // 和上一次设置的值一样，不会修改 MTime
    source->Update();        // MTime和上一次的一样，所以此处并不会真的Update
    std::cout << source->GetMTime() << std::endl;

    return 0;
}

#endif // TEST301

#ifdef TEST401

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

#endif // TEST401

#ifdef TEST402

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

#endif // TESt402

#ifdef TEST403

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class MyActor : public vtkOpenGLActor
{
public:
    static MyActor* New();
};

vtkStandardNewMacro(MyActor);
} // namespace

// 如果MyActor直接从vtkActor继承，则不能正常绘制
// 因为vtkNew<vtkActor>实际上new的是一个vtkOpenGLActor，详见TEST7的说明
// vtkOpenGLActor重写了Render()函数，继承vtkActor时Render()函数不能正常调用
// 类似的还有vtkPolyDataMapper vtkCamera等等，和OpenGL有关的都要注意继承的父类

int main(int, char*[])
{
    vtkNew<vtkCubeSource> source;
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(source->GetOutput());

    vtkNew<MyActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0, 1, 0);

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

#endif // TEST403

#ifdef TEST501

#include <iostream>
#include <vector>
#include <vtkIntArray.h>

int main()
{
    {
        vtkNew<vtkIntArray> arr;
        arr->InsertNextValue(1);
        arr->InsertNextValue(2);
        arr->InsertNextValue(3);

        std::cout << "A::number: " << arr->GetNumberOfValues() << "\t data:";

        for (vtkIdType i = 0; i < arr->GetNumberOfValues(); ++i)
        {
            std::cout << arr->GetValue(i) << ' ';
        }
    }

    {
        vtkNew<vtkIntArray> arr;
        std::vector<int> vec { 1, 2, 3 };

        // 最后一个参数save设置为1，表示vtk不负责释放数据，并不是意味着vtk会拷贝vec的数据到arr中
        arr->SetArray(vec.data(), static_cast<vtkIdType>(vec.size()), 1);

        std::cout << "\nB::number: " << arr->GetNumberOfValues() << "\t data:";

        for (vtkIdType i = 0; i < arr->GetNumberOfValues(); ++i)
        {
            std::cout << arr->GetValue(i) << ' ';
        }
    }

    {
        vtkNew<vtkIntArray> arr;
        {
            std::vector<int> vec { 1, 2, 3 };
            // vec离开这个作用域就会被释放
            arr->SetArray(vec.data(), static_cast<vtkIdType>(vec.size()), 1);
        }

        std::cout << "\nC::number: " << arr->GetNumberOfValues() << "\t data:";

        for (vtkIdType i = 0; i < arr->GetNumberOfValues(); ++i)
        {
            // 打印的值是非法值，因为vec已经被释放，并且vec的值并没有深拷贝到arr中
            // 如果SetArray的最后一个参数save设置为0，此处会崩溃，因为对已经释放的内存（vec）重复释放
            std::cout << arr->GetValue(i) << ' ';
        }
    }

    {
        {
            vtkNew<vtkIntArray> arr;
            {
                std::vector<int> vec { 1, 2, 3 };
                vtkNew<vtkIntArray> tmp;
                tmp->SetArray(vec.data(), static_cast<vtkIdType>(vec.size()), 1);
                arr->DeepCopy(tmp); // 深拷贝一份数据到arr
            }

            std::cout << "\nD::number: " << arr->GetNumberOfValues() << "\t data:";

            for (vtkIdType i = 0; i < arr->GetNumberOfValues(); ++i)
            {
                // 打印的值为预期值
                std::cout << arr->GetValue(i) << ' ';
            }
        }
    }

    {
        vtkNew<vtkIntArray> arr;
        {
            std::vector<int> vec { 1, 2, 3 };
            arr->SetVoidArray(vec.data(), static_cast<vtkIdType>(vec.size()), 1); // 和 SetArray 效果一样
        }

        std::cout << "\nE::number: " << arr->GetNumberOfValues() << "\t data:";

        for (vtkIdType i = 0; i < arr->GetNumberOfValues(); ++i)
        {
            std::cout << arr->GetValue(i) << ' ';
        }
    }
}

#endif // TEST501

#ifdef TEST502

#include <iostream>
#include <vector>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>

int main()
{
    {
        vtkNew<vtkPoints> points;
        points->InsertNextPoint(1., 2., 3.);
        points->InsertNextPoint(4., 5., 6.);

        std::cout << "Number: " << points->GetNumberOfPoints() << '\n';
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
        {
            double pt[3] {};
            points->GetPoint(i, pt);
            std::cout << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
        }
    }

    {
        std::vector<double> vec { 1., 2., 3., 4., 5., 6. };

        vtkNew<vtkDoubleArray> arr;
        arr->SetNumberOfComponents(3); // 必须设置为3，顶点包含{x,y,z}，否则VTK会打印ERROR级别的错误日志
        arr->SetArray(vec.data(), static_cast<vtkIdType>(vec.size()), 1);

        vtkNew<vtkPoints> points;
        points->SetData(arr);
        points->SetDataTypeToDouble(); // 必须和 vtkXXXArray 的类型一致，如果类型不一致，points会是空的

        std::cout << "Number: " << points->GetNumberOfPoints() << '\n';
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
        {
            double pt[3] {};
            points->GetPoint(i, pt);
            std::cout << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
        }
    }
}

#endif // TEST502
