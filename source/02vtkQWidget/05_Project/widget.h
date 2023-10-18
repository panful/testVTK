#pragma once

#include "controller.h"
#include "vtkViewer.h"
#include <QBoxLayout>
#include <QDebug>
#include <QPushButton>
#include <QWidget>
#include <random>

class Window
{
public:
    Window(QWidget* w) : m_parentWindow(w), m_controller(new Controller())
    {
    }

    virtual IViewer* GetIViewer() const
    {
        return m_controller;
    }

    QWidget* GetParentWidget() const
    {
        return m_parentWindow;
    }

protected:
    Controller* m_controller { nullptr };

private:
    QWidget* m_parentWindow { nullptr };
};

class Decorator : public Window
{
public:
    Decorator(Window* w) : Window(w->GetParentWidget()), m_window(w)
    {
    }

protected:
    Window* m_window { nullptr };
};

class WindowUi : public Decorator
{
public:
    WindowUi(Window* w) : Decorator(w)
    {
        auto btnLayout = new QHBoxLayout();
        {
            auto btn_fitView    = new QPushButton("FitView");
            auto btn_background = new QPushButton("SetBackground");

            btnLayout->addWidget(btn_fitView);
            btnLayout->addWidget(btn_background);

            QObject::connect(btn_fitView, &QPushButton::clicked, [this]() { m_controller->FitView(); });
            QObject::connect(btn_background, &QPushButton::clicked,
                [this]()
                {
                    static std::default_random_engine engine;
                    std::uniform_real_distribution<float> dist(0.f, 1.f);
                    float color[3] { dist(engine), dist(engine), dist(engine) };
                    m_controller->SetBackground(color);
                });
        }

        auto mainLayout = new QVBoxLayout();

        mainLayout->addLayout(btnLayout);
        mainLayout->addWidget(m_controller->GetWidget());

        m_window->GetParentWidget()->setLayout(mainLayout);
    }
};