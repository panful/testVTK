#include "window_vtk.h"

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

WindowVTK::WindowVTK()
{

    vtkCommand::LeftButtonPressEvent;
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
    //iren->Start();  // 死循环
}

bool WindowVTK::ProcessEvent(EventType t,int _x ,int _y, void* data)
{
    m_window->GetInteractor()->SetEventInformationFlipY(
        static_cast<int>(_x * 1.0 + 1.e-5),
        static_cast<int>(_y * 1.0 + 1.e-5));

    std::map<EventType, unsigned long> mapEvent{
        {EventType::LeftButtonPressEvent,                 vtkCommand::LeftButtonPressEvent},
        {EventType::LeftButtonReleaseEvent,               vtkCommand::LeftButtonReleaseEvent},
        {EventType::MiddleButtonPressEvent,               vtkCommand::MiddleButtonPressEvent},
        {EventType::MiddleButtonReleaseEvent,             vtkCommand::MiddleButtonReleaseEvent},
        {EventType::RightButtonPressEvent,                vtkCommand::RightButtonPressEvent},
        {EventType::RightButtonReleaseEvent,              vtkCommand::RightButtonReleaseEvent},
        {EventType::KeyPressEvent,                        vtkCommand::KeyPressEvent},
        {EventType::KeyReleaseEvent,                      vtkCommand::KeyReleaseEvent},
        {EventType::MouseWheelForwardEvent,               vtkCommand::MouseWheelForwardEvent},
        {EventType::MouseWheelBackwardEvent,              vtkCommand::MouseWheelBackwardEvent},
        {EventType::MouseMoveEvent,                       vtkCommand::MouseMoveEvent}
    };

    switch (t)
    {
    case EventType::None:
        break;
    case EventType::LeftButtonPressEvent:
    case EventType::LeftButtonReleaseEvent:
    case EventType::MiddleButtonPressEvent:
    case EventType::MiddleButtonReleaseEvent:
    case EventType::RightButtonPressEvent:
    case EventType::RightButtonReleaseEvent:
    case EventType::KeyPressEvent:
    case EventType::KeyReleaseEvent:
    case EventType::MouseWheelForwardEvent:
    case EventType::MouseWheelBackwardEvent:
    case EventType::MouseMoveEvent:
        m_window->GetInteractor()->InvokeEvent(mapEvent[t], data);
        break;
    default:
        break;
    }

    return true;
}

