#pragma once

#include "vtkViewer.h"

#include <vtkActor.h>
#include <vtkProp.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include <forward_list>
#include <iostream>
#include <map>

class Subject
{
public:
    Subject(WindowVTK* v) : m_observer(v)
    {
    }

    virtual ~Subject()
    {
    }

protected:
    void Notify() const
    {
        m_observer->Render();
    }

    WindowVTK* m_observer { nullptr };
};

class Scene 
{
public:
    Scene()
    {
    }

    void SetColor(uint32_t id, float* color) const
    {
        vtkActor::SafeDownCast(m_actors.at(id))->GetProperty()->SetColor(color[0], color[1], color[2]);
    }

    void SetVisible(uint32_t id, bool v) const
    {
        vtkActor::SafeDownCast(m_actors.at(id))->SetVisibility(v);
    }

    void AddActor(uint32_t id, vtkProp* prop)
    {
        m_actors.try_emplace(id, prop);
    }

private:
    std::map<uint32_t, vtkSmartPointer<vtkProp>> m_actors;
};
