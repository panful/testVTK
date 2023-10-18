#pragma once

#include "actor.h"
#include "entity.h"
#include "geometry.h"
#include "infilter.h"
#include "iviewer.h"
#include "wmanager.h"

#include <iostream>
#include <map>
#include <memory>

class QWidget;

class RenderInterface
{
public:
    void CreateRenderWindow(QWidget* w) const
    {
        m_winManager->CreateRenderWindow(w);
    }

    void SetColor(Entity e, float* color) const
    {
        m_winManager->GetActiveViewer()->SetColor(m_actManager->GetId(e), color);
    }

    template <typename VT, typename IT>
    void AddGeometry(int id, const std::vector<VT>& v, const std::vector<IT>& i) const
    {
        m_geoManager->SetVertices(id, v);
        m_geoManager->SetIndices(id, i);
    }

    Entity CreateMesh(int id)
    {
        auto filter = new MeshFilter(m_geoManager->GetGeometry(id));
        auto entity = EntityManager::Instance()->Create();

        m_actManager->AddActor(entity, filter->GetActorData());
        m_winManager->GetActiveViewer()->AddActor(m_actManager->GetActor(entity));

        return entity;
    }

    void ShowEntity(Entity e) const
    {
        m_winManager->GetActiveViewer()->SetVisible(m_actManager->GetId(e), true);
    }

    void HideEntity(Entity e) const
    {
        m_winManager->GetActiveViewer()->SetVisible(m_actManager->GetId(e), false);
    }

private:
    std::unique_ptr<WindowManager> m_winManager { std::make_unique<WindowManager>() };
    std::unique_ptr<GeometryManager> m_geoManager { std::make_unique<GeometryManager>() };
    std::unique_ptr<ActorManager> m_actManager { std::make_unique<ActorManager>() };
};