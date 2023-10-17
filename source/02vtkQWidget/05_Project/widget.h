#pragma once

#include "vtkViewer.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QWidget>

class Window
{
public:
    Window(QWidget* w) : m_base(w)
    {
    }

    virtual QWidget* GetWidget() const
    {
        return m_base;
    }

private:
    QWidget* m_base { nullptr };
};

class Decorator : public Window
{
public:
    Decorator(Window* w) : Window(w->GetWidget()), m_window(w)
    {
    }

    QWidget* GetWidget() const override = 0;

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
            auto btn1 = new QPushButton("TEST1");
            auto btn2 = new QPushButton("TEST2");
            auto btn3 = new QPushButton("TEST3");

            btnLayout->addWidget(btn1);
            btnLayout->addWidget(btn2);
            btnLayout->addWidget(btn3);
        }

        auto mainLayout = new QVBoxLayout();
        m_present       = new WindowVTK();

        mainLayout->addLayout(btnLayout);
        mainLayout->addWidget(m_present);

        m_window->GetWidget()->setLayout(mainLayout);
    }

    QWidget* GetWidget() const
    {
        return m_present;
    }

private:
    QWidget* m_present { nullptr };
};