#pragma once

#include "iviewer.h"
#include "scene.h"
#include <QVTKRenderWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>


class WindowVTK : public IViewer,
                  public QVTKRenderWidget
{
public:
    WindowVTK(QWidget* p = nullptr) : QVTKRenderWidget(p)
    {
        vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
        this->setRenderWindow(renderWindow);

        m_renderer = vtkSmartPointer<vtkRenderer>::New();
        this->renderWindow()->AddRenderer(m_renderer);
    }

    void SetBackground(float* color) override
    {
        m_renderer->SetBackground(color[0], color[1], color[2]);
        Update();
    }

    void AddActor(const ActorData* data) override
    {
    }

    void Update()
    {
        this->renderWindow()->Render();
    }

private:
    vtkSmartPointer<vtkRenderer> m_renderer { nullptr };
};
