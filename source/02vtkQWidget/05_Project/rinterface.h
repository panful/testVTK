#pragma once

#include "wmanager.h"
#include <memory>

class QWidget;

class RenderInterface
{
public:
    void CreateRenderWindow(QWidget* w) const
    {
        m_wManager->CreateRenderWindow(w);
    }

    void SetBackground(float*color) const
    {
        m_wManager->GetWindow(0)->SetBackground(color);
    }

    void AddGeometry()const
    {

    }

    void CreateMesh()const
    {
        
    }

private:
    std::unique_ptr<WindowManager> m_wManager { std::make_unique<WindowManager>() };
};