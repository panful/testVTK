#include "Style.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


#include <iostream>
#include <thread>

vtkStandardNewMacro(Style);

//if (this->Interaction == NONE)
//{
//    std::cout << "none\n";
//}
//if (this->Interaction == PANNING)
//{
//    std::cout << "panning\n";
//}
//if (this->Interaction == ZOOMING)
//{
//    std::cout << "zooming\n";
//}
//if (this->Interaction == SELECTING)
//{
//    std::cout << "selecting\n";
//}
//
//std::cout << __FUNCTION__ << std::endl;

void Style::OnLeftButtonDown()
{
    Superclass::OnLeftButtonDown();
}

void Style::OnLeftButtonUp()
{
    Superclass::OnLeftButtonUp();
}

void Style::OnMiddleButtonDown()
{
    Superclass::OnMiddleButtonDown();
}

void Style::OnMiddleButtonUp()
{
    Superclass::OnMiddleButtonUp();
}

void Style::OnRightButtonDown()
{
    Superclass::OnRightButtonDown();

    if (this->Interaction == ROTATING)
    {
        m_rightStartPos = { this->Interactor->GetEventPosition()[0],this->Interactor->GetEventPosition()[1] };
        auto camera = this->CurrentRenderer->GetActiveCamera();
        vtkNew<vtkCamera> deepCamera;
        deepCamera->DeepCopy(camera);
        m_undoCameras.emplace(deepCamera);
    }
}

void Style::OnRightButtonUp()
{
    if (this->Interaction == ROTATING &&
        this->m_rightStartPos[0] == this->Interactor->GetEventPosition()[0] &&
        this->m_rightStartPos[1] == this->Interactor->GetEventPosition()[1])
    {
        std::cout << "pop\n";
        // 如果鼠标位置没有改变即没有做旋转动作，弹出栈顶元素
        m_undoCameras.pop();
    }

    Superclass::OnRightButtonUp();
}
  
void Style::OnMouseMove()
{
    Superclass::OnMouseMove();
}

void Style::OnMouseWheelForward()
{
    this->FindPokedRenderer(
        this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    if (!camera)
    {
        return;
    }

    vtkNew<vtkCamera> deepCamera;
    deepCamera->DeepCopy(camera);
    m_undoCameras.emplace(deepCamera);

    Superclass::OnMouseWheelForward();
}

void Style::OnMouseWheelBackward()
{
    this->FindPokedRenderer(
        this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    if (!camera)
    {
        return;
    }

    vtkNew<vtkCamera> deepCamera;
    deepCamera->DeepCopy(camera);
    m_undoCameras.emplace(deepCamera);

    Superclass::OnMouseWheelBackward();
}

void Style::UndoCamera()
{
    std::cout << "undo\t size:" << m_undoCameras.size() << "\n";
    if (m_undoCameras.empty())
    {
        return;
    }

    // 将回退的状态压入前进栈
    auto currentCamera = this->CurrentRenderer->GetActiveCamera();
    vtkNew<vtkCamera> deepCamera;
    deepCamera->DeepCopy(currentCamera);
    m_redoCameras.emplace(deepCamera);

    auto camera = m_undoCameras.top();
    m_undoCameras.pop();
    this->CurrentRenderer->ResetCamera();
    this->CurrentRenderer->SetActiveCamera(camera);
    this->Interactor->Render();
}

void Style::RedoCamera()
{
    std::cout << "redo\t size:" << m_redoCameras.size() << "\n";
    if (m_redoCameras.empty())
    {
        return;
    }

    // 将前进的状态压入回退栈
    auto currentCamera = this->CurrentRenderer->GetActiveCamera();
    vtkNew<vtkCamera> deepCamera;
    deepCamera->DeepCopy(currentCamera);
    m_undoCameras.emplace(deepCamera);

    auto camera = m_redoCameras.top();
    m_redoCameras.pop();
    this->CurrentRenderer->ResetCamera();
    this->CurrentRenderer->SetActiveCamera(camera);
    this->Interactor->Render();
}
