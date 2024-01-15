/**
 * 1. vtkInteractorEventRecorder 记录交互事件 播放交互事件
 * 2. vtkInteractorStyleJoystickCamera 长按左键一直旋转、右键一直缩放、中键一直平移，定时器的使用
 * 3. 当鼠标处于 vtkOrientationMarkerWidget 内部时，开启定时器旋转
 * 4. 读取事件记录文件并播放事件
 * 5. 定时器的使用
 */

#define TEST5

#ifdef TEST1

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

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
    renWin->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    vtkNew<vtkInteractorEventRecorder> recorder;
    recorder->SetInteractor(iren);
    recorder->SetFileName("record.log"); // 写入或读取文件名
    recorder->On();

#if (1)
    recorder->Record(); // 开始记录
#else
    recorder->ReadFromInputStringOff();
    recorder->Play(); // 开始播放
#endif

    renWin->Render();
    iren->Start();

    recorder->Stop(); // 结束记录/结束回放

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleJoystickCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace {
class Style : public vtkInteractorStyleJoystickCamera
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleJoystickCamera);

protected:
    void OnLeftButtonDown() override
    {
        // 会开启一个定时器，只要鼠标没有抬起就一直旋转
        // Win32 使用   WM_TIMER
        // Qt 使用      QTimer
        std::cout << "left down\n";
        Superclass::OnLeftButtonDown();
    }

    void Rotate() override
    {
        std::cout << "rotate\n";
        Superclass::Rotate();
    }

    void OnLeftButtonUp() override
    {
        // 销毁定时器
        std::cout << "left up\n";
        Superclass::OnLeftButtonUp();
    }

protected:
    Style()           = default;
    ~Style() override = default;
};

vtkStandardNewMacro(Style);
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
    renWin->SetSize(600, 600);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<Style> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
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

    // 鼠标是否处于widget内部
    bool GetPosInWidget(int X, int Y) const
    {
        double vp[4];
        this->Renderer->GetViewport(vp);

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

protected:
    MyWidget()
    {
        this->SetOutlineColor(1, 0, 0);
        this->OutlineActor->VisibilityOn();
    }

    // 屏蔽鼠标左键对widget的响应
    virtual void OnLeftButtonDown() override
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

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void SetWidget(const vtkSmartPointer<MyWidget>& w)
    {
        m_widget = w;
    }

protected:
    MyStyle()
    {
        // 开启定时器，鼠标左键按下旋转的时候通过定时器调用
        this->UseTimers = 1;
    }

    // 开启旋转
    void OnLeftButtonDown() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        if (m_widget && m_widget->GetPosInWidget(X, Y))
        {
            // 鼠标处于坐标轴所在区域(Widget)则开始旋转
            RotateAxesStartImpl();
        }

        Superclass::OnLeftButtonDown();
    }

    // 关闭旋转
    void OnLeftButtonUp() override
    {
        RotateAxesStopImpl();
        Superclass::OnLeftButtonUp();
    }

    void Rotate() override
    {
        int X = this->Interactor->GetEventPosition()[0];
        int Y = this->Interactor->GetEventPosition()[1];

        vtkRenderWindowInteractor* rwi = this->Interactor;

        // 鼠标移出坐标轴区域则停止旋转
        if (!m_widget || !m_widget->GetPosInWidget(X, Y))
        {
            std::cout << "outside\n";
            return;
        }

        if (this->CurrentRenderer == nullptr)
        {
            std::cout << "nullptr\n";
            return;
        }

        std::cout << "rotate\n";

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
    void RotateAxesStartImpl()
    {
        auto pos = this->Interactor->GetEventPosition();
        this->FindPokedRenderer(pos[0], pos[1]);
        this->StartRotate();
    }

    void RotateAxesStopImpl()
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
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(sphereSource->GetOutput());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 0, 0);

    vtkNew<vtkRenderer> renderer;
    renderer->SetLayer(0);
    renderer->AddActor(actor);
    renderer->ResetCamera();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->SetNumberOfLayers(2);
    renderWindow->SetSize(800, 600);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkNew<vtkAxesActor> axes;
    vtkNew<MyWidget> widget;
    widget->SetOutlineColor(0, 0, 1);
    widget->SetOrientationMarker(axes);
    widget->SetInteractor(renderWindowInteractor);
    widget->SetViewport(0.0, 0.0, 0.4, 0.4);
    widget->SetEnabled(1);
    widget->InteractiveOn();

    vtkNew<MyStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);
    style->SetWidget(widget);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}

#endif // TEST3

#ifdef TEST4

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <fstream>
#include <sstream>
#include <thread>

// 源码可以查看
// vtkInteractorEventRecorder.cxx [158] void vtkInteractorEventRecorder::Play()

namespace {

enum ModifierKey
{
    ShiftKey   = 1,
    ControlKey = 2,
    AltKey     = 4
};

class Style : public vtkInteractorStyleTrackballCamera
{
public:
    static Style* New();
    vtkTypeMacro(Style, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown() override
    {
        std::cout << "start play wheel event\n";

        if (this->Interactor)
        {
            char event[256] = {}, keySym[256] = {};
            int pos[2], ctrlKey, shiftKey, altKey, keyCode, repeatCount;

            std::string line;
            std::ifstream ifs("../resources/record.log");
            while (std::getline(ifs, line))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::istringstream iss(line);

                iss.imbue(std::locale::classic());

                iss.width(256);
                iss >> event;

                unsigned long ievent = vtkCommand::GetEventIdFromString(event);
                if (ievent != vtkCommand::NoEvent)
                {
                    iss >> pos[0];
                    iss >> pos[1];

                    // version >= 1.1
                    int m;
                    iss >> m;
                    shiftKey = (m & ModifierKey::ShiftKey) ? 1 : 0;
                    ctrlKey  = (m & ModifierKey::ControlKey) ? 1 : 0;
                    altKey   = (m & ModifierKey::AltKey) ? 1 : 0;

                    iss >> keyCode;
                    iss >> repeatCount;
                    iss >> keySym;

                    this->Interactor->SetEventPosition(pos);
                    this->Interactor->SetControlKey(ctrlKey);
                    this->Interactor->SetShiftKey(shiftKey);
                    this->Interactor->SetAltKey(altKey);
                    this->Interactor->SetKeyCode(static_cast<char>(keyCode));
                    this->Interactor->SetRepeatCount(repeatCount);
                    this->Interactor->SetKeySym(keySym);

                    this->Interactor->InvokeEvent(ievent, nullptr);
                }

                assert(iss.good() || iss.eof());
            }
        }

        Superclass::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(Style);
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

    vtkNew<Style> style;
    iren->SetInteractorStyle(style);

    renWin->Render();
    iren->Start();

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

namespace {
class MyCommand : public vtkCommand
{
public:
    static MyCommand* New();
    vtkTypeMacro(MyCommand, vtkCommand);

    virtual void Execute(vtkObject* caller, unsigned long, void*)
    {
        static int counter { 0 };
        std::cout << counter++ << '\n';
    }
};

vtkStandardNewMacro(MyCommand);

class MyStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static MyStyle* New();
    vtkTypeMacro(MyStyle, vtkInteractorStyleTrackballCamera);

    void OnLeftButtonDown() override
    {
        std::cout << "left button down\n";
        Superclass::OnLeftButtonDown();
    }
};

vtkStandardNewMacro(MyStyle);
} // namespace

int main()
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

    vtkNew<MyCommand> callback;

    vtkNew<MyStyle> style;
    style->AddObserver(vtkCommand::TimerEvent, callback);
    style->UseTimersOn();           // 使用定时器
    style->SetTimerDuration(10000); // 计时器间隔，毫秒为单位，修改默认值时要小心，可能会出问题

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle(style);

    renWin->Render();
    style->StartTimer(); // 会将当前交互状态修改为：VTKIS_TIMER，导致Style的某些默认交互事件不能执行
    iren->Start();

    return 0;
}

#endif // TEST5
