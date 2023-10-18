#pragma once

#include "iviewer.h"
#include "scene.h"
#include <QDebug>
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
        qDebug() << "VTKViewer::SetBackground";
        m_renderer->SetBackground(color[0], color[1], color[2]);
        Update();
    }

    void FitView() const
    {
        qDebug() << "VTKViewer::FitView";
        m_renderer->ResetCamera();
        Update();
    }

    void AddActor(vtkProp* prop) const
    {
        qDebug() << "VTKViewer::AddActor";
        m_renderer->AddActor(prop);
        Update();
    }

    void RemoveActor(vtkProp* prop) const
    {
        qDebug() << "VTKViewer::RemoveActor";
        m_renderer->RemoveActor(prop);
        Update();
    }

    void Update() const
    {
        this->renderWindow()->Render();
    }

private:
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
};
