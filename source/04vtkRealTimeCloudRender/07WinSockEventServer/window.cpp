#include "window.h"

#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleRubberBand3D.h>

#include <vtkCommand.h>

#include "socket.h"
#include <thread>

WindowVTK::WindowVTK()
{
    // polyData
    vtkNew<vtkCubeSource> cube;

    // mapper
    vtkNew<vtkPolyDataMapper> cubeMapper;
    cubeMapper->SetInputConnection(cube->GetOutputPort());

    // actor
    vtkNew<vtkActor> cubeActor;
    cubeActor->SetMapper(cubeMapper);
    cubeActor->GetProperty()->SetColor(0, 1, 0);

    // renderer
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->AddActor(cubeActor);
    m_renderer->SetBackground(.1, .2, .3);
    m_renderer->ResetCamera();

    // renderWindow
    m_window = vtkSmartPointer<vtkRenderWindow>::New();
    m_window->AddRenderer(m_renderer);
    m_window->SetSize(800, 600);

    // renderWindowInteractor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(m_window);

    vtkNew<vtkInteractorStyleRubberBand3D> style;
    iren->SetInteractorStyle(style);

    // render
    m_window->Render();
    //iren->Start();

    int i = 0;
}

void WindowVTK::MyStart()
{
    while (1)
    {
        auto ret = ServerSocket::GetInstance()->GetBuffer();
        if (ret.empty())
        {
            continue;
        }

        auto buffer = ret.c_str();
        char* type[5]{ 0 };
        char* x[5]{ 0 };
        char* y[5]{ 0 };

        memcpy(type, buffer, 4);
        buffer += 4;
        memcpy(x, buffer, 4);
        buffer += 4;
        memcpy(y, buffer, 4);

        auto type_ = *reinterpret_cast<long*>(type);
        auto x_ = *reinterpret_cast<int*>(x);
        auto y_ = *reinterpret_cast<int*>(y);

        std::cout << type_ << '\t' << x_ << '\t' << y_ << '\n';

        this->ProcessEvent(type_, x_, y_);
    }
}

bool WindowVTK::ProcessEvent(long t, int _x, int _y, void* data)
{
    m_window->GetInteractor()->SetEventInformationFlipY(
        static_cast<int>(_x * 1.0 + 1.e-5),
        static_cast<int>(_y * 1.0 + 1.e-5));

    std::map<long, unsigned long> mapEvent{
        { 1, vtkCommand::LeftButtonPressEvent},
        { 2, vtkCommand::LeftButtonReleaseEvent},
        { 3, vtkCommand::MiddleButtonPressEvent},
        { 4, vtkCommand::MiddleButtonReleaseEvent},
        { 5, vtkCommand::RightButtonPressEvent},
        { 6, vtkCommand::RightButtonReleaseEvent},
        { 7, vtkCommand::KeyPressEvent},
        { 8, vtkCommand::KeyReleaseEvent},
        { 9, vtkCommand::MouseWheelForwardEvent},
        { 10, vtkCommand::MouseWheelBackwardEvent},
        { 11, vtkCommand::MouseMoveEvent}
    };

    switch (t)
    {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        m_window->GetInteractor()->InvokeEvent(mapEvent[t], data);
        break;
    default:
        break;
    }

    return true;
}

