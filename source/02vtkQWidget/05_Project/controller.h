#pragma once

#include "outfilter.h"
#include "scene.h"
#include "vtkViewer.h"

class Controller : public IViewer
{
public:
    Controller()
    {
        m_viewer = new WindowVTK();
        m_scene  = new Scene(m_viewer);
    }

    QWidget* GetWidget() const
    {
        return m_viewer;
    }

    void SetBackground(float* color) override
    {
        m_viewer->SetBackground(color);
    }

    void FitView() override
    {
        m_viewer->FitView();
    }

    void RemoveActor(const ActorData*) override
    {
    }

    void AddActor(const ActorData* actor) override
    {
        if (actor)
        {
            m_scene->AddActor(actor->id, VTKFilter::Convert(actor));
        }
    }

    void SetColor(uint32_t id, float* color) override
    {
        m_scene->SetColor(id, color);
    }

    void SetVisible(uint32_t id, bool v) override
    {
        m_scene->SetVisible(id, v);
    }

private:
    Scene* m_scene;
    WindowVTK* m_viewer;
};