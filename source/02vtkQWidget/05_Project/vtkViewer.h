#pragma once

#include "iviewer.h"
#include "log.h"
#include "scene.h"

#include <QVTKRenderWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>

class WindowVTK : public QVTKRenderWidget
{
public:
    WindowVTK(QWidget* p = nullptr) : QVTKRenderWidget(p)
    {
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->setRenderWindow(renderWindow);

        m_renderer = vtkSmartPointer<vtkRenderer>::New();
        this->renderWindow()->AddRenderer(m_renderer);
    }

    void SetBackground(float* color)
    {
        LogDebug("");
        m_renderer->SetBackground(color[0], color[1], color[2]);
    }

    void FitView() const
    {
        LogDebug("");
        m_renderer->ResetCamera();
    }

    void AddActor(vtkProp* prop) const
    {
        LogDebug("");
        m_renderer->AddActor(prop);
    }

    void RemoveActor(vtkProp* prop) const
    {
        LogDebug("");
        m_renderer->RemoveActor(prop);
    }

    void Render() const
    {
        LogDebug("");
        this->renderWindow()->Render();
    }

private:
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
};
