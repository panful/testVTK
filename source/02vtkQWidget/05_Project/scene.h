// #pragma once

// #include "vtkViewer.h"
// #include <forward_list>
// #include <map>
// #include <vtkActor.h>
// #include <vtkProp.h>
// #include <vtkProperty.h>
// #include <vtkSmartPointer.h>

// class Subject
// {
// public:
//     void Attach(ViewerVTK* v)
//     {
//         m_observers.emplace_after(m_observers.end(), v);
//     }

//     void Detach(ViewerVTK* v)
//     {
//         m_observers.remove(v);
//     }

// protected:
//     void Notity()
//     {
//         for (const auto& obs : m_observers)
//         {
//             obs->Update();
//         }
//     }

// private:
//     std::forward_list<ViewerVTK*> m_observers;
// };

// class Scene : public Subject
// {
// public:
//     void SetColor(uint32_t id, float* color)
//     {
//         vtkActor::SafeDownCast(m_actors[id])->GetProperty()->SetColor(color[0], color[1], color[2]);
//         Notify();
//     }

// private:
//     std::map<uint32_t, vtkSmartPointer<vtkProp>> m_actors;
// };

// struct Actor
// {
// };

// class ViewerVTK : public IViewer,
//                   public Scene
// {
// public:
//     void AddActor();
//     void RemoveActor();
//     void SetColor();
//     void SetVisible();
// };