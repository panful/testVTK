#include "Style.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkVectorOperators.h"

#include <iostream>

#define MY_PRINT do{                      \
if (this->Interaction == ROTATING)          \
{                                          \
    std::cout << "rotating\n";              \
}                                          \
if (this->Interaction == NONE)              \
{                                          \
    std::cout << "none\n";                  \
}                                          \
if (this->Interaction == PANNING)          \
{                                          \
    std::cout << "panning\n";              \
}                                          \
if (this->Interaction == ZOOMING)          \
{                                          \
    std::cout << "zooming\n";              \
}                                          \
if (this->Interaction == SELECTING)          \
{                                          \
    std::cout << "selecting\n";              \
}}while (0);                              \


vtkStandardNewMacro(Style);

void Style::OnLeftButtonDown()
{
    switch (m_interactionMode)
    {
    case Style::InteractionMode::Rotate:
    {
        Superclass::OnRightButtonDown();
    }
    break;
    case Style::InteractionMode::Zoom:
    {
        Superclass::OnLeftButtonDown();
    }
    break;
    case Style::InteractionMode::Translation:
    {
        Superclass::OnMiddleButtonDown();
    }
    break;
    default:
    {
        Superclass::OnLeftButtonDown();
    }
    break;
    }
}

void Style::OnLeftButtonUp()
{
    switch (m_interactionMode)
    {
    case Style::InteractionMode::Rotate:
    {
        Superclass::OnRightButtonUp();
    }
    break;
    case Style::InteractionMode::Zoom:
    {
        Superclass::OnLeftButtonUp();
    }
    break;
    case Style::InteractionMode::Translation:
    {
        Superclass::OnMiddleButtonUp();
    }
    break;
    default:
    {
        Superclass::OnLeftButtonUp();
    }
    break;
    }
}

void Style::OnMiddleButtonDown()
{
    //Superclass::OnRightButtonDown();


    // 右键按下事件
    if (this->Interaction == NONE)
    {
        // ControlKey
        if (this->Interactor->GetShiftKey())
        {
            this->Interaction = ZOOMING;
        }
        else
        {
            this->Interaction = ROTATING;
        }
        this->FindPokedRenderer(
            this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
        this->InvokeEvent(vtkCommand::StartInteractionEvent);
    }
}

void Style::OnMiddleButtonUp()
{
    //MY_PRINT;
    //std::cout << __FUNCTION__ << std::endl;
    //Superclass::OnMiddleButtonUp();
    Superclass::OnRightButtonUp();
}

void Style::OnRightButtonDown()
{
    //std::cout << __FUNCTION__ << std::endl;
    Superclass::OnRightButtonDown();
    //Superclass::OnMiddleButtonDown();
}

void Style::OnRightButtonUp()
{
    //MY_PRINT;
    //std::cout << __FUNCTION__ << std::endl;
    Superclass::OnRightButtonUp();
    //Superclass::OnMiddleButtonUp();
}

void Style::OnMouseMove()
{
    //MY_PRINT
    //std::cout << __FUNCTION__ << std::endl;
    Superclass::OnMouseMove();
}

void Style::OnMouseWheelForward()
{
    //std::cout << __FUNCTION__ << std::endl;
    //Superclass::OnMouseWheelForward();

    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    ZoomByFocalPoint(mousePos, true);
    this->Interactor->Render();
}

void Style::OnMouseWheelBackward()
{

    //std::cout << __FUNCTION__ << std::endl;
    //Superclass::OnMouseWheelBackward();

    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    auto mousePos = vtkVector2d(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    ZoomByFocalPoint(mousePos, false);
    this->Interactor->Render();
}

void Style::OnChar()
{
    Superclass::OnChar();
}

void Style::ZoomByFocalPoint(const vtkVector2d& focalPoint, bool zoomIn)
{
    TranslateCameraToFocalPoint(focalPoint, true);
    if (zoomIn)
    {
        Superclass::OnMouseWheelForward();
    }
    else
    {
        Superclass::OnMouseWheelBackward();
    }
    TranslateCameraToFocalPoint(focalPoint, false);
}

void Style::TranslateCameraToFocalPoint(const vtkVector2d& point, bool forwardTo)
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

    if (this->CurrentRenderer == nullptr)
    {
        return;
    }

    if (this->CurrentRenderer->GetActiveCamera()->GetParallelProjection())
    {
        auto camera = this->CurrentRenderer->GetActiveCamera();
        const int* size = this->CurrentRenderer->GetSize();
        const int* origin = this->CurrentRenderer->GetOrigin();
        const auto& focalPoint = point;

        const vtkVector2d rbCenter2d = vtkVector2d(focalPoint.GetX(), focalPoint.GetY());
        const vtkVector3d rbCenter3d = vtkVector3d(rbCenter2d.GetX(), rbCenter2d.GetY(), 0.0);
        const vtkVector3d worldRBCenter = this->GetCurrentRenderer()->DisplayToWorld(rbCenter3d);

        const vtkVector2d winCenter2d = vtkRecti(origin[0], origin[1], size[0], size[1]).GetCenter();
        const vtkVector3d winCenter3d = vtkVector3d(winCenter2d.GetX(), winCenter2d.GetY(), 0.0);
        const vtkVector3d worldWinCenter = this->GetCurrentRenderer()->DisplayToWorld(winCenter3d);
        const vtkVector3d translation = worldRBCenter - worldWinCenter;

        vtkVector3d pos, fp;
        camera->GetPosition(pos.GetData());
        camera->GetFocalPoint(fp.GetData());

        if (forwardTo)
        {
            pos = pos + translation;
            fp = fp + translation;
        }
        else
        {
            pos = pos - translation;
            fp = fp - translation;
        }

        camera->SetPosition(pos.GetData());
        camera->SetFocalPoint(fp.GetData());
    }
}

void Style::SetInteractionMode(InteractionMode mode)
{
    if (m_interactionMode != mode)
    {
        m_interactionMode = mode;
    }
}
